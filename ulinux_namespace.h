#ifndef ULINUX_NAMESPACE_H
#define ULINUX_NAMESPACE_H
/* this code is protected by the GNU affero GPLv3
   author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
                           <sylware AT legeek dot net> */
#define loop for(;;)
#define sl ulinux_sl
#define si ulinux_si
#define ul ulinux_ul
#define u8 ulinux_u8
#define u16 ulinux_u16
#define s16 ulinux_s16
#define u32 ulinux_u32
#define u64 ulinux_u64
/*----------------------------------------------------------------------------*/
#define timeval ulinux_timeval
/*----------------------------------------------------------------------------*/
#define cpu2be16 ulinux_cpu2be16
#define cpu2be32 ulinux_cpu2be32
/*----------------------------------------------------------------------------*/
#define memset(a,b,c) ulinux_memset((void*)a,b,c)
#define snprintf(a,b,c,...) ulinux_snprintf(a,b,c,##__VA_ARGS__)
/*----------------------------------------------------------------------------*/
#define ISERR ULINUX_ISERR
#define EAGAIN ULINUX_EAGAIN
#define EINTR ULINUX_EINTR
#define ECONNABORTED ULINUX_ECONNABORTED
/*----------------------------------------------------------------------------*/
#define read(a,b,c) ulinux_sysc(read,3,a,b,c)
#define close(a) ulinux_sysc(close,1,a)
#define exit(a) ulinux_sysc(exit_group,1,a)
#define sendfile(a,b,c,d) ulinux_sysc(sendfile,4,a,b,c,d)
#define write(a,b,c) ulinux_sysc(write,3,a,b,c)
#define chroot(a) ulinux_sysc(chroot,1,a)
#define chdir(a) ulinux_sysc(chdir,1,a)
/*----------------------------------------------------------------------------*/
#define O_RDONLY ULINUX_O_RDONLY
#define open(path,flgs,more) ulinux_sysc(open,3,path,flgs,more)
/*----------------------------------------------------------------------------*/
#define stat ulinux_stat
#define fstat(fd,stat) ulinux_sysc(fstat,2,fd,stat)
/*----------------------------------------------------------------------------*/
#define PAGE_SZ ULINUX_PAGE_SZ
#define PRIVATE ULINUX_MAP_PRIVATE
#define ANONYMOUS ULINUX_MAP_ANONYMOUS
#define RD ULINUX_PROT_READ
#define WR ULINUX_PROT_WRITE
#ifdef __i386__
#define mmap(a,b,c) ulinux_sysc(mmap2,6,0,a,b,c,0,0)
#else
#define mmap(a,b,c) ulinux_sysc(mmap,6,0,a,b,c,0,0)
#endif
#define FD_SET_ULS_N ULINUX_FD_SET_ULS_N
#ifdef __i386__
#define select(a,b,c,d,e) ulinux_sysc(_newselect,5,a,b,c,d,e)
#else
#define select(a,b,c) ulinux_sysc(select,5,a,b,c,d,e)
#endif
/*----------------------------------------------------------------------------*/
#define SIG_BLOCK ULINUX_SIG_BLOCK
#define SFD_NONBLOCK ULINUX_SFD_NONBLOCK
#define SIGCHLD ULINUX_SIGCHLD
#define SIGTERM ULINUX_SIGTERM
#define signalfd4(a,b,c,d) ulinux_sysc(signalfd4,4,a,b,c,d)
#define signalfd_siginfo ulinux_signalfd_siginfo
#define rt_sigprocmask(a,b,c,d) ulinux_sysc(rt_sigprocmask,4,a,b,c,d)
/*----------------------------------------------------------------------------*/
#define EPOLLET ULINUX_EPOLLET
#define EPOLLIN ULINUX_EPOLLIN
#define EPOLLPRI ULINUX_EPOLLPRI
#define EPOLLERR ULINUX_EPOLLERR
#define EPOLLHUP ULINUX_EPOLLHUP
#define EPOLL_CTL_ADD ULINUX_EPOLL_CTL_ADD
#define epoll_create1(a) ulinux_sysc(epoll_create1,1,a)
#define epoll_event ulinux_epoll_event
#define epoll_ctl(a,b,c,d) ulinux_sysc(epoll_ctl,4,a,b,c,d)
#define epoll_wait(a,b,c,d) ulinux_sysc(epoll_wait,4,a,b,c,d)
/*----------------------------------------------------------------------------*/
#define AF_INET ULINUX_AF_INET
#define AF_INET6 ULINUX_AF_INET6
#define INADDR_ANY ULINUX_INADDR_ANY
#define SOL_SOCKET ULINUX_SOL_SOCKET
#define SO_REUSEADDR ULINUX_SO_REUSEADDR
#define SOCK_STREAM ULINUX_SOCK_STREAM
#define SOCK_NONBLOCK ULINUX_SOCK_NONBLOCK
#ifdef IPV6
#define sockaddr_in ulinux_sockaddr_in
#else
#define sockaddr_in6 ulinux_sockaddr_in6
#endif
#ifdef __i386__
static sl socket(sl a, sl b, sl c)
{
	sl args[3];

	args[0] = a;
	args[1] = b;
	args[2] = c;

	return ulinux_sysc(socketcall, 2, ULINUX_SYS_SOCKET, (sl)&args[0]);
}
#else
#define socket(a,b,c) ulinux_sysc(socket,3,a,b,c)
#endif
#ifdef __i386__
static sl setsockopt_wrap(sl a, sl b, sl c, sl d, sl e)
{
	sl args[5];

	args[0] = a;
	args[1] = b;
	args[2] = c;
	args[3] = d;
	args[4] = e;

	return ulinux_sysc(socketcall, 2, ULINUX_SYS_SETSOCKOPT, (sl)&args[0]);
}
#define setsockopt(a,b,c,d,e) setsockopt_wrap(a,b,c,(sl)d,e)
#else
#define setsockopt(a,b,c,d,e) ulinux_sysc(setsockopt,5,a,b,c,d,e)
#endif
#ifdef __i386__
static sl bind_wrap(sl a, sl b, sl c)
{
	sl args[3];

	args[0] = a;
	args[1] = b;
	args[2] = c;

	return ulinux_sysc(socketcall, 2, ULINUX_SYS_BIND, (sl)&args[0]);
}
#define bind(a,b,c) bind_wrap(a,(sl)b,c)
#else
#define bind(a,b,c) ulinux_sysc(bind,3,a,b,c)
#endif
#ifdef __i386__
static sl listen(sl a, sl b)
{
	sl args[2];

	args[0] = a;
	args[1] = b;

	return ulinux_sysc(socketcall, 2, ULINUX_SYS_LISTEN, (sl)&args[0]);
}
#else
#define listen(a,b) ulinux_sysc(listen,2,a,b)
#endif
#ifdef __i386__
static sl accept_wrap(sl a, sl b, sl c)
{
	sl args[3];

	args[0] = a;
	args[1] = b;
	args[2] = c;

	return ulinux_sysc(socketcall, 2, ULINUX_SYS_ACCEPT, (sl)&args[0]);
}
#define accept(a,b,c) accept_wrap(a,(sl)b,(sl)c)
#else
#define accept(a,b,c) ulinux_sysc(accept,3,a,b,c)
#endif
/*----------------------------------------------------------------------------*/
#endif
