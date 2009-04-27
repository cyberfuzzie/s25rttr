#!/bin/sh

cd $(dirname $0)

if [ -z "$ARCH" ] ; then
    export ARCH="local"
fi

export TARGET=/var/www/ra-doersch.de/nightly/s25client
export VERSION=$(date +%Y%m%d)

sed "s/WINDOW_VERSION \".*\"/WINDOW_VERSION \"$(date +%Y%m%d)\"/" ../version.h > /tmp/version.$$
if ! diff -s ../version.h /tmp/version.$$ >/dev/null 2>&1 ; then
	cp -v /tmp/version.$$ ../version.h
else
	rm -f /tmp/version.$$
fi

rm -fr $TARGET/$ARCH
mkdir -p $TARGET
mkdir -p $TARGET/$ARCH

mv $TARGET/build_${ARCH}.log $TARGET/build_${ARCH}_old.log

echo "Build started: $(date)" >$TARGET/build_${ARCH}.log
./make_release.sh >>$TARGET/build_${ARCH}.log 2>&1
EXIT=$?
echo "Build completed: $(date)" >>$TARGET/build_${ARCH}.log

if [ $EXIT != 0 ] ; then
  cat $TARGET/build_${ARCH}.log | /usr/bin/mail -s "Daily Build $VERSION Failed"  sf-team@siedler25.org

  EXIT=1
else
	REVISION=$(grep WINDOW_REVISION ../version.h | cut -d ' ' -f 3 | cut -d \" -f 2)
	rm -f $TARGET/s25rttr_${VERSION}-*_${ARCH}.tar.bz2
	rm -f $TARGET/s25rttr_*-${REVISION}_${ARCH}.tar.bz2
	cp -v s25rttr_$VERSION.tar.bz2 $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
	rm -f s25rttr_$VERSION.tar.bz2

	rm -f $TARGET/$ARCH/music.tar.bz2
	cp -v s25rttr_music.tar.bz2 $TARGET/$ARCH/music.tar.bz2 >> $TARGET/build_${ARCH}.log
	rm -f s25rttr_music.tar.bz2

	if [ -f $TARGET/$ARCH/binaries.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH -xf $TARGET/$ARCH/binaries.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi
	
	if [ -f $TARGET/$ARCH/drivers.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH -xf $TARGET/$ARCH/drivers.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH/libraries.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH -xf $TARGET/$ARCH/libraries.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH/RTTR.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH -xf $TARGET/$ARCH/RTTR.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH/music.tar.bz2 ] ; then
		if [ -d  $TARGET/$ARCH/share/s25rttr ] ; then
			tar -C $TARGET/$ARCH/share/s25rttr -xf $TARGET/$ARCH/music.tar.bz2 >> $TARGET/build_${ARCH}.log
		else
			tar -C $TARGET/$ARCH -xf $TARGET/$ARCH/music.tar.bz2 >> $TARGET/build_${ARCH}.log
		fi
	fi

	rm -f $TARGET/$ARCH/*.tar.bz2

 	echo "${REVISION}" > $TARGET/.revision
	echo "${VERSION}" > $TARGET/.version
	echo "s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2" >> $TARGET/.files
	
	OPWD=$PWD
	cd $TARGET/$ARCH
	md5deep -r -l . > /tmp/files.$$
	cd $OPWD

	find $TARGET/$ARCH -type f -exec bzip2 -v {} >> $TARGET/build_${ARCH}.log 2>&1 \;

	mv /tmp/files.$$ $TARGET/$ARCH/files
	
	touch $TARGET/$ARCH/revision-$REVISION
	touch $TARGET/$ARCH/version-$VERSION

	/srv/buildfarm/uploadrs.sh $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
        rm -f $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log

	EXIT=0
fi

unset VERSION

exit $EXIT
