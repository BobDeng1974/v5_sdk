cmd_/home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi /home/wangyaliang/workspace/tina/lichee/linux-3.10/include/uapi/linux/hsi hsi_char.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi /home/wangyaliang/workspace/tina/lichee/linux-3.10/include/linux/hsi ; bash scripts/headers_install.sh /home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi /home/wangyaliang/workspace/tina/lichee/linux-3.10/include/generated/uapi/linux/hsi ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi/$$F; done; touch /home/wangyaliang/workspace/tina/out/tulip-d1/compile_dir/toolchain/linux-dev//include/linux/hsi/.install
