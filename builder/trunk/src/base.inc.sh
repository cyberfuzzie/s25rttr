## base.inc.sh

usage()
{
	echo "usage: " >&2
	echo "--arch={macos|i686|x86_64|win32}" >&2
	echo "--client=nameofclient" >&2
	echo ""	>&2
	
	exit 1
}

make_zip()
{
	if [ -z "$NAME" ] ; then
		return 1
	fi
	
	cd ..

	sudo chown -R root.games ${NAME}
	sudo chmod -R 0755 ${NAME}

	rm -f ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.zip

	echo "creating ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.zip"
	if ! zip -vr ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH} ${NAME} -x .svn ; then
		echo "failed"
		cd ${NAME}
		return 1
	fi
	echo "done"
	
	sudo chmod -R 0777 ${NAME}
	cd ${NAME}
	
	return 0
}

make_tarbz2()
{
	if [ -z "$NAME" ] ; then
		return 1
	fi
	
	cd ..

	sudo chown -R root.games ${NAME}
	sudo chmod -R 0755 ${NAME}

	rm -f ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.tar.bz2
	
	PARAMS=
	if [ ! -z "$SUBDIR" ] ; then
		PARAMS="-C ${NAME}"
	else
		SUBDIR=${NAME}
	fi

	echo "creating ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.tar.bz2"
	if ! tar $PARAMS --exclude=.svn -cvjf ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.tar.bz2 ${SUBDIR} ; then
		echo "failed"
		cd ${NAME}
		return 1
	fi
	echo "done"
	
	sudo chmod -R 0777 ${NAME}
	cd ${NAME}
	
	return 0
}

make_deb()
{
	if [ -z "$NAME" ] ; then
		return 1
	fi

	if [ -z "$PKGARCH" ] ; then
		return 1
	fi

	cd ..

	sudo chown -R root.games ${NAME}
	sudo chmod -R 0755 ${NAME}

	rm -f ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb

	echo "creating ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb"
	if ! sudo dpkg -b ${NAME} ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb ; then
		echo "failed"
		cd ${NAME}
		return 1
	fi
	echo "done"

	sudo chmod -R 0777 ${NAME}
	sudo chmod 0777 ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb
	dpkg-sig -v -k 6D09334C -s builder ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb

	cd ${NAME}

	return 0
}

set	-- `getopt -n$0	-u -a --longoptions="arch: client:"	"h"	"$@"` || usage
[ $# -eq 0 ] &&	usage

BUILDER=FloSoft
ARCH=
CLIENT=
BASEDIR=/srv/buildfarm
UPLOADDIR=${BASEDIR}/uploads
BASE=${BASEDIR}/builder/src
WWWDIR=/www/ra-doersch.de/nightly/s25client

export BUILDER ARCH CLIENT BASEDIR UPLOADDIR BASE WWWDIR

mkdir -vp $UPLOADDIR $BASEDIR $BASE $WWWDIR

while [	$# -gt 0 ]
do
	case "$1" in
		--client)	CLIENT=$2;shift;;
		--arch)		ARCH=$2;shift;;
		-h)			usage;;
		--)			shift;break;;
		-*)			usage;;
		*)			break;;
	esac
	shift
done

[ -z "$CLIENT" ] && usage

case "$ARCH" in
	i686)
		PKGARCH=i386
	;;
	x86_64)
		PKGARCH=amd64
	;;
	*)
		PKGARCH=
	;;
esac

export PKGARCH

BUILDDIR=${BASEDIR}/${CLIENT}/build_${ARCH}
VERSION=$(grep WINDOW_VERSION ${BASEDIR}/${CLIENT}/version.h | cut -d ' ' -f 3 | cut -d \" -f 2)
REVISION=$(grep WINDOW_REVISION ${BASEDIR}/${CLIENT}/version.h | cut -d ' ' -f 3 | cut -d \" -f 2)

[ -z "$VERSION" ] && echo "failed to read version" >&2
[ -z "$REVISION" ] && echo "failed to read revision" >&2

export BUILDDIR VERSION REVISION

. $BASE/config_${CLIENT}.inc.sh

## exit 0
