#!/bin/bash


#wget -c https://raw.github.com/RobertCNelson/tools/master/pkgs/dtc.sh
#chmod u+x dtc.sh

#cp DM-GPIO-Test-00A0.dtbo /lib/firmware
	
export SLOTS=/sys/devices/bone_capemgr.9/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins

cat $SLOTS
cat $PINS
echo arm_control-1 > $SLOTS

