#!/bin/sh

# stolen from ffmpeg configure like a pig
# this is brutal and stupid... then easy and explicit

# Prevent locale nonsense from breaking basic text processing.
LC_ALL=C
export LC_ALL

#-------------------------------------------------------------------------------
#uppercase S tells gcc assembler to run the pre-preprocessor
#lowercase s tells gcc assembler _NOT_ to run the pre-preprocessor
#
ulinux_target_arch_asm_srcs='
    ulinux/arch/sysc.S
    ulinux/arch/utils/endian.S
'

ulinux_c_srcs='
    ulinux/utils/mem.c
    ulinux/utils/ascii/string/vsprintf.c
'
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
lnanohttp_bin_ulinux_objs='
    sysc.o
    endian.o
    mem.o
    vsprintf.o
'
lnanohttp_bin_srcs='
    lnanohttp.c
'
#-------------------------------------------------------------------------------


clean_do(){
    files="
        $lnanohttp_bin_ulinux_objs
    "

    for src_file in $lnanohttp_bin_srcs
    do
        obj=${src_file%.c}
        obj=${obj}.o
        files="$obj $files"
    done

    rm -f $files
    rm -f ulinux/arch
    rmdir ulinux
    exit 0
}

################################################################################

# find source path
if test -f make; then
    src_path=.
else
    src_path=$(cd $(dirname "$0"); pwd)
    echo "$src_path" | grep -q '[[:blank:]]' &&
        die "Out of tree builds are impossible with whitespace in source path."
    test -e "$src_path/config.h" &&
        die "Out of tree builds are impossible with config.h in source dir."
fi

is_in(){
    value=$1
    shift
    for var in $*; do
        [ $var = $value ] && return 0
    done
    return 1
}

append(){
    var=$1
    shift
    eval "$var=\"\$$var $*\""
}

die(){
    echo "$0"
    exit 1
}

die_unknown(){
    echo "Unknown option \"$1\"."
    echo "See $0 --help for available options."
    exit 1
}

set_default(){
    for opt; do
        eval : \${$opt:=\$${opt}_default}
    done
}

CMDLINE_SET='
    arch
    asm
    cc
    bin_ccld_tmpl
'

#command line, set defaults
arch_default=$(uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)
asm_default='gcc -c'
cc_default='gcc -Wall -Wextra -Wno-missing-field-initializers -c -fpic -O0'
bin_ccld_tmpl_default="ld -Bstatic -nostdlib \$objects $(gcc -print-libgcc-file-name)"

#(tinycc + binutils) toolchain
##asm_default='as'
##cc_default='/home/sylvain/local/tinycc/bin/tcc -c' 
##bin_ccld_tmpl_default="ld \$objects"

set_default $CMDLINE_SET

show_help(){
    cat <<EOF
Usage: make [options] [operations]

Operations: [default is to build the http server binary]:
  clean                    clean build products


Options: [defaults in brackets after descriptions]

Help options:
  --help                   print this message

Advanced options (experts only):
  --arch=ARCH                   use ulinux target ARCH [$arch_default]
  --asm=ASM                     use target arch assembler command line ASM [$asm_default]
  --cc=CC                       use target arch C compiler command line CC [$cc_default]
  --bin-ccld-tmpl=BIN_CCLD_TMPL use linker command line template BIN_CCLD_TMPL for target arch binary [$bin_ccld_tmpl_default]
EOF
  exit 0
}

for opt do
    optval="${opt#*=}"
    case "$opt" in
        clean) clean_do
        ;;
        --help|-h) show_help
        ;;
        *)
            optname="${opt%%=*}"
            optname="${optname#--}"
            optname=$(echo "$optname" | sed 's/-/_/g')
            if is_in $optname $CMDLINE_SET; then
                eval $optname='$optval'
            elif is_in $optname $CMDLINE_APPEND; then
                append $optname "$optval"
            else
                die_unknown $opt
            fi
        ;;
    esac
done


#-------------------------------------------------------------------------------
#configure our ultra-thin linux user API abstraction layer
rm -f ulinux/arch
mkdir -p ulinux
ln -f -s $src_path/ulinux/archs/$arch ulinux/arch
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#assemble src files
target_arch_asm_srcs="
	$ulinux_target_arch_asm_srcs
"

for src_file in $target_arch_asm_srcs
do
    obj=${src_file%.S}
    obj=${obj}.o
    echo ASM $src_file
    $asm -o $(basename $obj) $src_file
done
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#C compile src files
c_srcs="
	$ulinux_c_srcs
	$lnanohttp_bin_srcs
"
for src_file in $c_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    echo CC $src_file
    $cc -o $(basename $obj) -I./ -I$src_path $src_path/$src_file
    c_objs="$obj $c_objs"
done
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#link the lnanohttp binary
for src_file in $lnanohttp_bin_srcs
do
    obj=${src_file%.c}
    obj=${obj}.o
    lnanohttp_bin_objs="$obj $lnanohttp_bin_objs"
done
lnanohttp_bin_objs="$lnanohttp_bin_objs $lnanohttp_bin_ulinux_objs"

echo BIN_CCLD lnanohttp
#resolve the template
objects=$lnanohttp_bin_objs
eval bin_ccld=\"$bin_ccld_tmpl\"
$bin_ccld  -o lnanohttp 
#-------------------------------------------------------------------------------
