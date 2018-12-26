#
# Copyright (C) 2016 OpenWrt.org
#
include sysinfo.sh

platform_do_upgrade() {
	local tar_file="$1"
	local board="$(get_target_board)"
    local boot_type=$(get_boot_type)

    case "$boot_type" in
        nand|emmc|sdcard)
            echo "$boot_type flashing boot"
            tar xzf $tar_file ${board}-sysupgrade/boot -O | dd of=/dev/by-name/boot bs=4k 2>/dev/null

            echo "$boot_type flashing rootfs"
            tar xzf $tar_file ${board}-sysupgrade/rootfs -O | dd of=/dev/by-name/rootfs bs=4k 2>/dev/null
            ;;
        nor)
            echo "$boot_type flashing boot"
            tar xzf $tar_file ${board}-sysupgrade/boot -O | mtd write - boot

            echo "$boot_type flashing rootfs"
            tar xzf $tar_file ${board}-sysupgrade/rootfs -O | mtd write - rootfs
            ;;
        *)
            echo "target flash not support"
            return 1
            ;;
    esac

	return 0
}

platform_check_image() {
	local tar_file="$1"
    local board=$(get_target_board)

	case "$board" in
	azalea-*)
        local kernel_length=`(tar xzf $tar_file ${board}-sysupgrade/boot -O | wc -c) 2> /dev/null`
		local rootfs_length=`(tar xzf $tar_file ${board}-sysupgrade/rootfs -O | wc -c) 2> /dev/null`
		;;

	*)
		echo "Sysupgrade is not supported on your board yet."
		return 1
		;;
	esac

	[ "$kernel_length" = 0 -o "$rootfs_length" = 0 ] && {
		echo "The upgarde image is corrupt."
		return 1
	}

	return 0
}
