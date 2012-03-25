#!/bin/bash

# NifSkope 2 helper script
# Purpose: create headers dependency graph in ready for ">> Makefile" format
# Prerequisites: grep, awk, tr, sed

echo "# Auto-generated header dependency list"
echo "# includes config.h generation when needed"
TPATH="src include"
for FOLDER in $TPATH
do
	# cpp c <-> header
	for FF in $(find $FOLDER -type f -printf %P$'\n' |\
		grep -i -e '\.cpp$' -e '\.c$')
	do
		FNAME=$FOLDER"/"$FF
		if [ -e $FNAME ]
		then
			OBJ=$(echo $FNAME | sed s/'\.c..$'/'\.o'/)
			echo -n $OBJ": "
			AFS=$IFS
			IFS=$'\n'
			for HDR in $(grep "#include.*\.h.*" $FNAME |\
					awk -F' ' '{print $2}' |\
					tr -d '\"<>')
			do
				echo -n " "$(find . -type f -name "*.h" -printf %P$'\n' | grep $HDR)
			done
			echo ""
			IFS=$AFS
			#echo $OBJ": "$( grep "#include.*\.h.*" $FNAME |\
			#		awk -F' ' '{print $2}' |\
			#		tr -d '\"<>' |\
			#		tr '\n' ' ')
		fi
	done
	# header <-> header
	for FF in $(find $FOLDER -type f -printf %P$'\n' | grep -i '\.h$')
	do
		FNAME=$FOLDER"/"$FF
		FILENAME=$(echo $FNAME | sed s/'.*\/'//)
		#FILEPATH=$(echo $FNAME | sed s/$FILENAME//)
		if [ -e $FNAME ]
		then
			echo $FNAME": "$(grep "#include.*\.h.*" $FNAME |\
				awk -F' ' '{print $2}' |\
				tr -d '\"<>' |\
				tr '\n' ' ') | grep -v "\: \$"
		fi
	done
	# headers list
	for FF in $(find $FOLDER -type f -printf %P$'\n' | grep -i '\.h$')
	do
		FNAME=$FOLDER"/"$FF
		FILENAME=$(echo $FNAME | sed s/'.*\/'//)
		#FILEPATH=$(echo $FNAME | sed s/$FILENAME//)
		if [ -e $FNAME ]
		then
			echo $FILENAME": "$FNAME
		fi
	done
done
# "unknown" header(s)
echo "%.h:"
echo $'\t'
