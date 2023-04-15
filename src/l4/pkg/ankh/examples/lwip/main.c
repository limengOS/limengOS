#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread-l4.h>
#include <fcntl.h>

#include <l4/util/util.h>
#include <l4/ankh/client-c.h>
#include <l4/ankh/lwip-ankh.h>
#include <l4/ankh/session.h>
#include <l4/re/c/mem_alloc.h>
#include <l4/re/c/rm.h>
#include <l4/crypto/rsaref2/global.h>
#include <l4/crypto/rsaref2/rsaref.h>

/* 
 * Need to include this file before others.
 * Sets our byteorder.
 */
#include "arch/cc.h"

#include <unistd.h>
#include <netdb.h>

#include "netif/etharp.h"
#include <lwip-util.h>
#include "lwip/dhcp.h"

#define GETOPT_LIST_END { 0, 0, 0, 0 }

enum options
{
	BUF_SIZE,
	SHM_NAME,
};

/* Network interface variables */
ip_addr_t ipaddr, netmask, gw;
struct netif netif;
/* Set network address variables */

struct netif my_netif;
extern err_t ankhif_init(struct netif*);

static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };


static void do_lookup(char *hostname)
{
	ip_addr_t i;
    struct hostent *h = lwip_gethostbyname(hostname);

	printf("gethostbyname(%s): %s\n", hostname, h ? "SUCCESS" : "FAILED");
	if (h) {
		//i.addr = *(u32_t*)h->h_addr_list[0];
		i.u_addr.ip4.addr = *(u32_t*)h->h_addr_list[0];     /* MMMadd */
		printf("   h_name: '%s'\n", h->h_name);
		printf("   --> "); lwip_util_print_ip(&i); printf("\n");
	}
}

static void dns_test(void)
{
	do_lookup("os.inf.tu-dresden.de");
	do_lookup("www.heise.de");
	do_lookup("www.twitter.com");
	do_lookup("www.tudos.org");
	do_lookup("www.ifsr.de");
	do_lookup("www.dynamo-dresden.de");
	do_lookup("www.spiegel.de");
	do_lookup("www.smoking-gnu.de");
	do_lookup("www.fail.fx");
}

static void prnt_md5(unsigned char* buf, unsigned int len)
{
    unsigned char digest[32];
    unsigned int digestLen, i;

    R_DigestBlock(digest, &digestLen, buf, len, DA_MD5);
    printf("md5sum = ");
    for(i = 0; i < digestLen; i++)
        printf("%02x", digest[i]);
    printf("\n");
}

static void  test_w_file(char *fn, unsigned char *d, int ln)
{
   int fd;
   fd = open(fn, O_CREAT|O_RDWR, S_IRWXU);
   if (fd >= 0) {
        write(fd, d, ln);
        close(fd);
   }
}

static unsigned char * read_file(char *fn, int *l)
{
    int fd;
    unsigned char *b;
    int b_ln = 0x8000;
    int ln;

    printf("to read file :%s\n", fn);

    fd = open(fn, 0, 0);
    if (fd >= 0){
        b = malloc(b_ln);
        memset(b, 0, b_ln);
        ln = read(fd, b, b_ln);
        printf("read file ret len = %d\n", ln);
        close(fd);
        *l = ln;
        return b;
    }
    return 0;
}

static void test_filesystem(void)
{
    unsigned char *b;
    int ln = 0;
    mkdir("/sys/test_new_dir", 0777);
    b = read_file("rom/L4Re.html", &ln);
    if (b){
        test_w_file("/sys/test_new_dir/new.html", b, ln);
        free(b);
        b = read_file("/sys/test_new_dir/new.html", &ln);
        prnt_md5(b, ln);
        free(b);
    }
}

static void server(void)
{
	int err, fd;
	struct sockaddr_in in, clnt;
	in.sin_family = AF_INET;
	in.sin_port = htons(3000);
	//in.sin_addr.s_addr = my_netif.ip_addr.addr;
	in.sin_addr.s_addr = my_netif.ip_addr.u_addr.ip4.addr;      /* MMMadd */

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	printf("socket created: %d\n", sock);

	err = bind(sock, (struct sockaddr*)&in, sizeof(in));
	printf("bound to addr: %d\n", err);

	err = listen(sock, 10);
	printf("listen(): %d\n", err);

	for (;;) {
		char buf[1024];
		socklen_t clnt_size = sizeof(clnt);
		fd = accept(sock, (struct sockaddr*)&clnt, &clnt_size);

		printf("Got connection from  ");
		lwip_util_print_ip((ip_addr_t*)&clnt.sin_addr); printf("\n");

		err = read(fd, buf, sizeof(buf));
		printf("Read from fd %d (err %d)\n", fd, err);
		printf("%s", buf);
	}
}

#define USE_DHCP    1
void httpd_init(void);

int main(int argc, char **argv)
{
#if LWIP_IPV4
#if USE_DHCP
IP_ADDR4(&gw, 0,0,0,0);
IP_ADDR4(&ipaddr, 0,0,0,0);
IP_ADDR4(&netmask, 0,0,0,0);
#else
IP_ADDR4(&gw, 192,168,8,1);
IP_ADDR4(&ipaddr, 192,168,8,36);
IP_ADDR4(&netmask, 255,255,255,0);
#endif
#endif
	if (l4ankh_init())
	  return 1;

	l4_cap_idx_t c = pthread_l4_cap(pthread_self());
	cfg.send_thread = c;

	static struct option long_opts[] = {
		{"bufsize", 1, 0, BUF_SIZE },
		{"shm", 1, 0, SHM_NAME },
		GETOPT_LIST_END
	};

	while (1) {
		int optind = 0;
		int opt = getopt_long(argc, argv, "b:s:", long_opts, &optind);
		printf("getopt: %d\n", opt);

		if (opt == -1)
			break;

		switch(opt) {
			case BUF_SIZE:
				printf("buf size: %d\n", atoi(optarg));
				cfg.bufsize = atoi(optarg);
				break;
			case SHM_NAME:
				printf("shm name: %s\n", optarg);
				snprintf(cfg.shm_name, CFG_SHM_NAME_SIZE, "%s", optarg);
				break;
			default:
				break;
		}
	}

	// Start the TCP/IP thread & init stuff
	tcpip_init(NULL, NULL);
    memset(&my_netif, 0, sizeof(struct netif));
	struct netif *n = netif_add(&my_netif,
	                            ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw),
	                            &cfg, // configuration state
	                            ankhif_init, ethernet_input);

	printf("netif_add: %p (%p)\n", n, &my_netif);
	assert(n == &my_netif);

    netif_set_link_up(&my_netif);
    netif_set_up(&my_netif);

#if USE_DHCP
	printf("dhcp_start()\n");
	dhcp_start(&my_netif);
	printf("dhcp started\n");

	while (!dhcp_supplied_address(&my_netif))
		l4_sleep(100);
#endif
	printf("Network interface is up.\n");

	printf("IP: "); lwip_util_print_ip(&my_netif.ip_addr); printf("\n");
	printf("GW: "); lwip_util_print_ip(&my_netif.gw); printf("\n");

//	dns_test();

    httpd_init();

    test_filesystem();

	server();

	return 0;
}
