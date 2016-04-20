/* this code is protected by the GNU affero GPLv3
   author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
                           <sylware AT legeek dot net> */
/*----------------------------------------------------------------------------*/
/* compiler stuff */
#include <stdarg.h>
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* ulinux stuff */
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>

#include <ulinux/file.h>
#include <ulinux/socket/socket.h>
#include <ulinux/socket/in.h>
#include <ulinux/signal/signal.h>
#include <ulinux/error.h>
#include <ulinux/epoll.h>
#include <ulinux/utils/mem.h>
#include <ulinux/utils/endian.h>
#include <ulinux/mmap.h>
#include <ulinux/time.h>
#include <ulinux/select.h>
#include <ulinux/stat.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#include "ulinux_namespace.h"
#include "exit_codes.h"
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* configuration */
/* 16 bits value for the port (below 1024, must be root, that you must be for
   chroot anyway) */
#define LISTENING_PORT 80
/* 32 bits value for the IPv4, can be INADDR_ANY */
#define LISTENING_IPV4 INADDR_ANY
/* the chroot patch used upon start */
#define CHROOT_PATH "/home/sylvain/wip/bn/chroot"
/* time out for a socket read/write, in seconds. 4 secs is huge */
#define CNX_WAIT_TIMEOUT 4
/* default file */
#define DEFAULT_FILE (u8*)"/index.xhtml"

/* XXX:Rely on user agent content type detection.
   For more content type accuracy, the next step would be a mapping between
   the file paths and the content types.*/
#define RESP_HDR_FMT (u8*)"\
HTTP/1.1 200 \r\n\
content-lenght:%u\r\n\r\n"
/******************************************************************************/

#define SIGBIT(sig) (1<<(sig-1))

/*----------------------------------------------------------------------------*/
/* sockets stuff */
static struct sockaddr_in srv_addr;
static si srv_sock;	/* the main listening socket */
static si cnx_sock;	/* a cnx socket from accept */

static ul cnx_sock_fd_set[FD_SET_ULS_N];
static u8 cnx_sock_fd_set_ul_idx;
static u8 cnx_sock_fd_ul_shift;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* fd facilities */
static si sigs_fd;	/* fd for signals */
static si epfd;		/* epoll fd */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define HTTP_METHOD_GET		0
#define HTTP_METHOD_HEAD	1
static u8 http_method;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* the buffer page */
static u8 *page;
static ul page_bytes_rd_n;	/* keep an eye on how much was read */

static u8 *method_target_space;
static u8 *target_start;
static u8 *target_end;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static si target_file_fd;
static ul target_file_sz;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static sl resp_hdr_sz;	/* the generated response header */
/*----------------------------------------------------------------------------*/


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


static void epoll_srv_sock_setup(void)
{
	struct epoll_event ep_evt;
	sl r;

	memset(&ep_evt, 0, sizeof(ep_evt));
	ep_evt.events = EPOLLIN | EPOLLPRI;
	ep_evt.data.fd = srv_sock;

	r = epoll_ctl(epfd, EPOLL_CTL_ADD, srv_sock, &ep_evt);
	if(ISERR(r))
		exit(SRV_SOCK_SETUP_EPOLL_CTL_ADD_FAILURE);
}

/*---------------------------------------------------------------------------*/
/* http method can request to close the connexion exiting, we are violent: we close the tcp cnx
   though we should send a response in order to be rfc correct */
#define HTTP_CLOSE 1

/* PAGE_SZ is way bigger than 4 bytes */
static u8 http_method_is_get(void)
{
	if (		page[0] == 'G'
		&&	page[1] == 'E'
		&&	page[2] == 'T'
		&&	page[3] == ' ')
		return 1;
	return 0;
}

/* PAGE_SZ is way bigger than 4 bytes */
static u8 http_method_is_head(void)
{
	if (		page[0] == 'H'
		&&	page[1] == 'E'
		&&	page[2] == 'A'
		&&	page[3] == 'D')
		return 1;
	return 0;
}

