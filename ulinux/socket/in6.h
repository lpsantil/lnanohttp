#ifndef ULINUX_SOCKET_IN6_H
#define ULINUX_SOCKET_IN6_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
/* network order is big endian */
struct ulinux_in6_addr {
	union {
		ulinux_u8	u6_addr8[16];
		ulinux_u16	u6_addr16[8];
		ulinux_u32	u6_addr32[4];
	} in6_u;
#define s6_addr	  in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 in6_u.u6_addr32
};

struct ulinux_sockaddr_in6 {
	ulinux_us		sin6_family;    	/* AF_INET6 */
	/* Transport layer port #, big endian */
	ulinux_u16		sin6_port;
	/* IPv6 flow information, big endian */
	ulinux_u32		sin6_flowinfo;
	struct ulinux_in6_addr	sin6_addr;		/* IPv6 address */
	/* scope id (new in RFC2553), endian not defined */
	ulinux_u32		sin6_scope_id;
}; /* the sockaddr size is actually 16 bytes... then no need to pad */
#endif
