cmd_/home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/uapi/linux/sunrpc debug.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/linux/sunrpc ; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/generated/uapi/linux/sunrpc ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc/$$F; done; touch /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/linux/sunrpc/.install
