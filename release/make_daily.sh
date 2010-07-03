#!/bin/sh

cd $(dirname $0)

if [ -z "$ARCH" ] ; then
    export ARCH="local"
fi

export TARGET=/var/www/ra-doersch.de/nightly/s25client
export VERSION=$(date +%Y%m%d)

#sed "s/WINDOW_VERSION \".*\"/WINDOW_VERSION \"$(date +%Y%m%d)\"/" ../version.h > /tmp/version.$$
#if ! diff -s $BUILDDIR/build_version.h /tmp/version.$$ >/dev/null 2>&1 ; then
#	cp -v /tmp/version.$$ $BUILDDIR/version.h
#else
#	rm -f /tmp/version.$$
#fi

rm -fr $TARGET/$ARCH.new
mkdir -p $TARGET
mkdir -p $TARGET/$ARCH
mkdir -p $TARGET/$ARCH.new

mv $TARGET/build_${ARCH}.log $TARGET/build_${ARCH}_old.log

echo "Build started: $(date)" >$TARGET/build_${ARCH}.log
./make_release.sh >>$TARGET/build_${ARCH}.log 2>&1
EXIT=$?
echo "Build completed: $(date)" >>$TARGET/build_${ARCH}.log

if [ $EXIT != 0 ] ; then
  cat $TARGET/build_${ARCH}.log | /usr/bin/mail -s "Daily Build $VERSION Failed" -c bugs@siedler25.org sf-team@siedler25.org

  EXIT=1
else
	REVISION=$(grep WINDOW_REVISION $BUILDDIR/build_version.h | cut -d ' ' -f 3 | cut -d \" -f 2)
	rm -f $TARGET/s25rttr_${VERSION}-*_${ARCH}.tar.bz2
	rm -f $TARGET/s25rttr_*-${REVISION}_${ARCH}.tar.bz2
	cp -v s25rttr_$VERSION.tar.bz2 /srv/buildfarm/uploads/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
	rm -f s25rttr_$VERSION.tar.bz2

	rm -f $TARGET/$ARCH.new/music.tar.bz2
	cp -v s25rttr_music.tar.bz2 $TARGET/$ARCH.new/music.tar.bz2 >> $TARGET/build_${ARCH}.log
	rm -f s25rttr_music.tar.bz2

	if [ -f $TARGET/$ARCH.new/binaries.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH.new -xf $TARGET/$ARCH.new/binaries.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi
	
	if [ -f $TARGET/$ARCH.new/drivers.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH.new -xf $TARGET/$ARCH.new/drivers.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH.new/libraries.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH.new -xf $TARGET/$ARCH.new/libraries.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH.new/RTTR.tar.bz2 ] ; then
		tar -C $TARGET/$ARCH.new -xf $TARGET/$ARCH.new/RTTR.tar.bz2 >> $TARGET/build_${ARCH}.log
	fi

	if [ -f $TARGET/$ARCH.new/music.tar.bz2 ] ; then
		if [ -d  $TARGET/$ARCH.new/share/s25rttr ] ; then
			tar -C $TARGET/$ARCH.new/share/s25rttr -xf $TARGET/$ARCH.new/music.tar.bz2 >> $TARGET/build_${ARCH}.log
		elif [ -d $TARGET/$ARCH.new/s25client.app/Contents/MacOS/share/s25rttr ] ; then
			tar -C $TARGET/$ARCH.new/s25client.app/Contents/MacOS/share/s25rttr -xf $TARGET/$ARCH.new/music.tar.bz2 >> $TARGET/build_${ARCH}.log
		else
			tar -C $TARGET/$ARCH.new -xf $TARGET/$ARCH.new/music.tar.bz2 >> $TARGET/build_${ARCH}.log
		fi
	fi

	rm -f $TARGET/$ARCH.new/*.tar.bz2

 	echo "${REVISION}" > $TARGET/.revision
	echo "${VERSION}" > $TARGET/.version
	echo "s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2" >> $TARGET/.files
	
	OPWD=$PWD
	cd $TARGET/$ARCH.new
	echo -n > /tmp/links.$$
	find -type l -exec bash -c 'echo "{} $(readlink {})" >> /tmp/links.$$ ; rm {}' \;
	md5deep -r -l . > /tmp/files.$$
	cd $OPWD

	find $TARGET/$ARCH.new -type f -exec bzip2 -v {} >> $TARGET/build_${ARCH}.log 2>&1 \;

	mv /tmp/files.$$ $TARGET/$ARCH.new/files
	mv /tmp/links.$$ $TARGET/$ARCH.new/links
	
	touch $TARGET/$ARCH.new/revision-$REVISION
	touch $TARGET/$ARCH.new/version-$VERSION

	# swap old & new ARCH-Tree
	rm -fr $TARGET/$ARCH.old
	mv $TARGET/$ARCH $TARGET/$ARCH.old
	mv $TARGET/$ARCH.new $TARGET/$ARCH
	
	#if [ -z "$NORS" ] ; then
		#gpg -u 6D09334C --armor --sign --detach-sig $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
		#OUTPUT=$(/srv/buildfarm/uploadlp.py s25rttr s25client nightly $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2)
		#echo $OUTPUT >> $TARGET/build_${ARCH}.log
		#if [ "$(echo $OUTPUT | head -n 1)" = "Success!" ] ; then
		#	echo $OUTPUT | tail -n 1 >> $TARGET/rapidshare.txt
		#	/srv/buildfarm/remoters.sh "http://launchpad.net/s25rttr/s25client/nightly/+download/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2" >> $TARGET/build_${ARCH}.log
		#fi
	#	/srv/buildfarm/uploadrs.sh $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
		#rm -f $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2.asc >> $TARGET/build_${ARCH}.log
	#	rm -f $TARGET/s25rttr_${VERSION}-${REVISION}_${ARCH}.tar.bz2 >> $TARGET/build_${ARCH}.log
	#fi
	
	EXIT=0
fi

unset VERSION

exit $EXIT
