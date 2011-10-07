#!/bin/bash
set -e

IFS=$'\n'
for ff in $(find . -type f -printf %P$'\n' | grep -v "^\." | grep -i -e "\.cpp$" -e "\.h$" -e "\.c$" )
do
	echo $ff
	cat $ff | tr -d '\r' > $ff".UNIQUE"
	rm -f $ff
	mv $ff".UNIQUE" $ff
done
