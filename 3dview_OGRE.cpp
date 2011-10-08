/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2010, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

/*
  many 10x to "Sussch" @ "ogre3d" forums: "[SOLVED] Qt + Ogre on X"
  http://www.ogre3d.org/forums/viewtopic.php?f=2&t=62805

*/

#include "3dview_OGRE.h"
#define OVERRIDE

#ifdef NIFSKOPE_X
#include <X11/Xlib.h>
#endif /* NIFSKOPE_X */

NifSkopeOgre3D *NifSkopeOgre3D::create()
{
	NifSkopeOgre3D *tmp = new NifSkopeOgre3D();
	/*try {
            tmp->go();
	} catch( Ogre::Exception& e ) {
		//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		//MessageBox( NULL, e.getFullDescription().c_str(), "An exception hasoccured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		//#else
		std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
		//#endif
	}*/
	return tmp;
}

NifSkopeOgre3D::NifSkopeOgre3D(void)
	: NifSkopeQt3D (NULL)
	,ready(0), mRoot(0), mCam(0), mScn(0), mWin(0)
{
}

NifSkopeOgre3D::~NifSkopeOgre3D(void)
{
	//delete mRoot;
}

bool NifSkopeOgre3D::go()
{
	mRoot = new Ogre::Root("", "", "NifSkopeOgre3D.log");
	// A list of required plugins - it will not run without them
	Ogre::StringVector required_plugins;
	required_plugins.push_back("GL RenderSystem");
	required_plugins.push_back("Octree & Terrain Scene Manager");
	// List of plugins to load
	Ogre::StringVector plugins_toLoad;
#ifdef NIFSKOPE_OGRE_GL
	plugins_toLoad.push_back("RenderSystem_GL");
#endif /* NIFSKOPE_OGRE_GL */
#ifdef NIFSKOPE_OGRE_DX9
	plugins_toLoad.push_back("RenderSystem_Direct3D9");
#endif /* NIFSKOPE_OGRE_DX */
	plugins_toLoad.push_back("Plugin_OctreeSceneManager");
	// Load the OpenGL RenderSystem and the Octree SceneManager plugins
	for (Ogre::StringVector::iterator j = plugins_toLoad.begin(); j != plugins_toLoad.end(); j++)
	{
		#ifdef _DEBUG
		mRoot->loadPlugin(*j + Ogre::String("_d"));
		#else
		mRoot->loadPlugin(*j);
		#endif;
	}
	// Check if the required plugins are installed and ready for use
	// If not: exit the application
	Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
	for (Ogre::StringVector::iterator j = required_plugins.begin(); j != required_plugins.end(); j++) {
		bool found = false;
		// try to find the required plugin in the current installed plugins
		for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++) {
			if ((*k)->getName() == *j) {
				found = true;
				break;
			}
		}
		if (!found) {  // return false because a required plugin is not available
			return false;
		}
	}
	//-------------------------------------------------------------------------------------
	// setup resources
	// Only add the minimally required resource locations to load up the Ogre head mesh
	// TODO:remove this after the *.nif mesh + *.dds loader is ready
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/mnt/hd/inst-src/rain/ogre_src_v1-7-2/Samples/Media/materials/programs", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/mnt/hd/inst-src/rain/ogre_src_v1-7-2/Samples/Media/materials/scripts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/mnt/hd/inst-src/rain/ogre_src_v1-7-2/Samples/Media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/mnt/hd/inst-src/rain/ogre_src_v1-7-2/Samples/Media/models", "FileSystem", "General");
	/*Ogre::ResourceGroupManager::getSingleton().addResourceLocation("D:/projects/nifskope/debug/media/materials/programs", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("D:/projects/nifskope/debug/media/materials/scripts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("D:/projects/nifskope/debug/media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("D:/projects/nifskope/debug/media/models", "FileSystem", "General");*/
	//-------------------------------------------------------------------------------------
	// configure
	// Grab the OpenGL RenderSystem, or exit
#ifdef NIFSKOPE_OGRE_GL
	//TODO: choice dialog, settings load ?
	//
	Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#endif /* NIFSKOPE_OGRE_GL */
#ifdef NIFSKOPE_OGRE_DX9
	//Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
#endif /* NIFSKOPE_OGRE_DX9 */
#ifdef NIFSKOPE_OGRE_GL
	//TODO: choice dialog, settings load ?
	if(!(rs->getName() == "OpenGL Rendering Subsystem")) {
#endif /* NIFSKOPE_OGRE_GL */
#ifdef NIFSKOPE_OGRE_DX9
	//if(!(rs->getName() == "Direct3D9 Rendering Subsystem")) {
#endif /* NIFSKOPE_OGRE_DX9 */
		return false; //No RenderSystem found
	}
	// configure our RenderSystem
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("VSync", "Yes");
	rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit");// TODO: check 800 x 600
	mRoot->setRenderSystem(rs);
	Ogre::NameValuePairList misc;
	//misc["parentWindowHandle"] = Ogre::StringConverter::toString((long)window_provider.winId());

