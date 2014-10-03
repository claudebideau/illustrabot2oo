#!/bin/bash

CPWD=`pwd`
ARCHIVE="cfg_dongle.tgz"
TMP="/tmp/dongle"

echo ${CPWD}

mkdir -pv /tmp/dongle

if [ -e ${CPWD}/${ARCHIVE} ]; then

	cd ${TMP}
	echo "uncompress archive"
	tar xfz ${CPWD}/${ARCHIVE}


list_file=( 'usr/local/sbin/lte-dongle.sh'\
 'etc/udev/rules.d/40-usb_modeswitch.rules'\
 'etc/usb_modeswitch.d/12d1_156a.bin'\
  'etc/modules-load.d/huawey.conf' )

for file in ${list_file[*]}; do

  	if [ ! -e /${file} ] ; then
		echo " copy file=${file}"
		sudo cp ${TMP}/${file} /${file}
	fi

done
chmod 755 /usr/local/sbin/lte-dongle.sh

echo "# add manually the following line in /etc/network/interfaces"
echo "# ============================================================="
echo "# dongle LTE"
echo "allow-hotplug wwan0"
echo "iface wwan0 inet manual"
echo "    pre-up /usr/local/sbin/lte-dongle.sh backup-netconf"
echo "    post-up /usr/local/sbin/lte-dongle.sh connect"
echo "    pre-down /usr/local/sbin/lte-dongle.sh disconnect"
echo "    post-down /usr/local/sbin/lte-dongle.sh restore-netconf"



fi

