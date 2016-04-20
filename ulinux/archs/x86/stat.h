#ifndef ULINUX_ARCH_STAT_H
#define ULINUX_ARCH_STAT_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
struct ulinux_stat {
  ulinux_ul dev;
  ulinux_ul ino;
  ulinux_us mode;
  ulinux_us nlink;

  ulinux_us uid;
  ulinux_us gid;
  ulinux_ul rdev;
  ulinux_ul sz;
  ulinux_ul blk_sz;
  ulinux_ul blks;

  ulinux_ul atime;
  ulinux_ul atime_nsec;
  ulinux_ul mtime;
  ulinux_ul mtime_nsec;
  ulinux_ul ctime;
  ulinux_ul ctime_nsec;
  ulinux_ul __unused4;
  ulinux_ul __unused5;
};
#endif
