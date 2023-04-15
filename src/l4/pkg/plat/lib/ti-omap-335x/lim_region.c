/**
 * Copyright (C) 2021 lili
 * Author: lili
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 *
 */

#include <stdio.h>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <l4/sys/types.h>
#include <l4/io/io.h>
#include <plat/io.h>
#include <l4/re/c/rm.h>

#include <l4/sys/syscall_defs.h>

/* Watchdog timer registers */
struct wd_timer {
    unsigned int resv1[4];
    unsigned int wdtwdsc;   /* offset 0x010 */
    unsigned int wdtwdst;   /* offset 0x014 */
    unsigned int wdtwisr;   /* offset 0x018 */
    unsigned int wdtwier;   /* offset 0x01C */
    unsigned int wdtwwer;   /* offset 0x020 */
    unsigned int wdtwclr;   /* offset 0x024 */
    unsigned int wdtwcrr;   /* offset 0x028 */
    unsigned int wdtwldr;   /* offset 0x02C */
    unsigned int wdtwtgr;   /* offset 0x030 */
    unsigned int wdtwwps;   /* offset 0x034 */
    unsigned int resv2[3];
    unsigned int wdtwdly;   /* offset 0x044 */
    unsigned int wdtwspr;   /* offset 0x048 */
    unsigned int resv3[1];
    unsigned int wdtwqeoi;  /* offset 0x050 */
    unsigned int wdtwqstar; /* offset 0x054 */
    unsigned int wdtwqsta;  /* offset 0x058 */
    unsigned int wdtwqens;  /* offset 0x05C */
    unsigned int wdtwqenc;  /* offset 0x060 */
    unsigned int resv4[39];
    unsigned int wdt_unfr;  /* offset 0x100 */
};
/* Watchdog Timer */
#define WDT_BASE            0x44E35000

L4_INLINE void
l4_priv_op_arm_call(unsigned long op,
                     unsigned long addr,
                     unsigned long val)
{
  register unsigned long _op    __asm__ ("r0") = op;
  register unsigned long _addr __asm__ ("r1") = addr;
  register unsigned long _val   __asm__ ("r2") = val;

  __asm__ __volatile__
    ("mov     lr, pc                \n\t"
     "mov     pc, %[sc]             \n\t"
       :
    "=r" (_op),
    "=r" (_addr),
    "=r" (_val)
       :
       [sc] "i" (L4_SYSCALL_PRIV_OP),
    "0" (_op),
    "1" (_addr),
    "2" (_val)
       :
    "cc", "memory", "lr"
       );
}

typedef unsigned long      ddekit_addr_t;

#define AM33XX_CM_BASE		0x44e00000
#define AM33XX_SCM_BASE		0x44E10000
#define AM33XX_SCM_LEN		0x2400
#define AM33XX_RTC_BASE		0x44E3E000
#define RG1_LEN             0x40000

#define AM33XX_CPSW_BASE    0x4A100000
#define RG2_LEN             0x8000

#define AM33XX_EMIF0_BASE   0x4C000000
#define RG3_LEN             0x8000

#define OFFS    (AM33XX_L4_WK_IO_OFFSET + RG1_LEN + RG2_LEN + RG3_LEN)

#define  CNTRL_VADDR        (AM33XX_SCM_BASE + AM33XX_L4_WK_IO_OFFSET)
#define  CNTRL_LEN          AM33XX_SCM_LEN

typedef struct {
                resource_size_t     p;
                unsigned long       sz;
                resource_size_t     v;
} rgn_t;
static rgn_t rgn[] = {
                {AM33XX_CM_BASE,    RG1_LEN, 0},
                {AM33XX_CPSW_BASE,  RG2_LEN, 0},
                {AM33XX_EMIF0_BASE, RG3_LEN, 0},
                {0x48002000,         0x6000, 0},            //L4_PER Peripheral Memory (Peripheral clock)
                {0x48022000,         0xA000, 0},            //uart2:48022000  I2C1
                {0x48030000,         0x1000, 0},            //spi0
                {0x48040000,         0xD000, 0},            //dmtimer2~7 gpio1
                {0x48080000,         0x10000, 0},           //ELM
                {0x480C8000,         0x1000,  0},           //mailbox
                {0x4819C000,         0x13000, 0},           //i2c2 uart3~5 gpio2~3 spi1
                {0x48200000,         0x1000,  0},           //Interrupt controller (INTCPS)
                {0x49000000,         0x8000,  0},           //tpcc
                {0x49800000,         0x2000,  0},           //tptc
                {0x49900000,         0x2000,  0},           //tptc
                {0x49A00000,         0x2000,  0},           //tptc
                {0x4A300000,         0x40000,  0},          //ICSS
                {0x50000000,         0x1000000,  0},        //gpmc 16M
                {0x8000000,          0x10000000, 0},        //gpmc 256M
                {0x48060000,         0x2000, 0},            //mmc0 MMCHS0 8K
                {0x481D8000,         0x2000, 0},            //mmc1 8K
                {0x47810000,         0x11000, 0},           //mmc2 MMCHS2 64K; for start is 0x47810100 in am33xx_mmc2_addr_space
                {0x481AC000,         0x1000, 0},            //gpio2 
                {0x481AE000,         0x1000, 0},            //gpio3 
    };


