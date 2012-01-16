# ***** BEGIN LICENSE BLOCK *****
# 
# BSD License
# 
# Copyright (c) 2005-2012, NIF File Format Library and Tools
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the NIF File Format Library and Tools project may not be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# ***** END LICENCE BLOCK *****

#!/bin/bash
#set -e
#set -x

#
# Replace the last four digits in yyyy-yyyy expression in a specific string
# in all files in current and all sub-folders where the specific string is
# found.
#

function usage {
	echo "usage: "$0" new_year"
}

if [ -z $1 ]
then
	usage
	exit
fi

IFS=$'\n'
CHA="\(Copyright (c) 2005-\)[0-9]\{4\}\(, NIF File Format Library and Tools\)"
RES="\1$1\2"
for ff in $(find . -type f -printf %P$'\n')
do
	GCHA=$(echo $CHA | sed s/'\\[()]'/''/g)
	cat $ff | grep -o $GCHA | grep -v $1 &> /dev/null
	if [ $? -eq 0 ]
	then
		TMPF=$ff".tmp"
		if [ -e "$TMPF" ]
		then
			echo "File exists. Can't change \""$ff"\""
		else
			echo "Updating "$ff
			cat $ff | sed s/$CHA/$RES/ > $TMPF
			rm -f $ff
			mv $TMPF $ff
		fi
	fi
done
