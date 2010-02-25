#!/bin/sh

cp -v ../../bin/rttr.sh bin/
#cp -v ../../bin/rttr-update.sh bin/
cp -v ../../$BUILDDIR/src/s25client bin/
#cp -v ../../$BUILDDIR/libsiedler2/src/libsiedler2.so lib/
cp -v ../../$BUILDDIR/driver/video/*.so share/s25rttr/driver/video
cp -v ../../$BUILDDIR/driver/audio/*.so share/s25rttr/driver/audio

cp -v ../../$BUILDDIR/s25update/src/s25update share/s25rttr/RTTR
cp -v ../../$BUILDDIR/s-c/src/sound-convert share/s25rttr/RTTR
cp -v ../../$BUILDDIR/s-c/resample-1.8.1/src/s-c_resample share/s25rttr/RTTR

# make deb-package
PKGARCH=amd64

rm -rvf ../deb/s25rttr_$VERSION
mkdir -vp ../deb/s25rttr_$VERSION/usr/share/{pixmaps,applications}
mkdir -vp ../deb/s25rttr_$VERSION/DEBIAN

cp -rv . ../deb/s25rttr_$VERSION/usr
cp -rv ../../debian/control ../deb/s25rttr_$VERSION/DEBIAN/control

# updater entfernen
rm -vf ../deb/s25rttr_$VERSION/usr/share/s25rttr/RTTR/s25update

cp -v ../../debian/s25rttr.png ../deb/s25rttr_$VERSION/usr/share/pixmaps/s25rttr.png
cp -v ../../debian/s25rttr.desktop ../deb/s25rttr_$VERSION/usr/share/applications/s25rttr.desktop

mkdir -vp ../deb/s25rttr_$VERSION/usr/share/doc/s25rttr
mv -v ../deb/s25rttr_$VERSION/usr/*.txt ../deb/s25rttr_$VERSION/usr/share/doc/s25rttr

sed -i "s/Installed-Size: x/Installed-Size: $(du -sk ../deb/s25rttr_$VERSION | cut -f 1)/" ../deb/s25rttr_$VERSION/DEBIAN/control
sed -i "s/Version: x/Version: $(grep WINDOW_VERSION ../../../version.h | cut -d ' ' -f 3 | cut -d \" -f 2)-$(grep WINDOW_REVISION ../../../version.h | cut -d ' ' -f 3 | cut -d \" -f 2)/"  ../deb/s25rttr_$VERSION/DEBIAN/control
sed -i "s/Architecture: x/Architecture: $PKGARCH/" ../deb/s25rttr_$VERSION/DEBIAN/control


mkdir -vp ../../deb

sudo chown -R root.games ../deb/s25rttr_$VERSION
sudo chmod 0755 ../deb/s25rttr_$VERSION
sudo dpkg -b ../deb/s25rttr_$VERSION/ ../../deb/s25rttr_${VERSION}_${PKGARCH}.deb
sudo chown sifldoer.Domain\ Admins ../../deb/s25rttr_${VERSION}_${PKGARCH}.deb
sudo chown -R sifldoer.Domain\ Admins ../deb/s25rttr_$VERSION
dpkg-sig -v -k 6D09334C -s builder ../../deb/s25rttr_${VERSION}_${PKGARCH}.deb


# copy to target
if [ ! -z "$TARGET" ] ; then
	#reprepro -Vb $TARGET/deb remove sid s25rttr
	reprepro -Vb $TARGET/deb includedeb sid ../../deb/s25rttr_${VERSION}_${PKGARCH}.deb
	
	mkdir -p $TARGET/$ARCH.new
	
	BINARIES="bin/rttr.sh share/s25rttr/RTTR/s25update bin/s25client share/s25rttr/RTTR/sound-convert"
	rm -rv $TARGET/$ARCH.new/binaries.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/binaries.tar.bz2 $BINARIES
	
	#LIBRARIES="lib/libsiedler2.so"
	#rm -rv $TARGET/$ARCH.new/libraries.tar.bz2
	#tar --exclude=.svn -cvjf $TARGET/$ARCH.new/libraries.tar.bz2 $LIBRARIES
	
	rm -rv $TARGET/$ARCH.new/drivers.tar.bz2
	tar --exclude=.svn --exclude=share/s25rttr/RTTR/sound-convert -cvjf $TARGET/$ARCH.new/RTTR.tar.bz2 share/s25rttr/RTTR
	
	rm -rv $TARGET/$ARCH.new/drivers.tar.bz2
	tar --exclude=.svn -cvjf $TARGET/$ARCH.new/drivers.tar.bz2 share/s25rttr/driver
	
	cp -v readme.txt $TARGET/$ARCH.new
fi
