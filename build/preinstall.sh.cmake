#!/bin/bash
###############################################################################
## $Id$
###############################################################################

# Editable Variables
CMAKE_COMMAND=cmake

###############################################################################

if [ -z "$(type -p $CMAKE_COMMAND)" ] ; then
	echo "You have to install CMake"
	exit 1
fi

if [ ! -z "@SRCDIR@" ] ; then
	SRCDIR=@SRCDIR@
fi

if [ -z "$SRCDIR" ] ; then
	SRCDIR="$(dirname "$0")/.."
fi

###############################################################################

mecho()
{
	COLOR=$1
	shift
	$CMAKE_COMMAND -E cmake_echo_color --bold $COLOR "$*"
}

###############################################################################

PREFIX=@PREFIX@
BINDIR=@BINDIR@
DATADIR=@DATADIR@
LIBDIR=@LIBDIR@

###############################################################################

while test $# != 0 ; do
	case $1 in
	--*=*)
		ac_option=`expr "X$1" : 'X\([^=]*\)='`
		ac_optarg=`expr "X$1" : 'X[^=]*=\(.*\)'`
		ac_shift=:
	;;
	*)
		ac_option=$1
		ac_optarg=$2
		ac_shift=shift
	;;
	esac

	case $ac_option in
	-prefix | --prefix)
		$ac_shift
		PREFIX=$ac_optarg
	;;
	-bindir | --bindir)
		$ac_shift
		BINDIR=$ac_optarg
	;;
	-datadir | --datadir)
		$ac_shift
		DATADIR=$ac_optarg
	;;
	-libdir | --libdir)
		$ac_shift
		LIBDIR=$ac_optarg
	;;
	*)
		echo "Unknown option: $ac_option"
		exit 1
	;;
	esac

	shift
done

if [ -z "${PREFIX}" ] ; then
	PREFIX=/usr/local
fi

if [ -z "${BINDIR}" ] ; then
	BINDIR=${PREFIX}/bin
fi

if [ -z "${DATADIR}" ] ; then
	DATADIR=${PREFIX}/share/s25rttr
fi

if [ -z "${LIBDIR}" ] ; then
	LIBDIR=${DATADIR}
fi

echo "## Using Path-Prefix \"${PREFIX}\""
echo "## Using Binary Dir \"${BINDIR}\""
echo "## Using Data Dir \"${DATADIR}\""
echo "## Using Library Dir \"${LIBDIR}\""

###############################################################################

# strip ending slash from $DESTDIR
DESTDIR=${DESTDIR%/}

# adding the slash again if DESTDIR is not empty
if [ ! -z "$DESTDIR" ] ; then
	DESTDIR=${DESTDIR}/
	mecho --red "## Using Destination Dir \"${DESTDIR}\""
fi

###############################################################################

mecho --blue "## Creating directories"
mkdir -vp ${DESTDIR}${BINDIR} || exit 1
mkdir -vp ${DESTDIR}${DATADIR} || exit 1
mkdir -vp ${DESTDIR}${DATADIR}/S2 || exit 1
mkdir -vp ${DESTDIR}${DATADIR}/RTTR || exit 1
mkdir -vp ${DESTDIR}${LIBDIR} || exit 1
mkdir -vp ${DESTDIR}${LIBDIR}/driver/video || exit 1
mkdir -vp ${DESTDIR}${LIBDIR}/driver/audio || exit 1

mecho --blue "## Installing binaries"
cp -v ${SRCDIR}/release/bin/rttr.sh ${DESTDIR}${BINDIR} || exit 1

mecho --blue "## Installing RTTR directory"
if [ -d ${SRCDIR}/RTTR/.svn ] ; then
	LANG=C svn --force --non-interactive export ${SRCDIR}/RTTR ${DESTDIR}${DATADIR}/RTTR || exit 1
elif [ -d ${SRCDIR}/RTTR/.bzr ] ; then
	LANG=C bzr export ${SRCDIR}/RTTR ${DESTDIR}${DATADIR}/RTTR || exit 1
else
	cp -rv ${SRCDIR}/RTTR/* ${DESTDIR}${DATADIR}/RTTR || exit 1
fi
rm -f ${DESTDIR}${DATADIR}/RTTR/languages/*.po

mecho --blue "## Installing language files"
cp -v ${SRCDIR}/RTTR/languages/*.mo ${DESTDIR}${DATADIR}/RTTR/languages/ || exit 1

mecho --blue "## Installing S2 placeholder"
echo "creating ${DESTDIR}${DATADIR}/S2/put\ your\ S2-Installation\ in\ here"
touch ${DESTDIR}${DATADIR}/S2/put\ your\ S2-Installation\ in\ here || exit 1

exit 0

###############################################################################