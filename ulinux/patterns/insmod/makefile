.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

insmod:insmod.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. ../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_mem.c -o mem.o
	cpp -Wall -Wextra -I../../.. ../../utils/ascii/string/vsprintf.c \
		-o cpp_vsprintf.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_vsprintf.c -o vsprintf.o
	as ../../arch/args_env.s -o args_env.o
	cpp -Wall -Wextra -I../../.. insmod.c -o cpp_insmod.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_insmod.c -o insmod.o
	ld -O -nostdlib -Bstatic --strip-all insmod.o args_env.o string.o mem.o \
		vsprintf.o $$(gcc -print-libgcc-file-name) --output insmod

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_insmod.c insmod.o insmod \
	       cpp_mem.c mem.o \
	       cpp_vsprintf.c vsprintf.o \
	       args_env.o
