#!/bin/bash
###############################################################################
## $Id$
###############################################################################

# Editable Variables
CMAKE_COMMAND=cmake

###############################################################################

if [ -z "$(type -p $CMAKE_COMMAND)" ] ; then
	echo "You have to install CMake"
	exit 1
fi

mecho()
{
	COLOR=$1
	shift
	$CMAKE_COMMAND -E cmake_echo_color --bold $COLOR "$*"
}

###############################################################################

PREFIX=/usr/local
BINDIR=
DATADIR=
as_cr_letters='abcdefghijklmnopqrstuvwxyz'
as_cr_LETTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
as_cr_Letters=$as_cr_letters$as_cr_LETTERS
as_cr_digits='0123456789'
as_cr_alnum=$as_cr_Letters$as_cr_digits

while test $# != 0 ; do
	case $1 in
	--*=*)
		ac_option=`expr "X$1" : 'X\([^=]*\)='`
		ac_optarg=`expr "X$1" : 'X[^=]*=\(.*\)'`
		ac_shift=:
	;;
	*)
		ac_option=$1
		ac_optarg=$2
		ac_shift=shift
	;;
	esac

	case $ac_option in
	-prefix | --prefix)
		$ac_shift
		PREFIX=$ac_optarg
	;;
        -iprefix | --iprefix)
		$ac_shift
		IPREFIX=$ac_optarg
	;;
	-bindir | --bindir)
		$ac_shift
		BINDIR=$ac_optarg
	;;
	-datadir | --datadir)
		$ac_shift
		DATADIR=$ac_optarg
	;;
	-arch | --arch)
		$ac_shift
		ARCH=$ac_optarg
	;;
  	-enable-* | --enable-*)
    	ac_feature=`expr "x$ac_option" : 'x-*enable-\([^=]*\)'`
    	# Reject names that are not valid shell variable names.
    	expr "x$ac_feature" : ".*[^-._$as_cr_alnum]" >/dev/null &&
      	{
      		echo "error: invalid feature name: $ac_feature" >&2
   			{ (exit 1); exit 1; };
   		}
    	ac_feature=`echo $ac_feature | sed 's/[-.]/_/g'`
    	if [ -z "$ac_optarg" ] ; then
    		ac_optarg="yes"
    	fi
    	eval enable_$ac_feature=\$ac_optarg
    ;;
    *)
		echo "Unknown option: $ac_option"
		exit 1
	;;
	esac

	shift
done

if [ -z "$IPREFIX" ] ; then
	IPREFIX=$PREFIX
fi

if [ -z "$BINDIR" ] ; then
	BINDIR=$IPREFIX/bin
fi

if [ -z "$DATADIR" ] ; then
	DATADIR=$IPREFIX/share/s25rttr
fi

###############################################################################

mecho --blue "Creating directories"
mkdir -vp $BINDIR || exit 1
mkdir -vp $DATADIR || exit 1
mkdir -vp $DATADIR/S2 || exit 1
mkdir -vp $DATADIR/RTTR || exit 1
mkdir -vp $DATADIR/driver/video || exit 1
mkdir -vp $DATADIR/driver/audio || exit 1
mkdir -vp $IPREFIX/lib || exit 1

mecho --blue "Installing binaries"
cp -v ../release/bin/rttr.sh $BINDIR || exit 1

mecho --blue "Installing RTTR directory"
if [ -d ../RTTR/.svn ] ; then
	svn --force --non-interactive export ../RTTR $DATADIR/RTTR || exit 1
elif [ -d ../RTTR/.bzr ] ; then
	bzr export ../RTTR $DATADIR/RTTR || exit 1
else
	cp -rv ../RTTR/* $DATADIR/RTTR || exit 1
fi
rm -f $DATADIR/RTTR/languages/*.po

mecho --blue "Installing language files"
cp -v ../RTTR/languages/*.mo $DATADIR/RTTR/languages/ || exit 1

mecho --blue "Installing S2 placeholder"
touch $DATADIR/S2/put\ your\ S2-Installation\ in\ here || exit 1

exit 0

###############################################################################
