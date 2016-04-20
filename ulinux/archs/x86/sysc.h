#ifndef ULINUX_ARCH_SYSC_H
#define ULINUX_ARCH_SYSC_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/

long ulinux_sysc_0(long name);
long ulinux_sysc_1(long name,long a1);
long ulinux_sysc_2(long name,long a1,long a2);
long ulinux_sysc_3(long name,long a1,long a2,long a3);
long ulinux_sysc_4(long name,long a1,long a2,long a3,long a4);
long ulinux_sysc_5(long name,long a1,long a2,long a3,long a4,long a5);
long ulinux_sysc_6(long name,long a1,long a2,long a3,long a4,long a5,long a6);

#define wrap_ulinux_sysc_0(name)  wrap_ulinux_sysc_0(name);
#define wrap_ulinux_sysc_1(name,a1) ulinux_sysc_1(name,(long)(a1))
#define wrap_ulinux_sysc_2(name,a1,a2) ulinux_sysc_2(name,(long)(a1),(long)(a2))
#define wrap_ulinux_sysc_3(name,a1,a2,a3) ulinux_sysc_3(name,(long)(a1),(long)(a2),(long)(a3))
#define wrap_ulinux_sysc_4(name,a1,a2,a3,a4) ulinux_sysc_4(name,(long)(a1),(long)(a2),(long)(a3),(long)(a4))
#define wrap_ulinux_sysc_5(name,a1,a2,a3,a4,a5) ulinux_sysc_5(name,(long)(a1),(long)(a2),(long)(a3),(long)(a4),(long)(a5))
#define wrap_ulinux_sysc_6(name,a1,a2,a3,a4,a5,a6) ulinux_sysc_6(name,(long)(a1),(long)(a2),(long)(a3),(long)(a4),(long)(a5),(long)(a6))

#define ulinux_sysc(name,nr,...)					\
	wrap_ulinux_sysc_##nr(__ULINUX_NR_##name,##__VA_ARGS__)

