#!/bin/sh

## This script is meant to run nifskope from the command line

ARCH=$(uname -m)
case $ARCH in
	x86_64 | s390x | sparc64)
		LIB_DIR="/usr/lib64"
		;;
	* )
		LIB_DIR="/usr/lib"
		;;
esac

LD_LIBRARY_PATH="$LIB_DIR/OGRE" "$LIB_DIR/nifskope" "$@"

