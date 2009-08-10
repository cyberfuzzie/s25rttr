#!/bin/bash

. base.inc.sh

[ -z "$ARCH" ] && usage

upload()
{
	exec 1>upload_${ARCH}.log 2>&1
	
	mkdir -vp ${UPLOADDIR}/archive
	
	echo "" > $WWWDIR/.files
	echo $VERSION > $WWWDIR/.version
	echo $REVISION > $WWWDIR/.revision
	
	## daten für updater entpacken/packen
	
	## debian packages zu repository hinzufügen
	if [ ! -z "$PKGARCH" ] ; then
		if [ -f "${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb" ] ; then
			while [ -f ${WWWDIR}/deb/db/lockfile ] ; do
				sleep 2
			done

			CURRENT=$(reprepro -A $PKGARCH -Vb ${WWWDIR}/deb list sid s25rttr | cut -f 3 -d ' ')
			NEW=$VERSION-$REVISION
			
			echo "changing repository: $CURRENT => $NEW"
			if [ "$NEW" = "$CURRENT" ] ; then
				echo "removing same version"

				while [ -f ${WWWDIR}/deb/db/lockfile ] ; do
					sleep 2
				done

				reprepro -A $PKGARCH -Vb ${WWWDIR}/deb remove sid s25rttr
			fi
			
			echo "adding file ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb to repository"

			while [ -f ${WWWDIR}/deb/db/lockfile ] ; do
				sleep 2
			done

			reprepro -A $PKGARCH -Vb ${WWWDIR}/deb includedeb sid ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb
			echo done
			
			mv -v ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb ${UPLOADDIR}/archive/${NAME}-${REVISION}_${PKGARCH}.deb
		fi
	fi

	## nach Rapidshare hochladen
	for I in $(ls ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.*) ; do
		echo "uploading file $I to rapidshare.com"
		
		export TARGET=${WWWDIR}
		#${BASEDIR}/uploadrs.sh $I
		unset TARGET
		
		echo "done"
		
		mv -v $I ${UPLOADDIR}/archive/$(basename $I)
		
		echo $(basename $I) >> ${WWWDIR}/.files
	done
	
	## Changelog erzeugen
	svn log ${CLIENT} > ${WWWDIR}/changelog.txt
	
	## Build-Log erzeugen
	cat build_${ARCH}.log packager_${ARCH}.log upload_${ARCH}.log > ${WWWDIR}/build_${ARCH}.log

	return 0	
}

if ! upload ; then
	exit 1
fi

exit 0
