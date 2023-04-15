#pragma once

#include <l4/sys/compiler.h>
#include <lwip/tcpip.h>

#include <stdio.h>

L4_INLINE void lwip_util_print_ip(ip_addr_t *ip)
{
  printf("%d.%d.%d.%d", ip4_addr1(&ip->u_addr.ip4), ip4_addr2(&ip->u_addr.ip4),  /* MMMadd */
                        ip4_addr3(&ip->u_addr.ip4), ip4_addr4(&ip->u_addr.ip4));  /* MMMadd */
}
