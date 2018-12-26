#
# Copyright (C) 2015-2016 Allwinner
#
# This is free software, licensed under the GNU General Public License v2.
# See /build/LICENSE for more information.

define KernelPackage/sunxi-vin
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=sunxi-vin support
  FILES+=$(LINUX_DIR)/drivers/media/v4l2-core/videobuf2-dma-contig.ko
  FILES+=$(LINUX_DIR)/drivers/media/platform/sunxi-vin/vin_io.ko
  FILES+=$(LINUX_DIR)/drivers/media/platform/sunxi-vin/modules/sensor/lt6911_mipi.ko
  FILES+=$(LINUX_DIR)/drivers/media/platform/sunxi-vin/vin_v4l2.ko
  AUTOLOAD:=$(call AutoLoad,90, videobuf2-dma-contig vin_io vfe_io lt6911_mipi vin_v4l2)
endef

define KernelPackage/sunxi-vin/description
  Kernel modules for sunxi-vin support
endef

$(eval $(call KernelPackage,sunxi-vin))
