cmd_/home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc/.install := bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/uapi/scsi/fc fc_els.h fc_fs.h fc_gs.h fc_ns.h; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/scsi/fc ; bash scripts/headers_install.sh /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc /home/wangyaliang/workspace/vecoo/lichee/linux-3.10/include/generated/uapi/scsi/fc ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc/$$F; done; touch /home/wangyaliang/workspace/vecoo/out/azalea-aiworld/compile_dir/toolchain/linux-dev//include/scsi/fc/.install