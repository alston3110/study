#!/bin/sh

if [ $(iwconfig 2>/dev/null | grep -o "^\w*" | wc -l ) = 0 ]; then
	echo "PITX_WIFI	  : no device "
	modprobe ath9k 
fi

while [ -d /proc/$1 ]; do
	if [ $(ifconfig | grep "wlan" | wc -l ) = 0 ]; then
		ifconfig $(iwconfig 2>/dev/null | grep -o "^\w*" ) up
		sleep 18
	fi

	echo "$2.WIFI     : ($(iwlist $(ifconfig | grep "wlan" | awk '{print $1}') scan | grep Cell | wc -l))"
#	ifconfig $(iwconfig 2>/dev/null | grep -o "^\w*") down
	if [ $(iwlist $(ifconfig | grep "wlan" | awk '{print $1}') scan | grep Cell | wc -l) > 1 ]; then
		break;
	fi 
	sleep 5 
done

psp=$(ps ax | grep "wpa_supplicant -B" | grep -v grep | awk '{print $1}')
echo "psp=$psp"
kill $psp
sleep 2

wpa_supplicant -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant.conf &
sleep 2

num=$(wpa_cli -iwlan0 -p/var/run/wpa_supplicant add_network)	
wpa_cli -iwlan0 -p/var/run/wpa_supplicant set_network $num ssid '"1234567890-wpa"'
wpa_cli -iwlan0 -p/var/run/wpa_supplicant set_network $num psk '"1234567890"'
wpa_cli -iwlan0 -p/var/run/wpa_supplicant enable_network $num
udhcpc -i wlan0