#ifdef NIFSKOPE_WIN
	//works under windows with both GL and DX
	misc["externalWindowHandle"] = Ogre::StringConverter::toString((long)winId());
	// TODO: works but the rendering is not fit to the window
	//misc["parentWindowHandle"] = Ogre::StringConverter::toString((long)winId());
#else
#ifdef NIFSKOPE_X
	//QX11Info info = x11Info(); #ifdef Q_WS_X11 #ifdef _WIN32
	Display* dpy = XOpenDisplay (getenv ("DISPLAY"));
    int screen = DefaultScreen (dpy);
	/*misc["externalWindowHandle"] = fails
		Ogre::StringConverter::toString((unsigned long)(info.display())) + ":" +
		Ogre::StringConverter::toString((unsigned int)(info.screen())) + ":" +
		Ogre::StringConverter::toString((unsigned long)winId());*/
	QWidget *q_parent = dynamic_cast <QWidget *> ( parent() );
	if (q_parent) {
	misc["parentWindowHandle"] = 
		/*Ogre::StringConverter::toString((unsigned long)(info.display())) + ":" +
		Ogre::StringConverter::toString((unsigned int)(info.screen())) + ":" +
		Ogre::StringConverter::toString((unsigned long)q_parent->winId());*/
		Ogre::StringConverter::toString((unsigned long)(dpy)) + ":" +
		Ogre::StringConverter::toString((unsigned int)(screen)) + ":" +
		Ogre::StringConverter::toString((unsigned long)q_parent->winId());
	} else return false;
#else
#error No supported windowing system found
#endif /* NIFSKOPE_X */
#endif /* NIFSKOPE_WIN */

	misc["vsync"] = "true";// it ignores the above when "createRenderWindow"
	//mWin = mRoot->createRenderWindow("mu", 800, 600, false);
	//  ogre created window 
	mWin = mRoot->initialise(false, "Nifskope 2");
	mWin = mRoot->createRenderWindow("Nifskope 2", width(), height(), false, &misc);

    mWin->setActive( true );
    mWin->resize(width(), height());
    mWin->setVisible( true );

#ifdef NIFSKOPE_X
    // Get the ID of Ogre render window
    WId window_id;
    mWin->getCustomAttribute( "WINDOW", &window_id );
    // Take over the ogre created window.
    QWidget::create( window_id );
    resizeEvent( NULL );
    setAttribute( Qt::WA_PaintOnScreen );
    setAttribute( Qt::WA_OpaquePaintEvent );
#endif /* NIFSKOPE_X */

	//-------------------------------------------------------------------------------------
	// choose scenemanager
	// Get the SceneManager, in this case the OctreeSceneManager
	mScn = mRoot->createSceneManager("OctreeSceneManager", "DefaultSceneManager");
	//-------------------------------------------------------------------------------------
	// create camera
	// Create the camera
	mCam = mScn->createCamera("PlayerCam");
	// Position it at 500 in Z direction
	mCam->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCam->lookAt(Ogre::Vector3(0,0,-300));
	mCam->setNearClipDistance(5);
	//-------------------------------------------------------------------------------------
	// create viewports
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWin->addViewport(mCam);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	// Alter the camera aspect ratio to match the viewport
	mCam->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	//-------------------------------------------------------------------------------------
	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//-------------------------------------------------------------------------------------
	// Create any resource listeners (for loading screens)
	//createResourceListener();
	//-------------------------------------------------------------------------------------
	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//-------------------------------------------------------------------------------------
	// Create the scene
	Ogre::Entity* ogreHead = mScn->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode = mScn->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(ogreHead);
	// Set ambient light
	mScn->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	// Create a light
	Ogre::Light* l = mScn->createLight("MainLight");
	l->setPosition(20,80,50);
	ready = 1;

	/*
	//-------------------------------------------------------------------------------------
	//mRoot->startRendering();
	while(true) {
		// Pump window messages for nice behaviour
		Ogre::WindowEventUtilities::messagePump();
		// Render a frame
		mRoot->renderOneFrame();
		if(mWin->isClosed())	{
			return false;
		}
	}
	// We should never be able to reach this corner
	// but return true to calm down our compiler
	return true;
	*/
	return ready;
}// go

OVERRIDE void NifSkopeOgre3D::resizeEvent(QResizeEvent *p)// overrides
{
	if (!ready)
		return;
	//mWin->resize(width(), height());
	mWin->windowMovedOrResized();
	//mCam->setAspectRatio(1);
}

OVERRIDE void NifSkopeOgre3D::paintEvent(QPaintEvent *p)
{
	if (!ready)
		return;
	// Pump window messages for nice behaviour
	Ogre::WindowEventUtilities::messagePump();
	// Render a frame
	mRoot->renderOneFrame();
	//if(mWin->isClosed())	{
	//	return false;
	//}
}
