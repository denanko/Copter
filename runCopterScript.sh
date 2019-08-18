#!/bin/sh -e
### BEGIN INIT INFO
# Provides:          capemgr.sh
# Required-Start:    $local_fs
# Required-Stop:     $local_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start daemon at boot time
# Description:       Enable service provided by daemon.
### END INIT INFO#! 
sleep 5s
echo PRU-COPTER > /sys/devices/bone_capemgr.9/slots
sleep 15s
cd ../../var/lib/cloud9/DeviceTree/Copter/
sudo ./RunControlCopter
exit 0