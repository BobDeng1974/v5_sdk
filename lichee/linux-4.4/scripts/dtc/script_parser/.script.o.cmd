cmd_scripts/dtc/script_parser/script.o := gcc -Wp,-MD,scripts/dtc/script_parser/.script.o.d -O2 -I/home/wanlong/workspace/v5/out/host/include -I/home/wanlong/workspace/v5/out/host/usr/include  -Wall -Wmissing-prototypes -Wstrict-prototypes   -I/home/wanlong/workspace/v5/out/host/include -I./tools/include -Iscripts/dtc -Iscripts/dtc/libfdt -Iscripts/dtc/script_parser -c -o scripts/dtc/script_parser/script.o scripts/dtc/script_parser/script.c

source_scripts/dtc/script_parser/script.o := scripts/dtc/script_parser/script.c

deps_scripts/dtc/script_parser/script.o := \
  /usr/include/stdc-predef.h \
  /usr/include/assert.h \
  /usr/include/features.h \
  /usr/include/x86_64-linux-gnu/sys/cdefs.h \
  /usr/include/x86_64-linux-gnu/bits/wordsize.h \
  /usr/include/x86_64-linux-gnu/gnu/stubs.h \
  /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
  /usr/include/memory.h \
  /usr/include/string.h \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h \
  /usr/include/xlocale.h \
  /usr/include/x86_64-linux-gnu/bits/string.h \
  /usr/include/x86_64-linux-gnu/bits/string2.h \
  /home/wanlong/workspace/v5/out/host/include/endian.h \
  /home/wanlong/workspace/v5/out/host/include/byteswap.h \
  /usr/include/byteswap.h \
  /usr/include/x86_64-linux-gnu/bits/byteswap.h \
  /usr/include/x86_64-linux-gnu/bits/types.h \
  /usr/include/x86_64-linux-gnu/bits/typesizes.h \
  /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
  /usr/include/endian.h \
  /usr/include/x86_64-linux-gnu/bits/endian.h \
  /usr/include/stdlib.h \
  /usr/include/x86_64-linux-gnu/bits/string3.h \
  /usr/include/x86_64-linux-gnu/bits/waitflags.h \
  /usr/include/x86_64-linux-gnu/bits/waitstatus.h \
  /usr/include/x86_64-linux-gnu/sys/types.h \
  /usr/include/time.h \
  /usr/include/x86_64-linux-gnu/sys/select.h \
  /usr/include/x86_64-linux-gnu/bits/select.h \
  /usr/include/x86_64-linux-gnu/bits/sigset.h \
  /usr/include/x86_64-linux-gnu/bits/time.h \
  /usr/include/x86_64-linux-gnu/bits/select2.h \
  /home/wanlong/workspace/v5/out/host/include/sys/sysmacros.h \
  /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
  /usr/include/alloca.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib.h \
  /usr/include/stdio.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stdarg.h \
  /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
  /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
  /usr/include/x86_64-linux-gnu/bits/stdio.h \
  /usr/include/x86_64-linux-gnu/bits/stdio2.h \
  scripts/dtc/script_parser/common.h \
  scripts/dtc/script_parser/script.h \

scripts/dtc/script_parser/script.o: $(deps_scripts/dtc/script_parser/script.o)

$(deps_scripts/dtc/script_parser/script.o):
