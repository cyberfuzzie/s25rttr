#!/bin/sh
###############################################################################
## $Id: cmake.sh 2735 2007-09-08 14:22:04Z FloSoft $
###############################################################################


# Editable Variables
CMAKE_COMMAND=cmake

###############################################################################


if [ -z "$(type -p $CMAKE_COMMAND)" ] ; then
	echo "You have to install CMake"
	exit 1;
fi

mecho()
{
	COLOR=$1
	shift
	$CMAKE_COMMAND -E cmake_echo_color --bold $COLOR "$*"
}

if [ ! -e bin ] ; then
	mecho --blue "Creating symlink «bin» ..."
	ln -s . bin
fi

if [ ! -e RTTR ] ; then
	if [ -e ../RTTR ] ; then
		mecho --blue "Creating symlink «RTTR» ..."
		ln -s ../RTTR RTTR
	else
		mecho --red "Directory «RTTR» missing!"
		exit 1
	fi
fi

if [ ! -e share ] ; then
	mecho --blue "Creating symlink «share» ..."
	ln -s . share
fi

if [ ! -e s25rttr ] ; then
	mecho --blue "Creating symlink «s25rttr» ..."
	ln -s . s25rttr
fi

if [ ! -e S2 ] ; then
	if [ -e ../S2 ] ; then
		mecho --blue "Creating symlink «S2» ..."
	ln -s ../S2 S2
	else
		mecho --red "Directory «S2» missing!"
		exit 1
	fi
fi

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
	-bindir | --bindir)
		$ac_shift
		BINDIR=$ac_optarg
	;;
	-datadir | --datadir)
		$ac_shift
		DATADIR=$ac_optarg
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

echo "Setting Prefix to \"$PREFIX\""
PARAMS="-DPREFIX=$PREFIX"

if [ ! -z "$BINDIR" ] ; then
	echo "Setting Binary Dir to \"$BINDIR\""
	PARAMS="$PARAMS -DBINDIR=$BINDIR"
fi

if [ ! -z "$DATADIR" ] ; then
	echo "Setting Data Dir to \"$DATADIR\""
	PARAMS="$PARAMS -DDATADIR=$DATADIR"
fi

case "$enable_debug" in
yes|YES|Yes)
	mecho --magenta "Activating debug build"
	PARAMS="$PARAMS -DCMAKE_BUILD_TYPE=Debug"
;;
*)
	mecho --magenta "Activating release build"
;;
esac

mecho --blue "Running \"cmake $PARAMS\""
$CMAKE_COMMAND $PARAMS ..

if [ $? != 0 ] ; then
	mecho --red "An error occured - please check above!"
	exit 1
fi

mecho --blue "Now type \"make\" to build project"

exit 0
