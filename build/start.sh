#!/bin/sh

make
if [ $? != 0 ] ; then
	exit 1
fi

export LD_LIBRARY_PATH=libsiedler2/src
export EF_ALLOW_MALLOC_0=1
export EF_PROTECT_FREE=0
export EF_FREE_WIPES=1

BIN=src/s25client
CMD=
ARGS=

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
		-n | --nowait)
			NOWAIT=1
			;;
		debug)
			CMD=gdb
			;;
		valgrind)
			CMD=valgrind
			ARGS="$ARGS --leak-check=full --log-file=s25client"
			;;
		ddd)
			CMD=ddd
			;;
		mpatrol)
			CMD=mpatrol
			ARGS="$ARGS -C"
			;;
		*)
			echo "Unknown option: $ac_option"
			exit 1
			;;
	esac
	
	shift
done

case $CMD in
	gdb)
		if [ -n "$NOWAIT" ] ; then
			ARGS="$ARGS -x start.gdb"
		fi
		;;
esac

$CMD $ARGS $BIN

exit $?