static u8 http_method_match(void)
{
	if (http_method_is_get())
		http_method = HTTP_METHOD_GET;
	else if (http_method_is_head())
		http_method = HTTP_METHOD_HEAD;
	else
		return HTTP_CLOSE; /* garbage or method not handled */
	return 0;
}

/* if we have an error or we time out, notify for socket closing */
#define CNX_SOCK_RD_WAIT_FAILURE 1
#define fd_set cnx_sock_fd_set
#define ul_idx cnx_sock_fd_set_ul_idx
#define ul_shift cnx_sock_fd_ul_shift
static u8 cnx_sock_rd_wait(void)
{
	struct timeval tv;
	sl r;

	fd_set[ul_idx] = (1UL << ul_shift); /* was zero-ed in global_init */

	tv.sec = CNX_WAIT_TIMEOUT;
	tv.usec = 0;

	loop {	
		r = select(cnx_sock + 1, &cnx_sock_fd_set[0], 0, 0, &tv);
		if (r != -EINTR)
			break;
	}

	if (ISERR(r) || r == 0) /* r != 1 */
		return CNX_SOCK_RD_WAIT_FAILURE;
	return 0;
}
#undef fd_set
#undef byte_offset
#undef byte_shift

#define CNX_SOCK_RD_FAILURE -1 
static sl cnx_sock_rd(void)
{
	sl bytes_rd_n;

	loop {
		bytes_rd_n = read(cnx_sock, page + page_bytes_rd_n,
						PAGE_SZ - page_bytes_rd_n);
		if (bytes_rd_n != -EINTR) {
			if (ISERR(bytes_rd_n))
				bytes_rd_n = CNX_SOCK_RD_FAILURE;
			break;
		}
	}
	return bytes_rd_n;
}

static u8 http_method_rd(void)
{
	u8 *c;

	c = page;

	loop {
		u8 r;
		sl bytes_rd_n;

		r = cnx_sock_rd_wait();
		if (r == CNX_SOCK_RD_WAIT_FAILURE)
			break;

		bytes_rd_n = cnx_sock_rd();
		if (bytes_rd_n == CNX_SOCK_RD_FAILURE || bytes_rd_n == 0)
			break;

		page_bytes_rd_n += bytes_rd_n;

		loop {
			if (bytes_rd_n-- == 0)
				break;

			if (*c == ' ') {
				/* got the space separator:method' 'target */
				method_target_space = c;
				return 0;
			}

			++c;
		}

		if (page_bytes_rd_n == PAGE_SZ) /* no more page room */
			break;
	}
	return HTTP_CLOSE;
}

/* XXX:must be called with room in the page or it will loop with 0 read bytes
   till the other end decides to shutdown the connection  */
#define UNABLE_TO_READ_AT_LEAST_ONE_BYTE 1
static u8 http_rd_at_least_one_byte(void)
{
	loop {
		u8 r;
		sl bytes_rd_n;

		r = cnx_sock_rd_wait();
		if (r == CNX_SOCK_RD_WAIT_FAILURE)
			break;

		bytes_rd_n = cnx_sock_rd();
		if (bytes_rd_n == CNX_SOCK_RD_FAILURE || bytes_rd_n == 0)
			break;

		if (bytes_rd_n != 0) {
			page_bytes_rd_n += bytes_rd_n;
			return 0;
		}
	}
	return UNABLE_TO_READ_AT_LEAST_ONE_BYTE;
}

static u8 http_target_end_locate(void)
{
	u8 *c;

	c = method_target_space + 1;

	loop {
		/* need at least 1 more byte to keep going */
		if (c == (page + page_bytes_rd_n)) {
			u8 r;

			if (page_bytes_rd_n == PAGE_SZ)
				break;/* but no more room in our page */

			/* we have still room in our page */

			r = http_rd_at_least_one_byte();			
			if (r == UNABLE_TO_READ_AT_LEAST_ONE_BYTE)
				break;
		}

		if (*c == ' ') {
			target_end = c;
			return 0;	
		}
		++c;
	}
	return HTTP_CLOSE;
}

