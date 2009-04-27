#!/bin/sh

# Variablen die übergeben werden
VARS="PREFIX BINDIR DATADIR"

# temporäre Datei entfernen
rm -f /tmp/input.$$

# Variablen "globalisieren"
J=1
for I in $VARS ; do
	echo -n $I >> /tmp/input.$$
	echo "=\"\$$J\"" >> /tmp/input.$$
	
	J=$(($J+1))
done	

# und einbinden
. /tmp/input.$$

# temporäre Datei entfernen
rm -f /tmp/input.$$

echo "Creating local.h:"

# nur Linux-Teil rausgreppen und ggf verändern
grep -A 50 "Other-OS Dependant-Configuration" local.h | tr -d \\\r > /tmp/output.$$

CHANGED=0
for I in $VARS ; do
	X=`grep "#define $I" /tmp/output.$$ | cut -d " " -f 3 | tr -d \"\\\n\\\r`
	Y=`eval "echo \\$$I"`
	Z=`grep "#define $I" /tmp/output.$$ | cut -d " " -f 1 | tr -cd /`
	
	#echo $X
	
	echo "  $I=$Y"
	
	if (test "$Z" == "//") && (test "${Y}err" != "err") ; then
		# Variable ist auskommentiert, einkommentieren
		sed -i "s/\/\/#define $I/#define $I/" local.h
	fi
	if (test "$Z" != "//") && (test "${Y}err" == "err") ; then
		# Variable ist einkommentiert, aber (neuer Wert) leer -> auskommentieren
		sed -i "s/#define $I/\/\/#define $I/" local.h
	fi
	
	if (test "$X" != "$Y") && (test "${Y}err" != "err") ; then
		# Variablen sind unterschiedlich -> neuen Wert setzen
		sed -i "s#\#define $I \"$X\"#\#define $I \"$Y\"#" local.h
	fi
done

rm -f /tmp/output.$$

echo "done"

exit 0
