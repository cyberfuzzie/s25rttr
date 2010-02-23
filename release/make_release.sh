#!/bin/sh

if [ -z "$VERSION" ] ; then
	export VERSION=$(grep WINDOW_VERSION ../version.h | cut -d ' ' -f 3 | cut -d \" -f 2)
fi

if [ -z "$BUILDDIR" ] ; then
	export BUILDDIR=../build
fi

if [ -z "$TARGET" ] ; then
	export TARGET=""
fi

if [ -z "$ARCH" ] ; then
	export ARCH=unknown
fi

echo "Building in $BUILDDIR for $ARCH"

echo "Executing cmake -DPREFIX=. -DCMAKE_BUILD_TYPE=RelWithDebugInfo -DCOMPILEFOR_PLATFORM=$ARCH .."

mkdir -vp $BUILDDIR
OLDDIR=$PWD
cd $BUILDDIR
if ! ../build/cmake.sh --prefix=. --arch=$ARCH --enable-reldeb  ; then
	exit 1
fi
cd $OLDDIR
unset OLDDIR

mkdir -vp $BUILDDIR/RTTR

if ! make -j 2 -C $BUILDDIR ; then
	exit 1
fi

ARCH="$(grep COMPILEFOR $BUILDDIR/CMakeCache.txt | cut -d '=' -f 2 | head -n 1).$(grep COMPILEARCH $BUILDDIR/CMakeCache.txt | cut -d '=' -f 2 | head -n 1)"

sudo rm -v -r $ARCH
mkdir -vp $ARCH
if ! cd $ARCH ; then
	exit 1
fi

mkdir -v -p s25rttr_$VERSION
if ! cd s25rttr_$VERSION ; then
	exit 1
fi

echo $PWD

mkdir -vp bin
mkdir -vp share/s25rttr
mkdir -vp share/s25rttr/S2
mkdir -vp share/s25rttr/driver/video
mkdir -vp share/s25rttr/driver/audio
mkdir -vp lib

cp -v ../../../RTTR/texte/readme.txt .
cp -v ../../../RTTR/texte/keyboardlayout.txt .

rm -rf share/s25rttr/RTTR
svn --force export ../../../RTTR share/s25rttr/RTTR

cd  share/s25rttr

rm -vf ../../../../s25rttr_music.tar.bz2
tar --exclude=.svn -cvjf ../../../../s25rttr_music.tar.bz2 RTTR/MUSIC

cd ../..

cp -rv ../../../RTTR/languages/*.mo share/s25rttr/RTTR/languages/

rm -vf share/s25rttr/RTTR/languages/*.po
rm -vf share/s25rttr/RTTR/REPLAYS/*.rpl
rm -vf share/s25rttr/RTTR/sound.lst
rm -vf share/s25rttr/RTTR/settings.bin
rm -vf share/s25rttr/RTTR/MUSIC/SNG/SNG_*.OGG
rm -vf *~ *.bak

rm -vrf share/s25rttr/S2
mkdir -vp share/s25rttr/S2
touch share/s25rttr/S2/put\ your\ S2-Installation\ in\ here

if [ -f ../../files.$ARCH.sh ] ; then
	../../files.$ARCH.sh
fi

cd ..

rm -vf ../s25rttr_$VERSION.tar.bz2
tar --exclude=.svn -cvjf ../s25rttr_$VERSION.tar.bz2 s25rttr_$VERSION

echo "Release Version $VERSION erstellt"

exit 0
