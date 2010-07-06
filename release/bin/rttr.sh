#!/bin/bash

DIR=$(dirname "$0")

chmod 0755 $DIR/../bin/rttr.sh $DIR/../share/s25rttr/RTTR/s25update $DIR/../bin/s25client $DIR/../share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

if [ "$LD_LIBRARY_PATH" = "" ] ; then
	export LD_LIBRARY_PATH="$DIR/../lib"
else
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$DIR/../lib"
fi

cmd=
noupdate=0
for I in $1; do
	case $I in
		debug)
			noupdate=1
			cmd=gdb
		;;
		noupdate)
			noupdate=1
		;;
		gdb|ldd|valgrind|strace)
			noupdate=1
			cmd=$I
		;;
	esac
done

if [ $noupdate -eq 0 ] ; then
	if [ -f $DIR/../share/s25rttr/RTTR/s25update ] ; then
		echo "checking for an update ..."
		cp $DIR/../share/s25rttr/RTTR/s25update /tmp/s25update.$$
		chmod 0755 /tmp/s25update.$$
		/tmp/s25update.$$ --verbose --dir "$DIR/../"
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

cd $DIR
$cmd $DIR/../bin/s25client $*

exit $?
