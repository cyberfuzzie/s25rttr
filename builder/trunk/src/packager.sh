#!/bin/bash

. base.inc.sh

[ -z "$ARCH" ] && usage

packager()
{
	exec 1>packager_${ARCH}.log 2>&1
	
	mkdir -vp $ARCH
	if ! cd $ARCH ; then
		return 1
	fi
	
	. ${BASE}/packager_${CLIENT}.inc.sh

	return 0
}

if ! packager ; then
	exit 1
fi

exit 0
