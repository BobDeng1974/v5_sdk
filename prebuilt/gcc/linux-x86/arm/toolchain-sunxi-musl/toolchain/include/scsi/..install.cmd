cmd_/home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/uapi/scsi scsi_bsg_fc.h scsi_netlink.h scsi_netlink_fc.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/scsi ; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/generated/uapi/scsi ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/$$F; done; touch /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/.install