static u8 http_target_file_open(void)
{
	sl r;

	loop {
		r = open(target_start, O_RDONLY, 0);
		if (r != -EINTR)
			break;
	}
	if (ISERR(r))
		return HTTP_CLOSE;

	target_file_fd = (si)r;
	return 0;
}

static u8 http_target_file_sz_get(void)
{
	struct stat target_stat;
	sl r;

	memset(&target_stat, 0, sizeof(target_stat));

	r = fstat(target_file_fd, &target_stat);
	if (ISERR(r))
		return HTTP_CLOSE;

	target_file_sz = target_stat.sz;	
	return 0;
}

/* if we have an error or we time out, notify for socket closing */
#define CNX_SOCK_SEND_WAIT_FAILURE 1
#define fd_set cnx_sock_fd_set
#define ul_idx cnx_sock_fd_set_ul_idx
#define ul_shift cnx_sock_fd_ul_shift
static u8 cnx_sock_send_wait(void)
{
	struct timeval tv;
	sl r;

	fd_set[ul_idx] = (1UL << ul_shift); /* was zero-ed in global_init */

	tv.sec = CNX_WAIT_TIMEOUT;
	tv.usec = 0;

	loop {	
		r = select(cnx_sock + 1, 0, &cnx_sock_fd_set[0], 0, &tv);
		if (r != -EINTR)
			break;
	}

	if (ISERR(r) || r == 0) /* r != 1 */
		return CNX_SOCK_SEND_WAIT_FAILURE;
	return 0;
}
#undef fd_set
#undef byte_offset
#undef byte_shift

#define CNX_SOCK_SEND_RESP_HDR_FAILURE -1 
static sl cnx_sock_send_resp_hdr(void)
{
	sl bytes_sent_n;

	loop {
		bytes_sent_n = write(cnx_sock, page, resp_hdr_sz);
		if (bytes_sent_n != -EINTR) {
			if (ISERR(bytes_sent_n))
				bytes_sent_n = CNX_SOCK_SEND_RESP_HDR_FAILURE;
			break;
		}
	}
	return bytes_sent_n;
}

static u8 http_resp_hdr_send(void)
{
	resp_hdr_sz = (sl)snprintf(page, PAGE_SZ, RESP_HDR_FMT, target_file_sz);

	if (resp_hdr_sz == 0)
		return HTTP_CLOSE;

	loop {
		u8 r;
		sl bytes_sent_n;

		r = cnx_sock_send_wait();
		if (r == CNX_SOCK_SEND_WAIT_FAILURE)
			return HTTP_CLOSE;

		bytes_sent_n = cnx_sock_send_resp_hdr();
		if (bytes_sent_n == CNX_SOCK_SEND_RESP_HDR_FAILURE)
			break;

		resp_hdr_sz -= bytes_sent_n;

		if (resp_hdr_sz == 0)
			return 0;	/* resp hrd was sent */
	}
	return HTTP_CLOSE;
}

#define CNX_SOCK_SENDFILE_FAILURE -1 
static sl cnx_sock_sendfile(void)
{
	sl bytes_sent_n;

	loop {
		bytes_sent_n = sendfile(cnx_sock, target_file_fd, 0,
								target_file_sz);
		if (bytes_sent_n != -EINTR) {
			if (ISERR(bytes_sent_n))
				bytes_sent_n = CNX_SOCK_SENDFILE_FAILURE;
			break;
		}
	}
	return bytes_sent_n;
}

static u8 http_sendfile(void)
{
	loop {
		u8 r;
		sl bytes_sent_n;

		if (target_file_sz == 0)
			break;

		r = cnx_sock_send_wait();
		if (r == CNX_SOCK_SEND_WAIT_FAILURE)
			return HTTP_CLOSE;

		bytes_sent_n = cnx_sock_sendfile();
		if (bytes_sent_n == CNX_SOCK_SENDFILE_FAILURE)
			break;

		target_file_sz -= bytes_sent_n;
	}
	return HTTP_CLOSE;
}

/*---------------------------------------------------------------------------*/

