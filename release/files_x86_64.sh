#!/bin/sh

cp -v ../../bin/rttr.sh bin/
#cp -v ../../bin/rttr-update.sh bin/
cp -v ../../$BUILDDIR/src/s25client bin/
#cp -v ../../$BUILDDIR/libsiedler2/src/libsiedler2.so lib/
cp -v ../../$BUILDDIR/driver/video/*.so share/s25rttr/driver/video
cp -v ../../$BUILDDIR/driver/audio/*.so share/s25rttr/driver/audio

cp -v ../../$BUILDDIR/s25update/src/s25update share/s25rttr/RTTR
cp -v ../../$BUILDDIR/s-c/src/sound-convert share/s25rttr/RTTR

if [ ! -z "$TARGET" ] ; then
	mkdir -p $TARGET/$ARCH
	
	BINARIES="bin/rttr.sh share/s25rttr/RTTR/s25update bin/s25client share/s25rttr/RTTR/sound-convert"
	rm -rv $TARGET/$ARCH/binaries.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH/binaries.tar.bz2 $BINARIES
	
	#LIBRARIES="lib/libsiedler2.so"
	#rm -rv $TARGET/$ARCH/libraries.tar.bz2
	#tar --exclude=.svn -cvjf $TARGET/$ARCH/libraries.tar.bz2 $LIBRARIES

	rm -rv $TARGET/$ARCH/drivers.tar.bz2
	tar --exclude=.svn --exclude=share/s25rttr/RTTR/sound-convert -cvjf $TARGET/$ARCH/RTTR.tar.bz2 share/s25rttr/RTTR

	rm -rv $TARGET/$ARCH/drivers.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH/drivers.tar.bz2 share/s25rttr/driver
	
	cp -v readme.txt $TARGET/$ARCH
fi
