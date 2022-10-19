#!/bin/bash
DEVICE=/dev/sda1
MOUNTPOINT=/mnt

while true
do
	/usr/bin/mount $DEVICE $MOUNTPOINT > /dev/null 2>&1
	if [ $? -eq 0 ] ; then
		echo -n "installing file: "
		ls -la *.uf2 | awk -F " " '{print $9", "$5" Bytes, changed: "$6" "$7", "$8}'
		cp *.uf2 $MOUNTPOINT
		sleep 1
		umount $MOUNTPOINT
		sleep 5
	else
		sleep 1
	fi
done