static void cnx_handle(void)
{
	u8 r;

	page_bytes_rd_n = 0;

	/* read till we find the first space */
	r = http_method_rd();
	if (r == HTTP_CLOSE)
		goto direct_exit;

	/* is this space defining a reasonable method name? */
	if (method_target_space == (page  + PAGE_SZ)) /* huge method name */
		goto direct_exit;

	r = http_method_match();
	if (r == HTTP_CLOSE)
		goto direct_exit;

	r = http_target_end_locate();
	if (r == HTTP_CLOSE)
		goto direct_exit;

	/* target is exactly "/" or prepare the path */
	if ((target_end - (method_target_space + 1)) == 1
					&& method_target_space[1] == '/') {
		target_start = DEFAULT_FILE;
	} else {
		target_start = method_target_space + 1;
		*target_end = 0;
	}

	r = http_target_file_open();
	if (r == HTTP_CLOSE)
		goto direct_exit;

	r = http_target_file_sz_get();
	if (r == HTTP_CLOSE)
		goto close_target_file;

	r = http_resp_hdr_send();
	if (r == HTTP_CLOSE)
		goto close_target_file;

	if (http_method == HTTP_METHOD_GET)
		(void)http_sendfile();

close_target_file:
	close(target_file_fd);
direct_exit:
	return;
}

static void cnx_sock_close(void)
{
	loop {
		sl r;

  		r = close(cnx_sock);
		if (r != -EINTR) /* ignores errors */
			break;
	}
}

static void cnx_sock_fd_set_params(void)
{
	si ul_bits_n;

	ul_bits_n = 8 * sizeof(ul);

	cnx_sock_fd_set_ul_idx = cnx_sock / ul_bits_n;
	cnx_sock_fd_ul_shift = cnx_sock % ul_bits_n;
}

static void cnxs_consume(void)
{
	loop {
		sl r;
		struct sockaddr_in peer;
		sl peer_len;

		peer_len = sizeof(peer);
		loop {
			r = accept(srv_sock, &peer, &peer_len);
			if (r != -EINTR && r != ECONNABORTED) /* based on man page */
				break;
			/* SIGSTOP will generate a EINTR */
		}
		
		if (r != -EAGAIN && ISERR(r))
			exit(CNXS_CONSUME_ACCEPT_GENERIC_FAILURE);
		if (peer_len != sizeof(peer))
			exit(CNXS_CONSUME_ACCEPT_WRONG_PEER);
		if (r == -EAGAIN)
			break;	/* no more connexion pending */
		
		cnx_sock=(si)r;

		cnx_sock_fd_set_params();

		cnx_handle();
		cnx_sock_close();
	}
}

static void sigs_consume(void)
{
	struct signalfd_siginfo info;
	
	loop {
		sl r;

		loop {
			memset(&info, 0, sizeof(info));
			r = read(sigs_fd, &info, sizeof(info));
			if (r != -EINTR)
				break;
		}
		if (r != -EAGAIN && ((ISERR(r) || (r > 0
							&& r != sizeof(info)))))
			exit(SIGS_CONSUME_SIGINFO_READ_FAILURE);
		if (r == 0 || r == -EAGAIN)
			break;
	
		switch (info.ssi_signo) {
		case SIGTERM:
			exit(0);
			break;
		/* please, do add the ones you like */
		}
	}
}

static void main_loop(void)
{
	loop {
		struct epoll_event evts[2];	/*sigs_fd and srv_sock */
		sl r;
		sl j;
	
		loop {
			memset(evts, 0, sizeof(evts));
			r = epoll_wait(epfd, evts, 2, -1);
			if (r != -EINTR)
				break;
		}
		if (ISERR(r))
			exit(MAIN_LOOP_EPOLL_WAIT_GENERIC_FAILURE);

		j = 0;
		loop {
			if (j == r)
				break;

			if (evts[j].data.fd == sigs_fd) {
			  if(evts[j].events & EPOLLIN)
				sigs_consume();
			  else
				exit(MAIN_LOOP_EPOLL_WAIT_SIGS_FD_EVENT_IS_NOT_EPOLLIN);
			} else if (evts[j].data.fd == srv_sock) {
			  if (evts[j].events & (EPOLLERR | EPOLLHUP | EPOLLPRI))
				exit(MAIN_LOOP_EPOLL_WAIT_SRV_SOCK_UNEXPECTED_EVENT);
			  else if (evts[j].events & EPOLLIN)
				cnxs_consume();
			  else
				exit(MAIN_LOOP_EPOLL_WAIT_SRV_SOCK_UNKNOWN_FAILURE);
			}

			++j;
		}
	}
}

