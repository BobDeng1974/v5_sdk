cmd_/home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/uapi/mtd inftl-user.h mtd-abi.h mtd-user.h nftl-user.h ubi-user.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/mtd ; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/generated/uapi/mtd ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd/$$F; done; touch /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/mtd/.install
