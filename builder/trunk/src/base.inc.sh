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
	if ! zip -vr ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH} ${NAME} -x .svn ; then
		cd ${NAME}
		return 1
	fi
	
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
	if ! tar --exclude=.svn -cvjf ${UPLOADDIR}/${NAME}-${REVISION}_${ARCH}.tar.bz2 ${NAME} ; then
		cd ${NAME}
		return 1
	fi
	
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
	if ! sudo dpkg -b ${NAME} ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb ; then
		cd ${NAME}
		return 1
	fi

	sudo chmod -R 0777 ${NAME}
	sudo chmod 0777 ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb
	dpkg-sig -v -k 6D09334C -s builder ${UPLOADDIR}/${NAME}-${REVISION}_${PKGARCH}.deb

	cd ${NAME}

	return 0
}

set	-- `getopt -n$0	-u -a --longoptions="arch: client:"	"h"	"$@"` || usage
[ $# -eq 0 ] &&	usage

ARCH=
CLIENT=
BASEDIR=/srv/buildfarm
UPLOADDIR=${BASEDIR}/uploads
BASE=${BASEDIR}/builder/src

export ARCH CLIENT BASEDIR UPLOADDIR BASE

mkdir -vp $UPLOADDIR $BASEDIR $BASE

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

BUILDDIR=${BASEDIR}/${CLIENT}/build_${ARCH}
VERSION=$(grep WINDOW_VERSION ${BASEDIR}/${CLIENT}/version.h | cut -d ' ' -f 3 | cut -d \" -f 2)
REVISION=$(grep WINDOW_REVISION ${BASEDIR}/${CLIENT}/version.h | cut -d ' ' -f 3 | cut -d \" -f 2)

[ -z "$VERSION" ] && echo "failed to read version" >&2
[ -z "$REVISION" ] && echo "failed to read revision" >&2

export BUILDDIR VERSION REVISION

## exit 0
