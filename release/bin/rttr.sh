#!/bin/bash

DIR=$(dirname $0)

chmod 0755 $DIR/../bin/rttr.sh $DIR/../share/s25rttr/RTTR/s25update $DIR/../bin/s25client $DIR/../share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

if [ "$LD_LIBRARY_PATH" = "" ] ; then
	export LD_LIBRARY_PATH="$DIR/../lib"
else
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$DIR/../lib"
fi

if [ ! "$1" = "noupdate" ] ; then
	if [ -f $DIR/../share/s25rttr/RTTR/s25update ] ; then
		echo "checking for an update ..."
		cp $DIR/../share/s25rttr/RTTR/s25update /tmp/s25update.$$
		chmod 0755 /tmp/s25update.$$
		/tmp/s25update.$$ --verbose --dir $DIR/../
		if [ -z "$(diff -q /tmp/s25update.$$ $DIR/../share/s25rttr/RTTR/s25update)" ] ; then
			PARAM=noupdate
		fi
		$DIR/../bin/rttr.sh $PARAM $*
		exit $?
	fi
else
	shift
fi

chmod 0755 $DIR/../bin/rttr.sh $DIR/../share/s25rttr/RTTR/s25update $DIR/../bin/s25client $DIR/../share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

CMD=

case "$1" in
	debug)
		CMD=gdb
	;;
	valgrind)
		CMD=valgrind
	;;
	ldd)
		CMD=ldd
	;;
esac
shift

$CMD $DIR/../bin/s25client $*

exit $?
