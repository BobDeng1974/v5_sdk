#!/bin/sh

adbd &
udc=`ls /sys/class/udc`
isudc=0
while [ 1 ]
do
	sleep 1
	otg_role=`cat /sys/devices/platform/soc/usbc0/otg_role`
	if [ $otg_role = "usb_device" ]
	then
		if [ $isudc == 0 ]
		then
			echo $udc > /sys/kernel/config/usb_gadget/g1/UDC
			isudc=1
		fi
	else
		isudc=0
	fi
done
