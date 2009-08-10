## packager_s25client_win32.sh

## ordner verschieben
mv -v share/s25rttr/RTTR .
mv -v share/s25rttr/driver .
mv -v share/s25rttr/S2/* .

## ordner entfernen
rm -rf share

## binaries
cp -v ${BASEDIR}/${CLIENT}/release/bin/rttr.bat start.bat
cp -v ${BUILDDIR}/src/s25client.exe ./
cp -v ${BUILDDIR}/s25update/src/s25update RTTR/
cp -v ${BUILDDIR}/s-c/src/sound-convert.exe RTTR/

## driver (doppelte dll-namen umgehen)
cp -v ${BUILDDIR}/driver/video/*.dll driver/video/
for I in $(ls driver/video/*.dll) ; do
	mv -v $I "$(dirname $I)/$(echo $(basename $I) | sed s/video/rls_/)"
done
cp -v ${BUILDDIR}/driver/audio/*.dll driver/audio/
for I in $(ls driver/audio/*.dll) ; do
	mv -v $I "$(dirname $I)/$(echo $(basename $I) | sed s/audio/rls_/)"
done

## dlls
cp -v /usr/i586-mingw32msvc/bin/libiconv2.dll .
cp -v /usr/i586-mingw32msvc/bin/libintl3.dll .
cp -v /usr/i586-mingw32msvc/bin/libogg-0.dll .
cp -v /usr/i586-mingw32msvc/bin/SDL_mixer.dll .
cp -v /usr/i586-mingw32msvc/bin/SDL.dll .
cp -v /usr/i586-mingw32msvc/bin/libvorbis-0.dll .
cp -v /usr/i586-mingw32msvc/bin/libvorbisfile-3.dll .
cp -v /usr/i586-mingw32msvc/bin/libcurl-4.dll RTTR/
cp -v /usr/i586-mingw32msvc/bin/zlib1.dll RTTR/

## texte
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/readme.txt .
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/keyboardlayout.txt .

## zip erstellen
if ! make_zip ; then
	exit 1
fi

## exit 0
