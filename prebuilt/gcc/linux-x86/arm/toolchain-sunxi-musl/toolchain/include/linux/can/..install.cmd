cmd_/home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/uapi/linux/can bcm.h error.h gw.h netlink.h raw.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/linux/can ; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/generated/uapi/linux/can ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can/$$F; done; touch /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/can/.install