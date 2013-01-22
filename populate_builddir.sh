#!/bin/bash

# editable variables
CMAKE_COMMAND=cmake

# function definitions
mecho()
{
	COLOR=$1
	shift
	$CMAKE_COMMAND -E cmake_echo_color --bold $COLOR "$*"
}

# first parameter is source directory
SRCDIR=$1
if [ -z "${SRCDIR}" ] ; then
	mecho --red "Error: no source directory given"
	exit 1
fi

# check if build directory is subdirectory of source
case ${PWD} in
	${SRCDIR}*)
		mecho --blue "Creating relative symlinks"
		RELBUILD=${PWD:$[${#SRCDIR}+1]}
		OIFS=${IFS}
		IFS='/'
		SRCDIR=
		for frag in ${RELBUILD} ; do
			if [ -z ${SRCDIR} ] ; then
				SRCDIR=".."
			else
				SRCDIR="${SRCDIR}/.."
			fi
		done
		IFS=${OIFS}
		;;
	*)
		mecho --blue "Creating absolute symlinks"
		;;
esac

for dir in bin share s25rttr ; do
	if [ ! -e ${dir} ] ; then
		mecho --blue "Creating symlink ${dir}"
		ln -vs . ${dir}
	fi
done

if [ ! -e RTTR ] ; then
	if [ -e "${SRCDIR}/RTTR" ] ; then
		mecho --blue "Creating symlink RTTR"
		ln -vs "${SRCDIR}/RTTR" RTTR
	else
		mecho --red "Error: Directory RTTR is missing"
		exit 1
	fi
fi

if [ ! -e S2 ] ; then
	if [ -e "${SRCDIR}/S2" ] ; then
		mecho --blue "Creating symlink S2"
		ln -vs "${SRCDIR}/S2" S2
	else
		mecho --yellow "Warning: Directory S2 is missing"
		mecho --yellow "Direct debugging from this directory will not work"
	fi
fi

exit 0

