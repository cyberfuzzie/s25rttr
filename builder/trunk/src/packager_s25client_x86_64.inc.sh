## packager_s25client_x86_64.sh

## binaries
cp -v ${BASEDIR}/${CLIENT}/release/bin/rttr.sh bin/
cp -v ${BUILDDIR}/src/s25client bin/
cp -v ${BUILDDIR}/s25update/src/s25update share/s25rttr/RTTR/
cp -v ${BUILDDIR}/s-c/src/sound-convert share/s25rttr/RTTR/

## driver
cp -v ${BUILDDIR}/driver/video/*.so share/s25rttr/driver/video/
cp -v ${BUILDDIR}/driver/audio/*.so share/s25rttr/driver/audio/

## texte
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/readme.txt .
cp -v ${BASEDIR}/${CLIENT}/RTTR/texte/keyboardlayout.txt .

## tar.bz2 erstellen
if ! make_tarbz2 ; then
	exit 1
fi

PKGARCH=amd64

## deb-package dirs+files anlegen
mkdir -vp usr
mv -v bin usr/
mv -v share usr/

mkdir -vp usr/share/{pixmaps,applications}
mkdir -vp DEBIAN

cp -rv ${BASEDIR}/${CLIENT}/release/debian/control DEBIAN/control
cp -v ${BASEDIR}/${CLIENT}/release/debian/s25rttr.png usr/share/pixmaps/s25rttr.png
cp -v ${BASEDIR}/${CLIENT}/release/debian/s25rttr.desktop usr/share/applications/s25rttr.desktop

## updater entfernen
rm -vf usr/share/s25rttr/RTTR/s25update

## texte verschieben
mkdir -vp usr/share/doc/s25rttr
mv -v *.txt usr/share/doc/s25rttr

## control file anpassen
sed -i "s/Installed-Size: x/Installed-Size: $(du -sk . | cut -f 1)/" DEBIAN/control
sed -i "s/Version: x/Version: ${VERSION}-${REVISION}/"  DEBIAN/control
sed -i "s/Architecture: x/Architecture: $PKGARCH/" DEBIAN/control

## deb-file erstellen
if ! make_deb ; then
	exit 1
fi

## exit 0
