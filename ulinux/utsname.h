#ifndef ULINUX_UTSNAME_H
#define ULINUX_UTSNAME_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ULINUX_UTS_LEN 64

struct ulinux_utsname {
	ulinux_u8 sysname[ULINUX_UTS_LEN + 1];
	ulinux_u8 nodename[ULINUX_UTS_LEN + 1];
	ulinux_u8 release[ULINUX_UTS_LEN + 1];
	ulinux_u8 version[ULINUX_UTS_LEN + 1];
	ulinux_u8 machine[ULINUX_UTS_LEN + 1];
	ulinux_u8 domainname[ULINUX_UTS_LEN + 1];
};
#endif