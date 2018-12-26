#!/bin/sh

/usr/bin/tcp_server > /dev/null 2>&1 &

while [ 1 ]
do
    [ -z "$(ps|grep tcp_client|grep -v grep)" ] && {
  	/usr/bin/tcp_client > /dev/null 2>&1 &
    }
    sleep 1
done
