/* this code is protected by the GNU affero GPLv3
   author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com> */
	.text
	.globl _start
	.type _start,@function
_start:
	xor %ebp, %ebp/*suggested by abi*/
	pop %eax/*argc is 1st argument of start*/
	movl %esp,%ebx/*argv/envp pointers is 2nd argument of start*/
	and  $~31, %esp/*32 bytes aligned stack*/
	push %ebx
	push %eax
	push %eax/*fake return address*/
	jmp ulinux_start
