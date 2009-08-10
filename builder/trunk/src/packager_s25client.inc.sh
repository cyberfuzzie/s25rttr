## packager_s25client.sh

## inside $ARCH

sudo rm -rf ${NAME}
mkdir -vp ${NAME}
if ! cd ${NAME} ; then
	return 1
fi

echo $PWD

mkdir -vp bin
mkdir -vp share/s25rttr
mkdir -vp share/s25rttr/S2
mkdir -vp share/s25rttr/driver/video
mkdir -vp share/s25rttr/driver/audio

rm -rf share/s25rttr/RTTR
svn --force export ${BASEDIR}/${CLIENT}/RTTR share/s25rttr/RTTR

cp -rv ${BASEDIR}/${CLIENT}/RTTR/languages/*.mo share/s25rttr/RTTR/languages/

rm -vf share/s25rttr/RTTR/languages/*.po
rm -vf share/s25rttr/RTTR/REPLAYS/*.rpl
rm -vf share/s25rttr/RTTR/sound.lst
rm -vf share/s25rttr/RTTR/settings.bin
rm -vf share/s25rttr/RTTR/MUSIC/SNG/SNG_*.OGG
rm -vf *~ *.bak

rm -vrf share/s25rttr/S2
mkdir -vp share/s25rttr/S2
touch share/s25rttr/S2/put\ your\ S2-Installation\ in\ here

if [ -f ${BASE}/packager_${CLIENT}_${ARCH}.inc.sh ] ; then
	. ${BASE}/packager_${CLIENT}_${ARCH}.inc.sh
fi

## return 0
