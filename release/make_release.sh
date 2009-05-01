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
	export ARCH=local
fi

echo "Building in $BUILDDIR for $ARCH"

echo "Executing cmake -DPREFIX=. -DCMAKE_BUILD_TYPE=Debug -DCOMPILEFOR_PLATFORM=$ARCH .."

mkdir -vp $BUILDDIR
OLDDIR=$PWD
cd $BUILDDIR
if ! cmake -DPREFIX=. -DCMAKE_BUILD_TYPE=Debug -DCOMPILEFOR_PLATFORM=$ARCH ..  ; then
	exit 1
fi
cd $OLDDIR
unset OLDDIR

mkdir -vp $BUILDDIR/RTTR

if ! make -C $BUILDDIR ; then
	exit 1
fi

rm -vr $ARCH
mkdir -vp $ARCH
if ! cd $ARCH ; then
	exit 1
fi

mkdir -vp s25rttr_$VERSION
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

rm -vf share/s25rttr/RTTR/REPLAYS/*.rpl
rm -vf share/s25rttr/RTTR/sound.lst
rm -vf share/s25rttr/RTTR/settings.bin
rm -vf share/s25rttr/RTTR/MUSIC/SNG/SNG_*.OGG
rm -vf *~ *.bak

rm -vrf share/s25rttr/S2
mkdir -vp share/s25rttr/S2
touch share/s25rttr/S2/put\ your\ S2-Installation\ in\ here

if [ -f ../../files_$ARCH.sh ] ; then
	../../files_$ARCH.sh
fi

cd ..

rm -vf ../s25rttr_$VERSION.tar.bz2
tar --exclude=.svn -cvjf ../s25rttr_$VERSION.tar.bz2 s25rttr_$VERSION

echo "Release Version $VERSION erstellt"

exit 0
