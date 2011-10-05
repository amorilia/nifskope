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

* If one of NifSkope's windows disappears due to moving it to a location
  outside of the desktop view, close NifSkope and remove the NifSkope settings
  by deleting the registry key(s)

    HKEY_USERS\S-1-5-***\Software\NifTools\NifSkope-***

  Launch NifSkope again, and all windows will appear at their default
  position.

* If NifSkope does not start at all, check your firewall. NifSkope uses the
  network (locally, nothing is sent to the outside world) to communicate with
  itself.

Reporting bugs
--------------

http://sourceforge.net/tracker/?group_id=149157&atid=776343
 
Compiling NifSkope from source
------------------------------

http://niftools.sourceforge.net/wiki/NifSkope/Compile


mingw/msys NifSkope with OGRE support:
1.If you download and install:
http://get.qt.nokia.com/qt/source/qt-win-opensource-4.7.1-mingw.exe
http://netcologne.dl.sourceforge.net/project/ogre/ogre/1.7/OgreSDK_mingw_v1-7-2.exe
into "d:\qtmingw" and "d:\ogremingw" accordingly, then:
2.msys:export PATH="/d/qtmingw/bin:/d/qtmingw/lib:"$PATH
  cmd:set PATH=d:\qtmingw\bin;d:\qtmingw\lib;%PATH%
3.msys:qmake -spec win32-g++ "OGRE_INC=/d/ogremingw/include/OGRE" "BOOST_INC=/d/ogremingw/boost_1_44" "OGRE_LIB=/d/ogremingw/lib/release"
  cmd:qmake -spec win32-g++ "OGRE_INC=d:\\ogremingw\\include\\OGRE" "BOOST_INC=d:\\ogremingw\\boost_1_44" "OGRE_LIB=d:\\ogremingw\\lib\\release"
4.msys:make
  cmd:mingw32-make
  
ms visual c++ 2008 express NifSkope with OGRE support:
1.If you download and install
http://netcologne.dl.sourceforge.net/project/ogre/ogre/1.7/OgreSDK_vc9_v1-7-2.exe
http://get.qt.nokia.com/qt/source/qt-win-opensource-4.7.1-vs2008.exe
into "d:\qt" and "d:\ogre" accordingly, then:
2.cmd:set PATH=d:\qt\bin;d:\qt\lib;%PATH%
3.cmd:qmake -t vcapp "OGRE_INC=d:\\ogre\\include\\OGRE" "BOOST_INC=d:\\ogre\\boost_1_44" "OGRE_LIB=d:\\ogre\\lib\\release"
4.cmd:NifSkope.vcproj
5.Select "Release" and hit "Build"->"Build Solution"

to run NifSkope with OGRE support:
In the "release" folder you'll find NifSkope.exe. You'll need following dlls:
- from "d:\qtmingw" or "d:\qt" depending on the build type "mingw" or "vs 2008":
  QtCore4.dll
  QtGui4.dll
  QtNetwork4.dll
  QtOpenGL4.dll
  QtXml4.dll
- from "d:\ogremingw" or "d:\ogre" depending on the build type "mingw" or "vs 2008":
  OgreMain.dll
  Plugin_OctreeSceneManager.dll
  RenderSystem_Direct3D9.dll (if you'll be using this render system)
  RenderSystem_GL.dll (if you'll be using this render system)
  boost_1_44\lib\libboost_thread-mgw45-mt-1_44.dll ( for mingw build only )
NifSkope.exe built with mingw will fail to load with dlls from non-mingw sdk
and vice versa.
