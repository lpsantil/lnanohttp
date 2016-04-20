	.text
	.global ulinux_sysc_0
	.type	ulinux_sysc_0,@function
ulinux_sysc_0:
	mov 4(%esp),%eax
	int $0x80
	ret

	.text
	.global ulinux_sysc_1
	.type	ulinux_sysc_1,@function
ulinux_sysc_1:
	push %ebx
	mov 8(%esp),%eax
	mov 12(%esp),%ebx
	int $0x80
	pop %ebx
	ret

	.text
	.global ulinux_sysc_2
	.type	ulinux_sysc_2,@function
ulinux_sysc_2:
	push %ebx
	push %ecx
	mov 12(%esp),%eax
	mov 16(%esp),%ebx
	mov 20(%esp),%ecx
	int $0x80
	pop %ecx
	pop %ebx
	ret

	.text
	.global ulinux_sysc_3
	.type	ulinux_sysc_3,@function
ulinux_sysc_3:
	push %ebx
	push %ecx
	push %edx
	mov 16(%esp),%eax
	mov 20(%esp),%ebx
	mov 24(%esp),%ecx
	mov 28(%esp),%edx
	int $0x80
	pop %edx
	pop %ecx
	pop %ebx
	ret

	.text
	.global ulinux_sysc_4
	.type	ulinux_sysc_4,@function
ulinux_sysc_4:
	push %ebx
	push %ecx
	push %edx
	push %esi
	mov 20(%esp),%eax
	mov 24(%esp),%ebx
	mov 28(%esp),%ecx
	mov 32(%esp),%edx
	mov 36(%esp),%esi
	int $0x80
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	ret

	.text
	.global ulinux_sysc_5
	.type	ulinux_sysc_5,@function
ulinux_sysc_5:
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	mov 24(%esp),%eax
	mov 28(%esp),%ebx
	mov 32(%esp),%ecx
	mov 36(%esp),%edx
	mov 40(%esp),%esi
	mov 44(%esp),%edi
	int $0x80
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	ret

	.text
	.global ulinux_sysc_6
	.type	ulinux_sysc_6,@function
ulinux_sysc_6:
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp
	mov 28(%esp),%eax
	mov 32(%esp),%ebx
	mov 36(%esp),%ecx
	mov 40(%esp),%edx
	mov 44(%esp),%esi
	mov 48(%esp),%edi
	mov 52(%esp),%ebp
	int $0x80
	pop %ebp
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	ret
