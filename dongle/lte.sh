#!/bin/sh

MODEM=/dev/ttyUSB1
IFACE=wwan0
BKP_ROUTE_DFL=/var/tmp/default-route
BKP_RESOLVER=/var/tmp/resolv.conf
ROUTE_BIN="/sbin/route"

########################################################################################

modem_attach() {

    echo -ne 'AT\r\n' > $MODEM
    sleep 1
    echo -ne 'ATZ\r\n' > $MODEM
    sleep 1
    echo -ne 'AT+CPIN=0000\r\n' > $MODEM
    sleep 2
    echo -ne 'ATQ0 V1 E1 S0=0\r\n' > $MODEM
    sleep 1
}

modem_start() {

    echo -ne 'AT\r\n' > $MODEM
    sleep 1
    echo -ne 'AT^NDISDUP=1,1,"orange.fr","orange","orange"\r\n' > $MODEM
    sleep 15
}

modem_start6() {

    echo -ne 'AT\r\n' > $MODEM
    sleep 1
    echo -ne 'AT^NDISDUP=1,1,"ppv6lte"\r\n' > $MODEM
    sleep 15
}

modem_stop() {

    echo -ne 'AT\r\n' > $MODEM
    sleep 1
    echo -ne 'AT^NDISDUP=1,0\r\n' > $MODEM
    sleep 1
}

modem_stop6() {

    echo -ne 'AT\r\n' > $MODEM
    sleep 1
    echo -ne 'AT^NDISDUP=1,0\r\n' > $MODEM
    sleep 1
}

########################################################################################

dhclient_kill() {
    /bin/kill `ps -eaf|grep dhclient|grep $IFACE|grep -v grep|awk '{print $2}'`
}

########################################################################################

backup_netconf() {

	# /etc/resolv.conf
	/bin/cp /etc/resolv.conf $BKP_RESOLVER

	# default route
	if [ ! -e $BKP_ROUTE_DFL ]; then
		ROUTE_DFL=`/bin/netstat -rn|/usr/bin/awk '/^0.0.0.0/{print "gw",$2,"dev",$8;}'`
		if [ "x$ROUTE_DFL" != "x" ]; then
			echo $ROUTE_DFL > $BKP_ROUTE_DFL
			$ROUTE_BIN del default $ROUTE_DFL
		fi
	fi
}

restore_netconf() {

	# /etc/resolv.conf
	if [ -e $BKP_RESOLVER ]; then
		/bin/cp $BKP_RESOLVER /etc/resolv.conf
		/bin/rm $BKP_RESOLVER
	fi

	# default route
	if [ -e $BKP_ROUTE_DFL ]; then
		$ROUTE_BIN add default `/bin/cat $BKP_ROUTE_DFL`
		/bin/rm -f $BKP_ROUTE_DFL
	fi
}

########################################################################################

echo [ `date` ] LTE $* >> /var/log/lte.log

case "$1" in

  attach)
    modem_attach
  ;;

  service-start)
    modem_attach
    modem_start
    ifconfig $IFACE up
    dhclient $IFACE
  ;;

  service-start6)
    modem_attach
    modem_start6
    ifconfig $IFACE up
  ;;

  service-stop)
    modem_stop
    ifconfig $IFACE down
    dhclient_kill
  ;;

  connect)
    modem_start
    ifconfig $IFACE up
    dhclient $IFACE
  ;;

  connect6)
    modem_start6
###    ifconfig $IFACE up
  ;;

  disconnect)
    modem_stop
    ifconfig $IFACE down
    dhclient_kill
  ;;

  disconnect6)
    modem_stop6
###    ifconfig $IFACE down
  ;;

  backup-netconf)
    backup_netconf
  ;;

  restore-netconf)
    restore_netconf
  ;;

  status)
    ifconfig $IFACE
    netstat -rn
  ;;

  *)
    echo "usage: lte.sh attach|connect|disconnect|backup-netconf|restore-netconf"
    exit 1
  ;;
esac


exit 0