static void srv_sock_create(void)
{
	sl bool_true;
	sl r;

	/* TCP on IPv4... erk! */
	r = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (ISERR(r))
		exit(SRV_SOCK_CREATE_FAILURE);
	srv_sock = (si)r;
	
	bool_true = 1;
	r = setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &bool_true,
							sizeof(bool_true));
	if (ISERR(r))
		exit(SRV_SOCK_SET_SOCK_OPTION_FAILURE);
	
	r = bind(srv_sock, &srv_addr, sizeof(srv_addr));
	if (ISERR(r))
		exit(SRV_SOCK_BIND_FAILURE);
	
	r = listen(srv_sock, 0);
	if (ISERR(r))
		exit(SRV_SOCK_LISTEN_FAILURE);
}

static void epoll_sigs_setup(void)
{
	struct epoll_event ep_evt;
	sl r;

	memset(&ep_evt, 0, sizeof(ep_evt));
	ep_evt.events = EPOLLET | EPOLLIN;
	ep_evt.data.fd = sigs_fd;
	r = epoll_ctl(epfd, EPOLL_CTL_ADD, sigs_fd, &ep_evt);
	if (ISERR(r))
		exit(EPOLL_SIGS_SETUP_EPOLL_ADD_FAILURE);
}

static void sigs_setup(void)
{/* synchronous treatement of signals with signalfd
    cannot change SIGKILL, neither SIGSTOP */
	u64 mask;
	sl r;

	mask = (~0);
	r = rt_sigprocmask(SIG_BLOCK, &mask, 0, sizeof(mask));
	if (ISERR(r))
		exit(SIGS_SETUP_BLOCKING_FAILURE);

	mask = SIGBIT(SIGTERM) | SIGBIT(SIGCHLD);
	sigs_fd = (si)signalfd4(-1, &mask, sizeof(mask), SFD_NONBLOCK);
	if (ISERR(sigs_fd))
		exit(SIGS_SETUP_HANDLERS_FAILURE);
}

static void page_mmap(void)
{
	sl addr;

	addr = mmap(PAGE_SZ, RD | WR, PRIVATE | ANONYMOUS);
	if(addr == 0 || ISERR(addr))
		exit(RCV_PAGE_MMAP_FAILURE);

	page = (u8*)addr;
}

static void setup(void)
{
	sigs_setup();

	epfd =(si)epoll_create1(0);
	if (ISERR(epfd))
		exit(SETUP_EPOLL_CREATE_FAILURE);

	epoll_sigs_setup();
	srv_sock_create();
	epoll_srv_sock_setup();

	page_mmap();
}

static void globals_init(void)
{
	srv_addr.sin_family = AF_INET;

	/* big endian port */
	srv_addr.sin_port = cpu2be16(LISTENING_PORT);
	srv_addr.sin_addr.s_addr = cpu2be32(LISTENING_IPV4);

	srv_sock = -1;	/* our listening socket */
	cnx_sock = -1;	/* a cnx socket from accept */

	sigs_fd = -1;	/* fd for signals */
	epfd = -1;	/* epoll fd */

	memset(&cnx_sock_fd_set[0], 0, sizeof(cnx_sock_fd_set));
}

static void chroot_do(void)
{
	sl r;

	r = chroot(CHROOT_PATH);
	if (ISERR(r))
		exit(CHROOT_FAILURE);

	r = chdir("/");
	if (ISERR(r))
		exit(CHDIR_FAILURE);
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/* XXX:may do the daemonic stuff if _really_ we need it */
void _start(void)
{
	close(0);
	close(1);
	close(2);
	chroot_do();
	globals_init();
	setup();
	main_loop();
}
