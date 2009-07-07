#!/bin/bash

cd $(dirname $0)

chmod 0755 ../rttr.command ../share/s25rttr/RTTR/s25update ../bin/s25client ../share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

if [ ! "$1" = "noupdate" ] ; then
	if [ -f ./share/s25rttr/RTTR/s25update ] ; then
		./share/s25rttr/RTTR/s25update
	fi
fi

chmod 0755 ./rttr.command ./share/s25rttr/RTTR/s25update ./bin/s25client ./share/s25rttr/RTTR/sound-convert >/dev/null 2>/dev/null

./bin/s25client

exit $?
