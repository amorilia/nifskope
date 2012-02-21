#!/bin/bash

# NifSkope 2 helper script
# Purpose: verify code convention and metrics

function verify_includes {
	grep '#include.*\.\./' $1
	if [ $? -eq 0 ]
	then
		echo
		echo "Failed: "$(echo $FF | sed s/'\(^.*\)\:'/'\1'/)
		exit 1
	fi
	# TODO: make sure "^#include "
	# TODO: make sure Qt includes are: "^#include <Q.*>" , no ".h"
}

# TODO: chk for missing license

TPATH="src include test"
for FOLDER in $TPATH
do
	SRC=$(find $FOLDER -type f -printf %P$'\n' | grep -i -e '\.cpp$' -e '\.h$' -e '\.c$')
	echo -n "Verifying for relative paths in include directives..."
	for FF in $SRC
	do
		verify_includes $FOLDER"/"$FF
	done
	echo -e " done."
done
