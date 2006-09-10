# nifxml_niflib.py
#
# This script generates C++ code for Niflib.
#
# --------------------------------------------------------------------------
# Command line options
#
# -p /path/to/niflib : specifies the path where niflib can be found 
#
# -b : enable bootstrap mode (generates templates)
# 
# -i : do NOT generate implmentation; place all code in defines.h
#
# -a : generate accessors for data in classes
#
# --------------------------------------------------------------------------
# ***** BEGIN LICENSE BLOCK *****
#
# Copyright (c) 2005, NIF File Format Library and Tools
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of the NIF File Format Library and Tools
#      project nor the names of its contributors may be used to endorse
#      or promote products derived from this software without specific
#      prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# ***** END LICENCE BLOCK *****
# --------------------------------------------------------------------------

from nifxml import *
from distutils.dir_util import mkpath
import os

#
# global data
#

ROOT_DIR = "."


prev = ""
for i in sys.argv:
    if prev == "-p":
        ROOT_DIR = i
    prev = i

#
# Sort Name Lists
#

block_names.sort()
compound_names.sort()
basic_names.sort()

#
# Generate Basic List Page
#

temp = Template()
temp.set_var( "title", "Basic Data Types" )

#List each Compound with Description

count = 0
basic_list = ""
for n in basic_names:
    x = basic_types[n]

    if count % 2 == 0:
        temp.set_var( "row-class", "reg0" )
    else:
        temp.set_var( "row-class", "reg1" )
            
    temp.set_var( "list-name", x.name )
    temp.set_var( "list-desc", x.description )

    basic_list += temp.parse( "templates/list_row.html" )

    count += 1


temp.set_var( "list", basic_list )

temp.set_var( "contents", temp.parse( "templates/list.html") )

f = file(ROOT_DIR + '/doc/basic_list.html', 'w')
f.write( temp.parse( "templates/main.html" ) )
f.close()
    

    
#
# Generate Basic Pages
#

count = 0
for n in basic_names:
    x = basic_types[n]

    temp = Template()
    temp.set_var( "title", x.name )
    temp.set_var( "name", x.name )
    temp.set_var( "description", x.description )

    #Create Found In list
    found_in = ""

    for b in block_names:
        for m in block_types[b].members:
            if m.ctype == n:
                found_in += "<li><a href=\"" + b + ".html\">" + b + "</a></li>\n"
                break

    for b in compound_names:
        for m in compound_types[b].members:
            if m.ctype == n:
                found_in += "<li><a href=\"" + b + ".html\">" + b + "</a></li>\n"
                break

    temp.set_var( "found-in", found_in );
    
    temp.set_var( "contents", temp.parse( "templates/basic.html") )

    f = file(ROOT_DIR + '/doc/' + x.cname + '.html', 'w')
    f.write( temp.parse( "templates/main.html" ) )
    f.close()


#
# Generate Compound List Page
#

temp = Template()
temp.set_var( "title", "Compound Data Types" )

#List each Compound with Description

count = 0
compound_list = ""
for n in compound_names:
    x = compound_types[n]

    if count % 2 == 0:
        temp.set_var( "row-class", "reg0" )
    else:
        temp.set_var( "row-class", "reg1" )
            
    temp.set_var( "list-name", x.name )
    temp.set_var( "list-desc", x.description )

    compound_list += temp.parse( "templates/list_row.html" )

    count += 1


temp.set_var( "list", compound_list )

temp.set_var( "contents", temp.parse( "templates/list.html") )

f = file(ROOT_DIR + '/doc/compound_list.html', 'w')
f.write( temp.parse( "templates/main.html" ) )
f.close()
    

    
#
# Generate Compound Pages
#

mkpath(os.path.join(ROOT_DIR, "obj"))
mkpath(os.path.join(ROOT_DIR, "gen"))

count = 0
for n in compound_names:
    x = compound_types[n]

    temp = Template()
    temp.set_var( "title", x.name )
    temp.set_var( "name", x.name )
    temp.set_var( "description", x.description )

    #Create Found In list
    found_in = ""

    for b in block_names:
        for m in block_types[b].members:
            if m.ctype == n:
                found_in += "<li><a href=\"" + b + ".html\">" + b + "</a></li>\n"
                break

    for b in compound_names:
        for m in compound_types[b].members:
            if m.ctype == n:
                found_in += "<li><a href=\"" + b + ".html\">" + b + "</a></li>\n"
                break

    temp.set_var( "found-in", found_in );

    #Create Attribute List
    attr_list = ""
    count = 0
    for a in x.members:
        temp.set_var( "attr-name", a.name )
        temp.set_var( "attr-type", a.ctype )
        temp.set_var( "attr-arg", a.arg )
        temp.set_var( "attr-arr1", a.arr1.lhs )
        temp.set_var( "attr-arr2", a.arr2.lhs )
        if a.cond.lhs and a.cond.op and a.cond.rhs:
            temp.set_var( "attr-cond", a.cond.lhs + " " + a.cond.op + " " + a.cond.rhs )
        else:
            temp.set_var( "attr-cond", "" )

        if count % 2 == 0:
            temp.set_var( "row-class", "reg0" )
        else:
            temp.set_var( "row-class", "reg1" )

        temp.set_var( "attr-desc", a.description )

        temp.set_var( "attr-from", a.orig_ver1 )
        temp.set_var( "attr-to", a.orig_ver2 )

        attr_list += temp.parse( "templates/attr_row.html" )

        count += 1

    temp.set_var( "attributes", attr_list )
    
    temp.set_var( "contents", temp.parse( "templates/compound.html") )

    f = file(ROOT_DIR + '/doc/' + x.cname + '.html', 'w')
    f.write( temp.parse( "templates/main.html" ) )
    f.close()
