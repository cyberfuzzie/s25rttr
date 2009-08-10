## packager_s25client_macos.sh

## app anlegen
mkdir -vp s25client.app/Contents/{Frameworks,MacOS,Resources}

## ordner verschieben
mv -v bin s25client.app/Contents/MacOS/
mv -v share s25client.app/Contents/MacOS/

## frameworks kopieren (da updater nicht mit symlinks umgehen kann, nur lib kopieren
mkdir -vp s25client.app/Contents/Frameworks/{SDL,SDL_mixer,sndfile,samplerate}.framework
cp -v /usr/i686-apple-darwin9/SDKs/MacOSX10.4u.sdk/Library/Frameworks/SDL.framework/SDL s25client.app/Contents/Frameworks/SDL.framework/SDL
cp -v /usr/i686-apple-darwin9/SDKs/MacOSX10.4u.sdk/Library/Frameworks/SDL_mixer.framework/SDL_mixer s25client.app/Contents/Frameworks/SDL_mixer.framework/SDL_mixer
cp -v /usr/i686-apple-darwin9/SDKs/MacOSX10.4u.sdk/Library/Frameworks/sndfile.framework/sndfile s25client.app/Contents/Frameworks/sndfile.framework/sndfile
cp -v /usr/i686-apple-darwin9/SDKs/MacOSX10.4u.sdk/Library/Frameworks/samplerate.framework/samplerate s25client.app/Contents/Frameworks/samplerate.framework/samplerate

## paketdaten+scripte kopieren
cp -v ${BASEDIR}/${CLIENT}/release/bin/macos/rttr.command s25client.app/Contents/MacOS/
cp -v ${BASEDIR}/${CLIENT}/release/bin/macos/rttr.terminal s25client.app/Contents/MacOS/
cp -v ${BASEDIR}/${CLIENT}/release/bin/macos/icon.icns s25client.app/Contents/Resources/
cp -v ${BASEDIR}/${CLIENT}/release/bin/macos/PkgInfo s25client.app/Contents/
cp -v ${BASEDIR}/${CLIENT}/release/bin/macos/Info.plist s25client.app/Contents/

## binaries
cp -v ${BASEDIR}/${CLIENT}/release/bin/rttr.sh s25client.app/Contents/MacOS/bin/
cp -v ${BUILDDIR}/src/s25client s25client.app/Contents/MacOS/bin/
cp -v ${BUILDDIR}/s25update/src/s25update s25client.app/Contents/MacOS/share/s25rttr/RTTR/
cp -v ${BUILDDIR}/s-c/src/sound-convert s25client.app/Contents/MacOS/share/s25rttr/RTTR/

## driver
cp -v ${BUILDDIR}/driver/video/*.dylib s25client.app/Contents/MacOS/share/s25rttr/driver/video/
cp -v ${BUILDDIR}/driver/audio/*.dylib s25client.app/Contents/MacOS/share/s25rttr/driver/audio/

## texte
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/readme.txt .
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/keyboardlayout.txt .

## verzeichnisse löschen
rm -vf bin
rm -vf share

SUBDIR="s25client.app readme.txt keyboardlayout.txt"

## tar.bz2 erstellen
if ! make_tarbz2 ; then
	exit 1
fi

## exit 0
