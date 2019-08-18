#!/bin/bash
sleep 10s
echo PRU-COPTER > /sys/devices/bone_capemgr.?/slots  
echo "Device tree overlay applied \n"
sleep 5s
cd ../../var/lib/cloud9/DeviceTree/Copter/
./RunControlCopter
exit 1
