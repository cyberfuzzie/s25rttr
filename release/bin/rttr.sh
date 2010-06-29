#!/bin/bash

DIR=$(dirname $0)

cd $DIR

chmod 0755 $DIR/bin/rttr.sh $DIR/share/s25rttr/RTTR/s25update $DIR/bin/s25client $DIR/share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

if [ "$LD_LIBRARY_PATH" = "" ] ; then
	export LD_LIBRARY_PATH="$DIR/lib"
else
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$DIR/lib"
fi

cd $DIR/..

if [ ! "$1" = "noupdate" ] ; then
	if [ -f ./share/s25rttr/RTTR/s25update ] ; then
		./share/s25rttr/RTTR/s25update
	fi
fi

chmod 0755 ./bin/rttr.sh ./share/s25rttr/RTTR/s25update ./bin/s25client ./share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

if [ "$1" = "debug" ] ; then
	gdb ./bin/s25client
else
	./bin/s25client
fi

exit $?
