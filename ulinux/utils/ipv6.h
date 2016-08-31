#ifndef ULINUX_UTILS_IPV6
#define ULINUX_UTILS_IPV6
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
static inline ulinux_u8 ulinux_ipv6_addr_any(struct ulinux_in6_addr *a)
{
	return ((a->s6_addr32[0] | a->s6_addr32[1] | a->s6_addr32[2]
						| a->s6_addr32[3]) == 0); 
}

static inline ulinux_u8 ulinux_ipv6_addr_loopback(struct ulinux_in6_addr *a)
{
	return ((a->s6_addr32[0] | a->s6_addr32[1] | a->s6_addr32[2]
				| (a->s6_addr32[3] ^ ulinux_cpu2be32(1))) == 0);
}

static inline ulinux_u8 ulinux_ipv6_addr_v4mapped(struct ulinux_in6_addr *a)
{
	return ((a->s6_addr32[0] | a->s6_addr32[1]
		| (a->s6_addr32[2] ^ ulinux_cpu2be32(0x0000ffff))) == 0);
}

static inline ulinux_u8 ulinux_ipv6_addr_is_isatap(struct ulinux_in6_addr *addr)
{
	return ((addr->s6_addr32[2] | ulinux_cpu2be32(0x02000000))
						== ulinux_cpu2be32(0x02005EFE));
}
#endif
