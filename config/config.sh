#!/bin/bash

if [ ! -e /lib/firmware/arm_control-1-00A0.dtbo ] ; then
    sudo cp arm_control-1-00A0.dtbo /lib/firmware
fi	

#wget -c https://raw.github.com/RobertCNelson/tools/master/pkgs/dtc.sh
#chmod u+x dtc.sh

#cp DM-GPIO-Test-00A0.dtbo /lib/firmware
	
export SLOTS=/sys/devices/bone_capemgr.9/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins

echo "cat $SLOTS"
cat $SLOTS

cat $PINS
echo "slot"
echo arm_control-1 > $SLOTS

