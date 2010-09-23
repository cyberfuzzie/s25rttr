#!/bin/bash
###############################################################################
## $Id: postinstall.sh.cmake 6755 2010-09-23 09:54:47Z FloSoft $
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

COMPILEFOR=@COMPILEFOR@
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
	-compilefor | --compilefor)
		$ac_shift
		COMPILEFOR=$ac_optarg
	;;
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

if [ -z "${COMPILEFOR}" ] ; then
	COMPILEFOR=$(uname -s | tr '[:upper:]' '[:lower:]')
fi

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

echo "## Installing for \"${COMPILEFOR}\""
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

mecho --blue "## Removing files which are unused (but installed by cmake)"
rm -vf ${DESTDIR}${LIBDIR}/driver/video/libvideo*.{a,lib}
rm -vf ${DESTDIR}${LIBDIR}/driver/audio/libaudio*.{a,lib}

# create app-bundle for apple
if [ "$COMPILEFOR" = "apple" ] ; then
	# app anlegen
	mkdir -vp ${DESTDIR}s25client.app/Contents/{MacOS,Resources} || exit 1
	
	# frameworks kopieren
	mkdir -vp ${DESTDIR}s25client.app/Contents/MacOS/Frameworks || exit 1
	mkdir -vp ${DESTDIR}s25client.app/Contents/MacOS/Frameworks/{SDL,SDL_mixer}.framework || exit 1
	cp -r /Library/Frameworks/SDL.framework ${DESTDIR}s25client.app/Contents/MacOS/Frameworks || exit 1
	cp -r /Library/Frameworks/SDL_mixer.framework ${DESTDIR}s25client.app/Contents/MacOS/Frameworks || exit 1
	
	# remove headers and additional libraries from the frameworks
	find ${DESTDIR}s25client.app/Contents/MacOS/Frameworks/ -name Headers -exec rm -rf {} \;
	find ${DESTDIR}s25client.app/Contents/MacOS/Frameworks/ -name Resources -exec rm -rf {} \;
	
	# copy miniupnp
	if [ -f /Developer/SDKs/MacOSX10.5.sdk/usr/lib/libminiupnpc.5.dylib ] ; then
		cp -rv /Developer/SDKs/MacOSX10.5.sdk/usr/lib/libminiupnpc.5.dylib ${DESTDIR}s25client.app/Contents/MacOS || exit 1
	elif  [ -f /srv/buildfarm/SDKs/MacOSX10.5.sdk/usr/lib/libminiupnpc.5.dylib ] ; then
		cp -rv /srv/buildfarm/SDKs/MacOSX10.5.sdk/usr/lib/libminiupnpc.5.dylib ${DESTDIR}s25client.app/Contents/MacOS || exit 1
	else
		echo "libminiupnpc.5.dylib was not found"
		exit 1
	fi

	mkdir -vp ${DESTDIR}s25client.app/Contents/MacOS/bin || exit 1

	# binaries und paketdaten kopieren
	cp -v ${SRCDIR}/release/bin/macos/rttr.command ${DESTDIR}s25client.app/Contents/MacOS/ || exit 1
	cp -v ${SRCDIR}/release/bin/macos/rttr.terminal ${DESTDIR}s25client.app/Contents/MacOS/ || exit 1
	cp -v ${SRCDIR}/release/bin/macos/icon.icns ${DESTDIR}s25client.app/Contents/Resources/ || exit 1
	cp -v ${SRCDIR}/release/bin/macos/PkgInfo ${DESTDIR}s25client.app/Contents/ || exit 1
	cp -v ${SRCDIR}/release/bin/macos/Info.plist ${DESTDIR}s25client.app/Contents/ || exit 1
	mv -v ${DESTDIR}bin/* ${DESTDIR}s25client.app/Contents/MacOS/bin/ || exit 1
	
	# remove dirs if empty
	rmdir ${DESTDIR}bin
	rmdir ${DESTDIR}lib
	
	# RTTR-Ordner kopieren	
	mv -v ${DESTDIR}share ${DESTDIR}s25client.app/Contents/MacOS/ || exit 1

elif [ "$COMPILEFOR" = "windows" ] ; then
	cp -v /usr/i586-mingw32msvc/bin/libiconv2.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/libintl3.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/libogg-0.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/SDL_mixer.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/SDL.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/libvorbis-0.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/libvorbisfile-3.dll ${DESTDIR} || exit 1
	cp -v /usr/i586-mingw32msvc/bin/libcurl-4.dll ${DESTDIR}RTTR || exit 1
	cp -v /usr/i586-mingw32msvc/bin/zlib1.dll ${DESTDIR}RTTR || exit 1
fi

exit 0

###############################################################################
