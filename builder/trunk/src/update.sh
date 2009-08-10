#!/bin/bash

. base.inc.sh

update()
{
	exec 1>update_$CLIENT.log 2>&1

	## revert files from svn:revert
	for I in $(svn propget svn:revert ${BASEDIR}/${CLIENT}) ; do
		svn revert ${BASEDIR}/${CLIENT}/$I
	done
	
	## update source
	if ! svn up ${BASEDIR}/${CLIENT} ; then
		return 1
	fi
	
	## update version
	
	VERSION=$(date +%Y%m%d)
	
	sed "s/WINDOW_VERSION \".*\"/WINDOW_VERSION \"$(date +%Y%m%d)\"/" ${BASEDIR}/${CLIENT}/version.h > /tmp/version.$$
	if ! diff -s ${BASEDIR}/${CLIENT}/version.h /tmp/version.$$ >/dev/null 2>&1 ; then
		cp -v /tmp/version.$$ ${BASEDIR}/${CLIENT}/version.h
	else
		rm -f /tmp/version.$$
	fi
	
	return 0
}

if ! update ; then
	exit 1
fi

exit 0