L4_CV L4_INLINE int
l4re_rm_reserve_area(l4_addr_t *start, unsigned long size,
                     unsigned flags, unsigned char align) L4_NOTHROW;
void lim_raw_writeb(unsigned char *a, unsigned char v);
void lim_raw_writew(unsigned short *a, unsigned short v);
void lim_raw_writel(unsigned int *a, unsigned int v);
void __init lim_init(void);
extern  void __iomem *ioremap(unsigned long phys_addr, unsigned long size);
extern int mck_insert_region(ddekit_addr_t pa, ddekit_addr_t va, unsigned int size);

static rgn_t * fnd_r(resource_size_t start, resource_size_t n)
{
    unsigned int i;
    for (i = 0; i < sizeof(rgn)/sizeof(rgn[0]); i++){
        if (start >= rgn[i].p && start + n <= rgn[i].p + rgn[i].sz)
            return &rgn[i];
    }
    return NULL;
}

struct resource * lim_request_mem_region(resource_size_t * p_start, resource_size_t n,
								            const char *name, int flags)
{
    int error;
    rgn_t *p;
    resource_size_t start = *p_start;
    struct resource *res = container_of(p_start, struct resource, start);

	if (ioremap(start, n))
	     return (struct resource *)res;
    if ((p = fnd_r(start, n))){
        l4re_rm_reserve_area((l4_addr_t*) & p->v, p->sz, L4RE_RM_F_SEARCH_ADDR, L4_PAGESHIFT);     //reserve virt-addr
        if ((error = l4io_request_iomem_region(p->p, p->v, p->sz,
                                                L4IO_MEM_EAGER_MAP | L4IO_MEM_USE_RESERVED_AREA)) < 0) {
            printf("Error lim_request_mem_region: area mapping failed: %d, p->p:%x, p->v:%x, sz:%lx\n", 
                                                                            error, p->p, p->v, p->sz);
            return NULL;
        }
        mck_insert_region(p->p, p->v, p->sz);
        printf("lim_request_mem_region: region pa=%x, va=%x, ln=%lx, start:%x\n", p->p, p->v, p->sz, start);
    } else {
        printf("lim_request_mem_region: out of region pa=%x, ln=%x, name:%s\n", start, n, name);
        return _request_mem_region(start, n, name);
    }

    return (struct resource *)res;
}

void hw_watchdog_disable(void);
void hw_watchdog_disable(void)
{
#define writel(v, a)    *(volatile unsigned int __force  *)a = v
#define readl(a)    *(volatile unsigned int __force  *)a
    struct wd_timer *wdt = (struct wd_timer *)ioremap(WDT_BASE, sizeof(struct wd_timer));

    /*
     * Disable watchdog
     */
    writel(0xAAAA, &wdt->wdtwspr);
    while (readl(&wdt->wdtwwps) != 0x0)
        ;
    writel(0x5555, &wdt->wdtwspr);
    while (readl(&wdt->wdtwwps) != 0x0)
        ;
}

void __init lim_init(void)
{
extern  void ankh_dummy_init(int dummy);
    unsigned int i;
    for (i = 0; i < sizeof(rgn)/sizeof(rgn[0]); i++){
        rgn[i].v = rgn[i].p + AM33XX_L4_WK_IO_OFFSET ;
        lim_request_mem_region(& rgn[i].p, rgn[i].sz, 0, 0);      //pre-request-all
    }
    printf("MMM--lim_ini-\n");
    
    ankh_dummy_init(0);
    hw_watchdog_disable();

}

void lim_raw_writeb(unsigned char *a, unsigned char v)
{
    if (((unsigned long)a < CNTRL_VADDR) || ((unsigned long)a >= (CNTRL_VADDR + CNTRL_LEN))){
        *(volatile unsigned char __force  *)a = v;
    } else
        l4_priv_op_arm_call(0, (unsigned long)a - CNTRL_VADDR, (unsigned long)v); 
}

void lim_raw_writew(unsigned short *a, unsigned short v)
{
    if (((unsigned long)a < CNTRL_VADDR) || ((unsigned long)a >= (CNTRL_VADDR + CNTRL_LEN))){
        *(volatile unsigned short __force  *)a = v;
    } else
        l4_priv_op_arm_call(1, (unsigned long)a - CNTRL_VADDR, (unsigned long)v); 
}

void lim_raw_writel(unsigned int *a, unsigned int v)
{
    if (((unsigned long)a < CNTRL_VADDR) || ((unsigned long)a >= (CNTRL_VADDR + CNTRL_LEN))){
        *(volatile unsigned int __force  *)a = v;
    } else
        l4_priv_op_arm_call(2, (unsigned long)a - CNTRL_VADDR, (unsigned long)v); 

}

dde_process_initcall(lim_init);

