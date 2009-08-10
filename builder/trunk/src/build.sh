#!/bin/bash

. base.inc.sh

[ -z "$ARCH" ] && usage

build()
{
	exec 1>build_${ARCH}.log 2>&1
	
	## $BUILDDIR erstellen
	mkdir -vp $BUILDDIR
	
	## cmake ausführen
	OLDDIR=$PWD
	cd $BUILDDIR
	if ! cmake -DPREFIX=. -DCMAKE_BUILD_TYPE=RelWithDebugInfo -DCOMPILEFOR_PLATFORM=$ARCH ..  ; then
		return 1
	fi
	cd $OLDDIR
	unset OLDDIR
	
	## RTTR-Verzeichnis erstellen
	mkdir -vp $BUILDDIR/RTTR
	
	## Bauen
	if ! make -j 2 -C $BUILDDIR ; then
		return 1
	fi
	
	return 0
}

if ! build ; then
	exit 1
fi

exit 0
