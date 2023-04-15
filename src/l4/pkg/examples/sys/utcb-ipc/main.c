/**
 * \file
 * \brief Low-level example of communication.
 *
 * This example shows how two threads can exchange data using the L4 IPC
 * mechanism.
 */
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>,
 *               Björn Döbel <doebel@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */
#include <l4/sys/ipc.h>
#include <l4/sys/thread.h>
#include <l4/sys/factory.h>
#include <l4/sys/utcb.h>
#include <l4/sys/task.h>
#include <l4/sys/vcon.h>
#include <l4/re/env.h>
#include <l4/re/c/util/cap_alloc.h>
#include <l4/re/c/util/kumem_alloc.h>
#include <l4/util/thread.h>

#include <stdio.h>
#include <string.h>

static unsigned char stack2[8 << 10] __attribute__((aligned(8)));
static l4_cap_idx_t thread1_cap, thread2_cap;

static void vlogprintn(const char *s, int l)
{
  if (l > L4_VCON_WRITE_SIZE)
    l = L4_VCON_WRITE_SIZE;

  l4_vcon_send(L4_BASE_LOG_CAP, s, l);
}

static void vlogprint(const char *s)
{
  vlogprintn(s, strlen(s));
}

static void vlogprintc(const char c)
{
  vlogprintn(&c, 1);
}

static void thread1(void)
{
  l4_msg_regs_t *mr = l4_utcb_mr();
  l4_msgtag_t tag;
  int i, j;

  printf("Thread1 up (%p)\n", l4_utcb());

  for (i = 0; i < 10; i++)
    {
      for (j = 0; j < L4_UTCB_GENERIC_DATA_SIZE; j++)
        mr->mr[j] = 'A' + (i + j) % ('~' - 'A' + 1);
      tag = l4_msgtag(0, L4_UTCB_GENERIC_DATA_SIZE, 0, 0);
      if (l4_msgtag_has_error(l4_ipc_send(thread2_cap, l4_utcb(), tag, L4_IPC_NEVER)))
	printf("IPC-send error\n");
    }

  mr->mr[0] = 1;
  if (l4_msgtag_has_error(l4_ipc_send(thread2_cap, l4_utcb(), tag, L4_IPC_NEVER)))
    printf("IPC-send error\n");

  printf("Thread1 done\n");
}

L4UTIL_THREAD_STATIC_FUNC(thread2)
{
  l4_msgtag_t tag;
  l4_msg_regs_t mr;
  unsigned i;

  // No printf() here because this would require a working pthread environment!
  vlogprint("Thread2 up\n");

  while (1)
    {
      if (l4_msgtag_has_error(tag = l4_ipc_receive(thread1_cap, l4_utcb(), L4_IPC_NEVER)))
        vlogprint("IPC receive error\n");
      memcpy(&mr, l4_utcb_mr(), sizeof(mr));
      if (mr.mr[0] == 1) // exit notification
        break;
      vlogprint("Thread2 receive: ");
      for (i = 0; i < l4_msgtag_words(tag); i++)
        vlogprintc((char)mr.mr[i]);
      vlogprint("\n");
    }

  vlogprint("Thread2 done, switching to thread1\n");
  if (l4_msgtag_has_error(l4_ipc_send(thread1_cap, l4_utcb(),
                                      tag, L4_IPC_NEVER)))
    vlogprint("IPC-send error\n");

  // In theory this could hit if the above IPC send operation doesn't switch
  // to the other thread.
  __builtin_trap();
}

int main(void)
{
  l4_msgtag_t tag;

  thread1_cap = l4re_env()->main_thread;
  thread2_cap = l4re_util_cap_alloc();

  if (l4_is_invalid_cap(thread2_cap))
    {
      printf("Cannot allocate thread2 capability\n");
      return 1;
    }

  tag = l4_factory_create_thread(l4re_env()->factory, thread2_cap);
  if (l4_error(tag))
    {
      printf("Cannot create thread2\n");
      return 2;
    }

  l4_addr_t kumem;
  if (l4re_util_kumem_alloc(&kumem, 0, L4RE_THIS_TASK_CAP, l4re_env()->rm))
    {
      printf("Cannot allocate UTCB for thread2\n");
      return 3;
    }

  l4_thread_control_start();
  l4_thread_control_pager(l4re_env()->rm);
  l4_thread_control_exc_handler(l4re_env()->rm);
  l4_thread_control_bind((l4_utcb_t *)kumem, L4RE_THIS_TASK_CAP);
  tag = l4_thread_control_commit(thread2_cap);
  if (l4_error(tag))
    {
      printf("Cannot set thread2 thread parameters\n");
      return 4;
    }

  tag = l4_thread_ex_regs(thread2_cap,
                          (l4_umword_t)thread2,
                          (l4_umword_t)(stack2 + sizeof(stack2)), 0);
  if (l4_error(tag))
    {
      printf("Cannot set thread2 IP/SP\n");
      return 5;
    }

  l4_sched_param_t sp = l4_sched_param(1, 0);
  tag = l4_scheduler_run_thread(l4re_env()->scheduler, thread2_cap, &sp);
  if (l4_error(tag))
    {
      printf("Cannot start thread2\n");
      return 6;
    }

  thread1();

  if (l4_msgtag_has_error(l4_ipc_receive(thread2_cap, l4_utcb(),
                                         L4_IPC_NEVER)))
    printf("IPC-receive error\n");

  l4_task_unmap(L4RE_THIS_TASK_CAP,
                l4_obj_fpage(thread2_cap, 0, L4_FPAGE_RWX),
                L4_FP_ALL_SPACES);

  printf("Terminated thread2. Terminating.\n");
  return 0;
}
