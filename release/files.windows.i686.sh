#!/bin/sh

rm -vrf bin
rm -vrf lib

#mkdir -p bin
#cp -v ../../bin/*.exe bin
cp -v ../../bin/rttr.bat start.bat

if [ -f ../../$BUILDDIR/src/s25client.exe ] ; then
	cp -v ../../$BUILDDIR/src/s25client.exe s25client.exe
else
	cp -v ../../$BUILDDIR/src/s25client s25client.exe
fi

cp -v ../../$BUILDDIR/driver/video/*.dll share/s25rttr/driver/video/
for I in $(ls share/s25rttr/driver/video/*.dll) ; do
	mv -v $I "$(dirname $I)/$(echo $(basename $I) | sed s/video/rls_/)"
done

cp -v ../../$BUILDDIR/driver/audio/*.dll share/s25rttr/driver/audio/
for I in $(ls share/s25rttr/driver/audio/*.dll) ; do
        mv -v $I "$(dirname $I)/$(echo $(basename $I) | sed s/audio/rls_/)"
done

if [ -f ../../$BUILDDIR/s25update/src/s25update.exe ] ; then
	cp -v ../../$BUILDDIR/s25update/src/s25update.exe share/s25rttr/RTTR/s25update.exe
else	
	cp -v ../../$BUILDDIR/s25update/src/s25update share/s25rttr/RTTR/s25update.exe
fi

if [ -f ../../$BUILDDIR/s-c/src/sound-convert.exe ] ; then
	cp -v ../../$BUILDDIR/s-c/src/sound-convert.exe share/s25rttr/RTTR/sound-convert.exe
else
	cp -v ../../$BUILDDIR/s-c/src/sound-convert share/s25rttr/RTTR/sound-convert.exe
fi

if [ -f ../../$BUILDDIR/s-c/src/s-c_resample.exe ] ; then
        cp -v ../../$BUILDDIR/s-c/src/s-c_resample.exe share/s25rttr/RTTR/s-c_resample.exe
else
        cp -v ../../$BUILDDIR/s-c/src/s-c_resample share/s25rttr/RTTR/s-c_resample.exe
fi

cp -v /usr/i586-mingw32msvc/bin/libsamplerate.dll share/s25rttr/RTTR
cp -v /usr/i586-mingw32msvc/bin/libsndfile-1.dll share/s25rttr/RTTR

cp -v /usr/i586-mingw32msvc/bin/libiconv2.dll .
cp -v /usr/i586-mingw32msvc/bin/libintl3.dll .
cp -v /usr/i586-mingw32msvc/bin/libogg-0.dll .
cp -v /usr/i586-mingw32msvc/bin/SDL_mixer.dll .
cp -v /usr/i586-mingw32msvc/bin/SDL.dll .
cp -v /usr/i586-mingw32msvc/bin/libvorbis-0.dll .
cp -v /usr/i586-mingw32msvc/bin/libvorbisfile-3.dll .
cp -v /usr/i586-mingw32msvc/bin/libcurl-4.dll share/s25rttr/RTTR
cp -v /usr/i586-mingw32msvc/bin/zlib1.dll share/s25rttr/RTTR

mv -v share/s25rttr/driver .
mv -v share/s25rttr/S2/* .
mv -v share/s25rttr/RTTR .

rm -vrf share

if [ ! -z "$TARGET" ] ; then
	mkdir -p $TARGET/$ARCH.new
	
	BINARIES="start.bat RTTR/s25update.exe s25client.exe RTTR/sound-convert.exe"
	rm -v $TARGET/$ARCH.new/binaries.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/binaries.tar.bz2 $BINARIES

	LIBRARIES="libiconv2.dll libintl3.dll SDL_mixer.dll SDL.dll RTTR/libcurl-4.dll RTTR/zlib1.dll libvorbisfile-3.dll libvorbis-0.dll libogg-0.dll"
	# RTTR/libsamplerate.dll RTTR/libsndfile-1.dll"
	rm -v $TARGET/$ARCH.new/libraries.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/libraries.tar.bz2 $LIBRARIES

	rm -v $TARGET/$ARCH.new/RTTR.tar.bz2
	tar --exclude=.svn --exclude=RTTR/libsamplerate.dll --exclude=RTTR/libsndfile-1.dll \
	    --exclude=RTTR/s25update.exe --exclude=RTTR/libcurl-4.dll --exclude=RTTR/zlib1.dll \
	    --exclude=RTTR/sound-convert.exe -cvjf $TARGET/$ARCH.new/RTTR.tar.bz2 RTTR
	
	rm -v $TARGET/$ARCH.new/drivers.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/drivers.tar.bz2 driver
	
	cp -v readme.txt $TARGET/$ARCH.new
fi
