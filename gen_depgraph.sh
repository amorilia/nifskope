#!/bin/bash

# NifSkope 2 helper script
# Purpose: create headers dependency graph in ready for ">> Makefile" format
# Prerequisites: makedepend, find

TPATH="src"
for FOLDER in $TPATH
do
	makedepend -Iinclude -Isrc -Y $(find $FOLDER -type f \( -iname "*.cpp" -o -iname "*.c" \) -printf %p$'\n')
done
