PKGDIR          ?= ..
L4DIR           ?= $(PKGDIR)/../..

TARGET	         = liblwip.a liblwip.so
PC_FILENAME      = lwip
REQUIRES_LIBS    = libpthread
CONTRIB_INCDIR   = lwip \
                   lwip/contrib/src/include \
                   lwip/contrib/src/include/ipv4 \
                   lwip/contrib/src/include/ipv6
DEFINES          = -DLWIP_L4=1
SRC_C            = arch/sys_arch.c \
                   arch/perf.c \
	               contrib/src/core/altcp.c \
	               contrib/src/core/altcp_alloc.c \
	               contrib/src/core/altcp_tcp.c \
	               contrib/src/core/def.c \
	               contrib/src/core/dns.c \
	               contrib/src/core/inet_chksum.c \
	               contrib/src/core/init.c \
	               contrib/src/core/ip.c \
	               contrib/src/core/ipv4/autoip.c \
	               contrib/src/core/ipv4/dhcp.c \
	               contrib/src/core/ipv4/etharp.c \
	               contrib/src/core/ipv4/icmp.c \
	               contrib/src/core/ipv4/igmp.c \
	               contrib/src/core/ipv4/ip4.c \
	               contrib/src/core/ipv4/ip4_addr.c \
	               contrib/src/core/ipv4/ip4_frag.c \
	               contrib/src/core/ipv6/dhcp6.c \
	               contrib/src/core/ipv6/ethip6.c \
	               contrib/src/core/ipv6/icmp6.c \
	               contrib/src/core/ipv6/inet6.c \
	               contrib/src/core/ipv6/ip6.c \
	               contrib/src/core/ipv6/ip6_addr.c \
	               contrib/src/core/ipv6/ip6_frag.c \
	               contrib/src/core/ipv6/mld6.c \
	               contrib/src/core/ipv6/nd6.c \
	               contrib/src/core/mem.c \
	               contrib/src/core/memp.c \
	               contrib/src/core/netif.c \
	               contrib/src/core/pbuf.c \
	               contrib/src/core/raw.c \
	               contrib/src/core/stats.c \
	               contrib/src/core/sys.c \
	               contrib/src/core/tcp.c \
	               contrib/src/core/tcp_in.c \
	               contrib/src/core/tcp_out.c \
	               contrib/src/core/timeouts.c \
	               contrib/src/core/udp.c \
	               contrib/src/netif/bridgeif.c \
	               contrib/src/netif/bridgeif_fdb.c \
	               contrib/src/netif/ethernet.c \
	               contrib/src/netif/lowpan6.c \
	               contrib/src/netif/lowpan6_ble.c \
	               contrib/src/netif/lowpan6_common.c \
	               contrib/src/netif/ppp/auth.c \
	               contrib/src/netif/ppp/ccp.c \
	               contrib/src/netif/ppp/chap-md5.c \
	               contrib/src/netif/ppp/chap-new.c \
	               contrib/src/netif/ppp/chap_ms.c \
	               contrib/src/netif/ppp/demand.c \
	               contrib/src/netif/ppp/eap.c \
	               contrib/src/netif/ppp/ecp.c \
	               contrib/src/netif/ppp/eui64.c \
	               contrib/src/netif/ppp/fsm.c \
	               contrib/src/netif/ppp/ipcp.c \
	               contrib/src/netif/ppp/ipv6cp.c \
	               contrib/src/netif/ppp/lcp.c \
	               contrib/src/netif/ppp/magic.c \
	               contrib/src/netif/ppp/mppe.c \
	               contrib/src/netif/ppp/multilink.c \
	               contrib/src/netif/ppp/polarssl/arc4.c \
	               contrib/src/netif/ppp/polarssl/des.c \
	               contrib/src/netif/ppp/polarssl/md4.c \
	               contrib/src/netif/ppp/polarssl/md5.c \
	               contrib/src/netif/ppp/polarssl/sha1.c \
	               contrib/src/netif/ppp/ppp.c \
	               contrib/src/netif/ppp/pppapi.c \
	               contrib/src/netif/ppp/pppcrypt.c \
	               contrib/src/netif/ppp/pppoe.c \
	               contrib/src/netif/ppp/pppol2tp.c \
	               contrib/src/netif/ppp/pppos.c \
	               contrib/src/netif/ppp/upap.c \
	               contrib/src/netif/ppp/utils.c \
	               contrib/src/netif/ppp/vj.c \
	               contrib/src/netif/slipif.c \
	               contrib/src/netif/zepif.c \
	               contrib/src/api/api_lib.c \
	               contrib/src/api/api_msg.c \
	               contrib/src/api/err.c \
	               contrib/src/api/if_api.c \
	               contrib/src/api/netbuf.c \
	               contrib/src/api/netdb.c \
	               contrib/src/api/netifapi.c \
	               contrib/src/api/sockets.c \
	               contrib/src/api/tcpip.c \
	               contrib/src/apps/altcp_tls/altcp_tls_mbedtls.c \
	               contrib/src/apps/altcp_tls/altcp_tls_mbedtls_mem.c \
	               contrib/src/apps/http/altcp_proxyconnect.c \
	               contrib/src/apps/http/fs.c \
	               contrib/src/apps/http/http_client.c \
	               contrib/src/apps/http/httpd.c \
	               contrib/src/apps/lwiperf/lwiperf.c \
	               contrib/src/apps/mdns/mdns.c \
	               contrib/src/apps/mqtt/mqtt.c \
	               contrib/src/apps/netbiosns/netbiosns.c \
	               contrib/src/apps/smtp/smtp.c \
	               contrib/src/apps/snmp/snmp_asn1.c \
	               contrib/src/apps/snmp/snmp_core.c \
	               contrib/src/apps/snmp/snmp_mib2.c \
	               contrib/src/apps/snmp/snmp_mib2_icmp.c \
	               contrib/src/apps/snmp/snmp_mib2_interfaces.c \
	               contrib/src/apps/snmp/snmp_mib2_ip.c \
	               contrib/src/apps/snmp/snmp_mib2_snmp.c \
	               contrib/src/apps/snmp/snmp_mib2_system.c \
	               contrib/src/apps/snmp/snmp_mib2_tcp.c \
	               contrib/src/apps/snmp/snmp_mib2_udp.c \
	               contrib/src/apps/snmp/snmp_msg.c \
	               contrib/src/apps/snmp/snmp_netconn.c \
	               contrib/src/apps/snmp/snmp_pbuf_stream.c \
	               contrib/src/apps/snmp/snmp_raw.c \
	               contrib/src/apps/snmp/snmp_scalar.c \
	               contrib/src/apps/snmp/snmp_snmpv2_framework.c \
	               contrib/src/apps/snmp/snmp_snmpv2_usm.c \
	               contrib/src/apps/snmp/snmp_table.c \
	               contrib/src/apps/snmp/snmp_threadsync.c \
	               contrib/src/apps/snmp/snmp_traps.c \
	               contrib/src/apps/snmp/snmpv3.c \
	               contrib/src/apps/snmp/snmpv3_mbedtls.c \
	               contrib/src/apps/sntp/sntp.c \
	               contrib/src/apps/tftp/tftp_server.c \



include $(L4DIR)/mk/lib.mk

CFLAGS += -Wno-unused-function -Wno-address