#if 0
#define ulinux_sysc(name,nr,args...)					\
({									\
	unsigned long resultvar;					\
	SYSC_LOAD_ARGS_##nr(args)					\
	SYSC_LOAD_REGS_##nr						\
	__asm__ __volatile__ (						\
		"movl %1,%%eax\n\tint $0x80\n\t"		\
		:"=a"(resultvar)					\
		:"i"(__ULINUX_NR_##name)SYSC_ASM_ARGS_##nr		\
		:"memory","cc"						\
	);								\
	(long)resultvar;						\
})


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_0()
#define SYSC_LOAD_REGS_0
#define SYSC_ASM_ARGS_0
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*in pic (position independent code), ebx must be saved*/
#ifdef __pic__
#define SYSC_LOAD_ARGS_1(a1)                                                   \
  long __arg1=(long)(a1);                                                      \
  SYSC_LOAD_ARGS_0()

#define SYSC_LOAD_REGS_1 						\
({									\
	__asm__ __volatile__ (						\
		"movl %0,%%ebx\n\t"				\
		:							\
		:"r"(__arg1)						\
		:"cc"							\
	);								\
	SYSC_LOAD_REGS_0						\
});
#define SYSC_ASM_ARGS_1	SYSC_ASM_ARGS_0,"r"(__arg1)
#else/*__pic__*/
#define SYSC_LOAD_ARGS_1(a1)                                                   \
  long __arg1=(long)(a1);                                                      \
  SYSC_LOAD_ARGS_0()

#define SYSC_LOAD_REGS_1                                                       \
  register long _a1 ("ebx")=__arg1;                                         \
  SYSC_LOAD_REGS_0

#define SYSC_ASM_ARGS_1	SYSC_ASM_ARGS_0,"r"(_a1)
#endif
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_2(a1,a2)                                                \
  long __arg2=(long)(a2);                                                      \
  SYSC_LOAD_ARGS_1(a1)

#define SYSC_LOAD_REGS_2                                                       \
  register long _a2 asm("ecx")=__arg2;                                         \
  SYSC_LOAD_REGS_1

#define SYSC_ASM_ARGS_2	SYSC_ASM_ARGS_1,"r"(_a2)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_3(a1,a2,a3)                                             \
  long __arg3=(long)(a3);                                                      \
  SYSC_LOAD_ARGS_2(a1,a2)

#define SYSC_LOAD_REGS_3                                                       \
  register long _a3 asm("edx")=__arg3;                                         \
  SYSC_LOAD_REGS_2

#define SYSC_ASM_ARGS_3	SYSC_ASM_ARGS_2,"r"(_a3)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_4(a1,a2,a3,a4)                                          \
  long __arg4=(long)(a4);                                                      \
  SYSC_LOAD_ARGS_3(a1,a2,a3)

#define SYSC_LOAD_REGS_4                                                       \
  register long _a4 asm("esi")=__arg4;                                         \
  SYSC_LOAD_REGS_3

#define SYSC_ASM_ARGS_4	SYSC_ASM_ARGS_3,"r"(_a4)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#define SYSC_LOAD_ARGS_5(a1,a2,a3,a4,a5)                                       \
  long __arg5=(long)(a5);                                                      \
  SYSC_LOAD_ARGS_4(a1,a2,a3,a4)

#define SYSC_LOAD_REGS_5                                                       \
  register long _a5 asm("edi")=__arg5;                                          \
  SYSC_LOAD_REGS_4

#define SYSC_ASM_ARGS_5	SYSC_ASM_ARGS_4,"r"(_a5)
/*----------------------------------------------------------------------------*/
#endif


/*============================================================================*/
#define __ULINUX_NR_restart_syscall 0
#define __ULINUX_NR_exit 1
#define __ULINUX_NR_fork 2
#define __ULINUX_NR_read 3
#define __ULINUX_NR_write 4
#define __ULINUX_NR_open 5
#define __ULINUX_NR_close 6
#define __ULINUX_NR_waitpid 7
#define __ULINUX_NR_creat 8
#define __ULINUX_NR_link 9
#define __ULINUX_NR_unlink 10
#define __ULINUX_NR_execve 11
#define __ULINUX_NR_chdir 12
#define __ULINUX_NR_time 13
#define __ULINUX_NR_mknod 14
#define __ULINUX_NR_chmod 15
#define __ULINUX_NR_lchown 16
#define __ULINUX_NR_break 17
#define __ULINUX_NR_oldstat 18
#define __ULINUX_NR_lseek 19
#define __ULINUX_NR_getpid 20
#define __ULINUX_NR_mount 21
#define __ULINUX_NR_umount 22
#define __ULINUX_NR_setuid 23
#define __ULINUX_NR_getuid 24
#define __ULINUX_NR_stime 25
#define __ULINUX_NR_ptrace 26
#define __ULINUX_NR_alarm 27
#define __ULINUX_NR_oldfstat 28
#define __ULINUX_NR_pause 29
#define __ULINUX_NR_utime 30
#define __ULINUX_NR_stty 31
#define __ULINUX_NR_gtty 32
#define __ULINUX_NR_access 33
#define __ULINUX_NR_nice 34
#define __ULINUX_NR_ftime 35
#define __ULINUX_NR_sync 36
#define __ULINUX_NR_kill 37
#define __ULINUX_NR_rename 38
#define __ULINUX_NR_mkdir 39
#define __ULINUX_NR_rmdir 40
#define __ULINUX_NR_dup 41
#define __ULINUX_NR_pipe 42
#define __ULINUX_NR_times 43
#define __ULINUX_NR_prof 44
#define __ULINUX_NR_brk 45
#define __ULINUX_NR_setgid 46
#define __ULINUX_NR_getgid 47
#define __ULINUX_NR_signal 48
#define __ULINUX_NR_geteuid 49
#define __ULINUX_NR_getegid 50
#define __ULINUX_NR_acct 51
#define __ULINUX_NR_umount2 52
#define __ULINUX_NR_lock 53
#define __ULINUX_NR_ioctl 54
#define __ULINUX_NR_fcntl 55
#define __ULINUX_NR_mpx 56
#define __ULINUX_NR_setpgid 57
#define __ULINUX_NR_ulimit 58
#define __ULINUX_NR_oldolduname 59
#define __ULINUX_NR_umask 60
#define __ULINUX_NR_chroot 61
#define __ULINUX_NR_ustat 62
#define __ULINUX_NR_dup2 63
#define __ULINUX_NR_getppid 64
#define __ULINUX_NR_getpgrp 65
#define __ULINUX_NR_setsid 66
#define __ULINUX_NR_sigaction 67
#define __ULINUX_NR_sgetmask 68
#define __ULINUX_NR_ssetmask 69
#define __ULINUX_NR_setreuid 70
#define __ULINUX_NR_setregid 71
#define __ULINUX_NR_sigsuspend 72
#define __ULINUX_NR_sigpending 73
#define __ULINUX_NR_sethostname 74
#define __ULINUX_NR_setrlimit 75
#define __ULINUX_NR_getrlimit 76
#define __ULINUX_NR_getrusage 77
#define __ULINUX_NR_gettimeofday 78
#define __ULINUX_NR_settimeofday 79
#define __ULINUX_NR_getgroups 80
#define __ULINUX_NR_setgroups 81
#define __ULINUX_NR_select 82
#define __ULINUX_NR_symlink 83
#define __ULINUX_NR_oldlstat 84
#define __ULINUX_NR_readlink 85
#define __ULINUX_NR_uselib 86
#define __ULINUX_NR_swapon 87
#define __ULINUX_NR_reboot 88
#define __ULINUX_NR_readdir 89
#define __ULINUX_NR_mmap 90
#define __ULINUX_NR_munmap 91
#define __ULINUX_NR_truncate 92
#define __ULINUX_NR_ftruncate 93
#define __ULINUX_NR_fchmod 94
#define __ULINUX_NR_fchown 95
#define __ULINUX_NR_getpriority 96
#define __ULINUX_NR_setpriority 97
#define __ULINUX_NR_profil 98
#define __ULINUX_NR_statfs 99
#define __ULINUX_NR_fstatfs 100
#define __ULINUX_NR_ioperm 101
#define __ULINUX_NR_socketcall 102
#define __ULINUX_NR_syslog 103
#define __ULINUX_NR_setitimer 104
#define __ULINUX_NR_getitimer 105
#define __ULINUX_NR_stat 106
#define __ULINUX_NR_lstat 107
#define __ULINUX_NR_fstat 108
#define __ULINUX_NR_olduname 109
#define __ULINUX_NR_iopl 110
#define __ULINUX_NR_vhangup 111
#define __ULINUX_NR_idle 112
#define __ULINUX_NR_vm86old 113
#define __ULINUX_NR_wait4 114
#define __ULINUX_NR_swapoff 115
#define __ULINUX_NR_sysinfo 116
#define __ULINUX_NR_ipc 117
#define __ULINUX_NR_fsync 118
#define __ULINUX_NR_sigreturn 119
#define __ULINUX_NR_clone 120
#define __ULINUX_NR_setdomainname 121
#define __ULINUX_NR_uname 122
#define __ULINUX_NR_modify_ldt 123
#define __ULINUX_NR_adjtimex 124
#define __ULINUX_NR_mprotect 125
#define __ULINUX_NR_sigprocmask 126
#define __ULINUX_NR_create_module 127
#define __ULINUX_NR_init_module 128
#define __ULINUX_NR_delete_module 129
#define __ULINUX_NR_get_kernel_syms 130
#define __ULINUX_NR_quotactl 131
#define __ULINUX_NR_getpgid 132
#define __ULINUX_NR_fchdir 133
#define __ULINUX_NR_bdflush 134
#define __ULINUX_NR_sysfs 135
#define __ULINUX_NR_personality 136
#define __ULINUX_NR_afs_syscall 137
#define __ULINUX_NR_setfsuid 138
#define __ULINUX_NR_setfsgid 139
#define __ULINUX_NR__llseek 140
#define __ULINUX_NR_getdents 141
#define __ULINUX_NR__newselect 142
#define __ULINUX_NR_flock 143
#define __ULINUX_NR_msync 144
#define __ULINUX_NR_readv 145
#define __ULINUX_NR_writev 146
#define __ULINUX_NR_getsid 147
#define __ULINUX_NR_fdatasync 148
#define __ULINUX_NR__sysctl 149
#define __ULINUX_NR_mlock 150
#define __ULINUX_NR_munlock 151
#define __ULINUX_NR_mlockall 152
#define __ULINUX_NR_munlockall 153
#define __ULINUX_NR_sched_setparam 154
#define __ULINUX_NR_sched_getparam 155
#define __ULINUX_NR_sched_setscheduler 156
#define __ULINUX_NR_sched_getscheduler 157
#define __ULINUX_NR_sched_yield 158
#define __ULINUX_NR_sched_get_priority_max 159
#define __ULINUX_NR_sched_get_priority_min 160
#define __ULINUX_NR_sched_rr_get_interval 161
#define __ULINUX_NR_nanosleep 162
#define __ULINUX_NR_mremap 163
#define __ULINUX_NR_setresuid 164
#define __ULINUX_NR_getresuid 165
#define __ULINUX_NR_vm86 166
#define __ULINUX_NR_query_module 167
#define __ULINUX_NR_poll 168
#define __ULINUX_NR_nfsservctl 169
#define __ULINUX_NR_setresgid 170
#define __ULINUX_NR_getresgid 171
#define __ULINUX_NR_prctl 172
#define __ULINUX_NR_rt_sigreturn 173
#define __ULINUX_NR_rt_sigaction 174
#define __ULINUX_NR_rt_sigprocmask 175
#define __ULINUX_NR_rt_sigpending 176
#define __ULINUX_NR_rt_sigtimedwait 177
#define __ULINUX_NR_rt_sigqueueinfo 178
#define __ULINUX_NR_rt_sigsuspend 179
#define __ULINUX_NR_pread64 180
#define __ULINUX_NR_pwrite64 181
#define __ULINUX_NR_chown 182
#define __ULINUX_NR_getcwd 183
#define __ULINUX_NR_capget 184
#define __ULINUX_NR_capset 185
#define __ULINUX_NR_sigaltstack 186
#define __ULINUX_NR_sendfile 187
#define __ULINUX_NR_getpmsg 188
#define __ULINUX_NR_putpmsg 189
#define __ULINUX_NR_vfork 190
#define __ULINUX_NR_ugetrlimit 191
#define __ULINUX_NR_mmap2 192
#define __ULINUX_NR_truncate64 193
#define __ULINUX_NR_ftruncate64 194
#define __ULINUX_NR_stat64 195
#define __ULINUX_NR_lstat64 196
#define __ULINUX_NR_fstat64 197
#define __ULINUX_NR_lchown32 198
#define __ULINUX_NR_getuid32 199
#define __ULINUX_NR_getgid32 200
#define __ULINUX_NR_geteuid32 201
#define __ULINUX_NR_getegid32 202
#define __ULINUX_NR_setreuid32 203
#define __ULINUX_NR_setregid32 204
#define __ULINUX_NR_getgroups32 205
#define __ULINUX_NR_setgroups32 206
#define __ULINUX_NR_fchown32 207
#define __ULINUX_NR_setresuid32 208
#define __ULINUX_NR_getresuid32 209
#define __ULINUX_NR_setresgid32 210
#define __ULINUX_NR_getresgid32 211
#define __ULINUX_NR_chown32 212
#define __ULINUX_NR_setuid32 213
#define __ULINUX_NR_setgid32 214
#define __ULINUX_NR_setfsuid32 215
#define __ULINUX_NR_setfsgid32 216
#define __ULINUX_NR_pivot_root 217
#define __ULINUX_NR_mincore 218
#define __ULINUX_NR_madvise 219
#define __ULINUX_NR_getdents64 220
#define __ULINUX_NR_fcntl64 221
#define __ULINUX_NR_gettid 224
#define __ULINUX_NR_readahead 225
#define __ULINUX_NR_setxattr 226
#define __ULINUX_NR_lsetxattr 227
#define __ULINUX_NR_fsetxattr 228
#define __ULINUX_NR_getxattr 229
#define __ULINUX_NR_lgetxattr 230
#define __ULINUX_NR_fgetxattr 231
#define __ULINUX_NR_listxattr 232
#define __ULINUX_NR_llistxattr 233
#define __ULINUX_NR_flistxattr 234
#define __ULINUX_NR_removexattr 235
#define __ULINUX_NR_lremovexattr 236
#define __ULINUX_NR_fremovexattr 237
#define __ULINUX_NR_tkill 238
#define __ULINUX_NR_sendfile64 239
#define __ULINUX_NR_futex 240
#define __ULINUX_NR_sched_setaffinity 241
#define __ULINUX_NR_sched_getaffinity 242
#define __ULINUX_NR_set_thread_area 243
#define __ULINUX_NR_get_thread_area 244
#define __ULINUX_NR_io_setup 245
#define __ULINUX_NR_io_destroy 246
#define __ULINUX_NR_io_getevents 247
#define __ULINUX_NR_io_submit 248
#define __ULINUX_NR_io_cancel 249
#define __ULINUX_NR_fadvise64 250
#define __ULINUX_NR_exit_group 252
#define __ULINUX_NR_lookup_dcookie 253
#define __ULINUX_NR_epoll_create 254
#define __ULINUX_NR_epoll_ctl 255
#define __ULINUX_NR_epoll_wait 256
#define __ULINUX_NR_remap_file_pages 257
#define __ULINUX_NR_set_tid_address 258
#define __ULINUX_NR_timer_create 259
#define __ULINUX_NR_timer_settime 260
#define __ULINUX_NR_timer_gettime 261
#define __ULINUX_NR_timer_getoverrun 262
#define __ULINUX_NR_timer_delete 263
#define __ULINUX_NR_clock_settime 264
#define __ULINUX_NR_clock_gettime 265
#define __ULINUX_NR_clock_getres 266
#define __ULINUX_NR_clock_nanosleep 267
#define __ULINUX_NR_statfs64 268
#define __ULINUX_NR_fstatfs64 269
#define __ULINUX_NR_tgkill 270
#define __ULINUX_NR_utimes 271
#define __ULINUX_NR_fadvise64_64 272
#define __ULINUX_NR_vserver 273
#define __ULINUX_NR_mbind 274
#define __ULINUX_NR_get_mempolicy 275
#define __ULINUX_NR_set_mempolicy 276
#define __ULINUX_NR_mq_open 277
#define __ULINUX_NR_mq_unlink 278
#define __ULINUX_NR_mq_timedsend 279
#define __ULINUX_NR_mq_timedreceive 280
#define __ULINUX_NR_mq_notify 281
#define __ULINUX_NR_mq_getsetattr 282
#define __ULINUX_NR_kexec_load 283
#define __ULINUX_NR_waitid 284
#define __ULINUX_NR_add_key 286
#define __ULINUX_NR_request_key 287
#define __ULINUX_NR_keyctl 288
#define __ULINUX_NR_ioprio_set 289
#define __ULINUX_NR_ioprio_get 290
#define __ULINUX_NR_inotify_init 291
#define __ULINUX_NR_inotify_add_watch 292
#define __ULINUX_NR_inotify_rm_watch 293
#define __ULINUX_NR_migrate_pages 294
#define __ULINUX_NR_openat 295
#define __ULINUX_NR_mkdirat 296
#define __ULINUX_NR_mknodat 297
#define __ULINUX_NR_fchownat 298
#define __ULINUX_NR_futimesat 299
#define __ULINUX_NR_fstatat64 300
#define __ULINUX_NR_unlinkat 301
#define __ULINUX_NR_renameat 302
#define __ULINUX_NR_linkat 303
#define __ULINUX_NR_symlinkat 304
#define __ULINUX_NR_readlinkat 305
#define __ULINUX_NR_fchmodat 306
#define __ULINUX_NR_faccessat 307
#define __ULINUX_NR_pselect6 308
#define __ULINUX_NR_ppoll 309
#define __ULINUX_NR_unshare 310
#define __ULINUX_NR_set_robust_list 311
#define __ULINUX_NR_get_robust_list 312
#define __ULINUX_NR_splice 313
#define __ULINUX_NR_sync_file_range 314
#define __ULINUX_NR_tee 315
#define __ULINUX_NR_vmsplice 316
#define __ULINUX_NR_move_pages 317
#define __ULINUX_NR_getcpu 318
#define __ULINUX_NR_epoll_pwait 319
#define __ULINUX_NR_utimensat 320
#define __ULINUX_NR_signalfd 321
#define __ULINUX_NR_timerfd_create 322
#define __ULINUX_NR_eventfd 323
#define __ULINUX_NR_fallocate 324
#define __ULINUX_NR_timerfd_settime 325
#define __ULINUX_NR_timerfd_gettime 326
#define __ULINUX_NR_signalfd4 327
#define __ULINUX_NR_eventfd2 328
#define __ULINUX_NR_epoll_create1 329
#define __ULINUX_NR_dup3 330
#define __ULINUX_NR_pipe2 331
#define __ULINUX_NR_inotify_init1 332
#define __ULINUX_NR_preadv 333
#define __ULINUX_NR_pwritev 334
#define __ULINUX_NR_rt_tgsigqueueinfo 335
#define __ULINUX_NR_perf_event_open 336
#define __ULINUX_NR_recvmmsg 337
#define __ULINUX_NR_fanotify_init 338
#define __ULINUX_NR_fanotify_mark 339
#define __ULINUX_NR_prlimit64 340
#define __ULINUX_NR_name_to_handle_at 341
#define __ULINUX_NR_open_by_handle_at 342
#define __ULINUX_NR_clock_adjtime 343
#define __ULINUX_NR_syncfs 344
#define __ULINUX_NR_sendmmsg 345
#define __ULINUX_NR_setns 346
#define __ULINUX_NR_process_vm_readv 347
#define __ULINUX_NR_process_vm_writev 348
#define __ULINUX_NR_kcmp 349
#define __ULINUX_NR_finit_module 350
#define __ULINUX_NR_sched_setattr 351
#define __ULINUX_NR_sched_getattr 352
#define __ULINUX_NR_renameat2 353
#define __ULINUX_NR_seccomp 354
#define __ULINUX_NR_getrandom 355
#define __ULINUX_NR_memfd_create 356
#define __ULINUX_NR_bpf 357
#define __ULINUX_NR_execveat 358
    
#endif
