==============
== NifSkope ==
==============
 
NifSkope is a tool for analyzing and editing NetImmerse files (.nif)

This is version 2.0.0 of NifSkope.

Known Issues and Workarounds
----------------------------

* On some graphic cards textures show up black or distorted. If this happens,
  uncheck "Shaders" in NifSkope's settings.

* Models with multiple texture maps may appear to be missing some textures,
  but they will become visible when some texture maps are disabled. This is
  due to the limited number of texture units in graphics hardware.

Reporting bugs
--------------

http://sourceforge.net/tracker/?group_id=149157&atid=776343
 
Compiling NifSkope from source
------------------------------

* Linux:
	cd nifskope2
	export OGRE_ROOT=wherever_it_is
	export QTDIR=wherever_it_is
	cp ../nifskope.png .
	cp -R ../nifskope2/resources .
	make

	export LD_LIBRARY_PATH=$OGRE_ROOT"/lib/OGRE"
	./nifskope
	hit "Load" and choose a file