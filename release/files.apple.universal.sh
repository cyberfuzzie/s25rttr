#!/bin/sh

cp -v ../../$BUILDDIR/src/s25client bin/
#cp -v ../../$BUILDDIR/libsiedler2/src/libsiedler2.dylib lib/
cp -v ../../$BUILDDIR/driver/video/*.dylib share/s25rttr/driver/video
cp -v ../../$BUILDDIR/driver/audio/*.dylib share/s25rttr/driver/audio

cp -v ../../$BUILDDIR/s25update/src/s25update share/s25rttr/RTTR
cp -v ../../$BUILDDIR/s-c/src/sound-convert share/s25rttr/RTTR
cp -v ../../$BUILDDIR/s-c/resample-1.8.1/src/s-c_resample share/s25rttr/RTTR

# app anlegen
mkdir -vp s25client.app/Contents/{MacOS,Resources}

# frameworks kopieren (da updater nicht mit symlinks umgehen kann, nur lib kopieren
mkdir -vp s25client.app/Contents/MacOS/Frameworks
mkdir -vp s25client.app/Contents/MacOS/Frameworks/{SDL,SDL_mixer}.framework
cp -rv /Library/Frameworks/SDL.framework s25client.app/Contents/MacOS/Frameworks
cp -rv /Library/Frameworks/SDL_mixer.framework s25client.app/Contents/MacOS/Frameworks

# remove headers and additional libraries from the frameworks
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL_mixer.framework/Versions/*/{Headers,Resources}
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL_mixer.framework/Versions/*/*/{Headers,Resources}
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL_mixer.framework/Versions/*/*/*/*/{Headers,Resources}
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL_mixer.framework/Versions/*/*/*/*/*/{Headers,Resources}
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL.framework/Versions/*/{Headers,Resources}
rm -rv s25client.app/Contents/MacOS/Frameworks/SDL.framework/Versions/*/*/{Headers,Resources}

# copy miniupnp
cp -rv /srv/buildfarm/SDKs/MacOSX10.5.sdk/usr/lib/libminiupnpc.5.dylib s25client.app/Contents/MacOS/Frameworks

mkdir -vp s25client.app/Contents/MacOS/bin

# binaries und paketdaten kopieren
cp -v ../../bin/macos/rttr.command s25client.app/Contents/MacOS/
cp -v ../../bin/macos/rttr.terminal s25client.app/Contents/MacOS/
cp -v ../../bin/macos/icon.icns s25client.app/Contents/Resources/
cp -v ../../bin/macos/PkgInfo s25client.app/Contents/
cp -v ../../bin/macos/Info.plist s25client.app/Contents/
mv -v bin/* s25client.app/Contents/MacOS/bin

rm -rf bin
rm -rf lib

# RTTR-Ordner kopieren	
mv -v share s25client.app/Contents/MacOS/

if [ ! -z "$TARGET" ] ; then
	mkdir -p $TARGET/$ARCH.new
	
	BINARIES="s25client.app"
	rm -rv $TARGET/$ARCH.new/binaries.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/binaries.tar.bz2 $BINARIES
	
	#LIBRARIES="lib/libsiedler2.dylib"
	#rm -rv $TARGET/$ARCH.new/libraries.tar.bz2
	#tar --exclude=.svn -cvjf $TARGET/$ARCH.new/libraries.tar.bz2 $LIBRARIES

	#rm -rv $TARGET/$ARCH.new/RTTR.tar.bz2
	#tar --exclude=.svn --exclude=share/s25rttr/RTTR/sound-convert -cvjf $TARGET/$ARCH.new/RTTR.tar.bz2 share/s25rttr/RTTR

	#rm -rv $TARGET/$ARCH.new/drivers.tar.bz2
	#tar --exclude=.svn -cvjf $TARGET/$ARCH.new/drivers.tar.bz2 share/s25rttr/driver
	
	cp -v readme.txt $TARGET/$ARCH.new
fi
