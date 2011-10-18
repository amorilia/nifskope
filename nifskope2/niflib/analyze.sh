#!/bin/bash
# prints XML tags and their attributes

T="nif.xml"
TAGS=$(cat $T | grep '<.* ' | awk '{print $1}' | sort -u | grep -v -e '<!' -e '?' | sed s/'<'//)
IFS=$'\n'
for TAG in $TAGS
do
	echo $TAG":"
	cat $T | grep '< *'$TAG | awk '{ split($0, a); for (i = 1; i <= length(a); i++) print a[i]}' | grep '="' | sed s/'=.*'// | grep -v "^$" | sed s/'^\(.*\)'/' - \1'/ | sort -u
done
