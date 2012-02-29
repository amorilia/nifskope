#!/bin/bash

# NifSkope 2 helper script
# Purpose: Refactor->Rename
# Note: use with care - regular expressions! - \".*\" doesn't necessary mean
#       "a string"...

function usage {
	echo "usage: "$0" old new"
}

if [ -z "$1" ]
then
	usage
	exit
fi

if [ -z "$2" ]
then
	usage
	exit
fi

TPATH="src include"

for FOLDER in $TPATH
do
	SRC=$(find $FOLDER -type f -printf %P$'\n' | grep -i -e '\.cpp$' -e '\.h$' -e '\.c$' | grep -v "ns_base.h")
	echo "Renaming... in \""$FOLDER"\""
	for FF in $SRC
	do
		FF=$FOLDER"/"$FF

		IFS=$'\n'
		CHA=$1
		RES=$2
		cat $FF | grep -o $CHA &> /dev/null
		if [ $? -eq 0 ]
		then	
			TMPF=$FF".tmp"
			if [ -e "$TMPF" ]
			then
				echo " File exists. Can't change \""$FF"\""
			else
				echo " Updating "$FF", replacing \""$CHA"\""" with ""\""$RES"\""
				cat $FF | sed s/$CHA/$RES/g > $TMPF
				rm -f $FF
				mv $TMPF $FF
			fi
		fi

	done
	echo "done."
done