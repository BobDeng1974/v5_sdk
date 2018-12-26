# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
curdir:=target

$(curdir)/builddirs:=vecoo
$(curdir)/builddirs-default:=vecoo
$(curdir)/builddirs-install:=vecoo

$(curdir)/imagebuilder/install:=$(curdir)/vecoo/install

$(eval $(call stampfile,$(curdir),target,prereq,.config))
$(eval $(call stampfile,$(curdir),target,compile,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),target,install,$(TMP_DIR)/.build))

$($(curdir)/stamp-install): $($(curdir)/stamp-compile) 

$(eval $(call subdir,$(curdir)))
