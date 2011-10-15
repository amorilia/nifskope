#!/bin/bash
set -e

if [ -z $1 ]
then
	echo "usage: "$0" classname"
	exit
fi

CNAME=$1
HP="__"$(echo $1 | awk '{print toupper($0)}')"_H__"

echo "#ifndef "$HP > $CNAME".h"
echo "#define "$HP >> $CNAME".h"
echo >> $CNAME".h"
echo "namespace NifSkope" >> $CNAME".h"
echo "{" >> $CNAME".h"
echo $'\t'"class "$CNAME >> $CNAME".h"
echo $'\t'"{" >> $CNAME".h"
echo $'\t'"};" >> $CNAME".h"
echo "}" >> $CNAME".h"
echo >> $CNAME".h"
echo "#endif /*"$HP"*/" >> $CNAME".h"

echo "#include \""$CNAME".h"\" > $CNAME".cpp"
echo >> $CNAME".cpp"
echo "namespace NifSkope" >> $CNAME".cpp"
echo "{" >> $CNAME".cpp"
echo "}" >> $CNAME".cpp"
