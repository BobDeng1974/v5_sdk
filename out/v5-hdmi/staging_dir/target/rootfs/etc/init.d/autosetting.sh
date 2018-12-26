#!/bin/sh


sleep 15
echo 1 > /proc/sys/net/ipv4/tcp_low_latency
ifconfig eth0 down
[ -n "$(dmesg|grep 'HDMI timing check')" -a -z "$(dmesg|grep 'HDMI not synced')" ] && {
    #server
    ifconfig eth0 hw ether F4:4E:FD:AE:CD:10
    ifconfig eth0 up
    ifconfig eth0 192.168.0.101 netmask 255.255.255.0
    /bin/sh /etc/init.d/resetIP.sh 192.168.0.101 > /tmp/shell.log 2>&1 &
} || {
    #client
    ifconfig eth0 hw ether F4:4E:FD:AE:CD:20
    ifconfig eth0 up
    ifconfig eth0 192.168.0.102 netmask 255.255.255.0
    /bin/sh /etc/init.d/resetIP.sh 192.168.0.102 > /tmp/shell.log 2>&1 &
}

