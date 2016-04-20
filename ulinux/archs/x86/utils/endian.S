#ifndef __GNUC__
	.text
	.global ulinux_cpu2be16
	.type ulinux_cpu2be16,@function
ulinux_cpu2be16:
	mov 4(%esp),%ax
	xchg %ah,%al
	ret

	.text
	.global ulinux_cpu2be32
	.type ulinux_cpu2be32,@function
ulinux_cpu2be32:
	mov 4(%esp),%eax
	bswap %eax
	ret

	.text
	.global ulinux_cpu2be64
	.type ulinux_cpu2be64,@function
ulinux_cpu2be64:
	mov 4(%esp),%eax
	mov 8(%esp),%edx
	bswap %eax
	bswap %edx
	xchg %eax,%edx
	ret
#endif
