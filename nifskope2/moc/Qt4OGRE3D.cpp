/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2011, NIF File Format Library and Tools
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
*	WIP
*
*	"OGRE" 3D render system for "NifSkope":
*	http://www.ogre3d.org
*
*	help: http://www.ogre3d.org/forums/
*	TFM :): http://www.ogre3d.org/tikiwiki/
*
*	My thanks to all the helpful people at the forums and to the people who
*	created	"QtOgre": http://www.ogre3d.org/tikiwiki/tiki-index.php?page=QtOgre
*	My thanks to the people behind OGRE - you rock!
*/

#include "Qt4OGRE3D.h"
#include "nifskope.h"

#ifdef NIFSKOPE_X
#include <X11/Xlib.h>
#endif /* NIFSKOPE_X */

// "NifLib"
#include "Compiler.h"

// OGRE
#include <OgreSceneNode.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreMath.h>
#include <OgreCommon.h>

#include <sys/time.h>

namespace NifSkope
{

/* Init the pseudo-random generator */
void
randomize(void)
{
	srand ((unsigned)time(NULL));
}

/* Returns a random number [0;N-1] 
   http://www.c-faq.com/lib/randrange.html */
int
rnd(int N)
{
	return (int)(rand () / (RAND_MAX / N + 1));
}

	class NifLoaded: public ICommand // event
	{
		Qt4OGRE3D *obj;
		NifLoaded()
		{
		}
		// TODO: the others ...
	public:
		NifLoaded(Qt4OGRE3D *owner)
		{
			obj = owner;
		}
		virtual void Exec(IEvent *sender)// handler
		{
			obj->LoadNif (sender);
		}
	};

	Qt4OGRE3D::Qt4OGRE3D(void)
		: Qt43D (NULL)
		,handleNifLoaded(0), ready(0), mRoot(0), mCam(0), mScn(0)
		,mWin(0), progress(0)
	{
	}

	inline void Wait(int seconds)
	{
		// TODO: not quite precise
		QTime stopTime = QTime::currentTime ().addSecs (seconds);
		while (QTime::currentTime () < stopTime)
			QCoreApplication::processEvents (QEventLoop::AllEvents, 100);
	}

	Qt4OGRE3D::~Qt4OGRE3D(void)
	{
		ready = false;
		if (mRoot) {
			// If you remove "detachRenderTarget" - one "X" error.
			// This causes a memory leak - 23 bytes, part of them are win title
			// TODO: check that out
			mRoot->detachRenderTarget (mWin);
			// If you remove "destroy" - two "X" errors
			this->destroy ();
			mRoot->shutdown ();
			Wait (2);// wait, because OGRE "won't", and "X" will "complain"
      		delete mRoot;
		}
		// Release NS event handlers
		if (handleNifLoaded)
			delete handleNifLoaded;
		NSINFO("~Qt4OGRE3D ()")
	}

	bool
	Qt4OGRE3D::go()
	{
		randomize ();
		ar = 0.05; br = 0.03; cr = 0.08;
		// Attach to events from "NifSkope"
		handleNifLoaded = new NifLoaded (this);
		App->File.OnLoad.Subscribe (handleNifLoaded);

		Ogre::String sEmpty  ("");
		Ogre::String sLog    ("NifSkopeOgre3D.log");
		Ogre::String sRSGL   ("RenderSystem_GL");
		Ogre::String sRSGLn  ("OpenGL Rendering Subsystem");
		Ogre::String sRSDX9  ("RenderSystem_Direct3D9");
		Ogre::String sRSDX9n ("Direct3D9 Rendering Subsystem");
		Ogre::String sScnMngr("Plugin_OctreeSceneManager");
		Ogre::String sOptnFS ("Full Screen");
		Ogre::String sOptnVS ("VSync");
		Ogre::String sOptnVM ("Video Mode");
		Ogre::String sNo     ("No");
		Ogre::String sYes    ("Yes");
		Ogre::String sDefVM  ("800 x 600 @ 32-bit");
		Ogre::String sEWH    ("externalWindowHandle");
		Ogre::String sPWH    ("parentWindowHandle");
		Ogre::String sDP     (":");
		Ogre::String sVsync  ("vsync");
		Ogre::String sTrue   ("true");

		// Only when "detachRenderTarget" is called for that window:
		//  - set a name and there is a memory leak;
		//  - set none and there is no memory leak;
		// Wild guess: "detachRenderTarget" "forgets" to release some structure.
		// TODO: try to find out, meanwhile stay "untitled"
		Ogre::String sWTitle ("");

		const char *sDISPLAY = "DISPLAY";

		// Setup OGRE
		mRoot = new Ogre::Root (sEmpty, sEmpty, sLog);
#ifdef _DEBUG
		Ogre::String ext ("_d");
#else
		Ogre::String ext ("");
#endif
#ifdef NIFSKOPE_OGRE_GL
		mRoot->loadPlugin (sRSGL + ext);
#endif /* NIFSKOPE_OGRE_GL */
#ifdef NIFSKOPE_OGRE_DX9
		mRoot->loadPlugin (sRSDX9 + ext);
#endif /* NIFSKOPE_OGRE_DX9 */
		mRoot->loadPlugin (sScnMngr + ext);
		//	Select OGRE render system
		//	TODO: choice dialog, settings load, etc.
		Ogre::RenderSystem* rs = NULL;
#ifdef NIFSKOPE_OGRE_GL
		rs = mRoot->getRenderSystemByName (sRSGLn);
		if (!rs || rs->getName () != sRSGLn)
			return false; // failed
#endif /* NIFSKOPE_OGRE_GL */
#ifdef NIFSKOPE_OGRE_DX9
		//rs = mRoot->getRenderSystemByName(sRSDX9n);
		if (!rs || rs->getName () != sRSDX9n)
			return false; // failed
#endif /* NIFSKOPE_OGRE_DX9 */
		rs->setConfigOption (sOptnFS, sNo);
		rs->setConfigOption (sOptnVS, sYes);
		rs->setConfigOption (sOptnVM, sDefVM);
		//	TODO: AA, AF, etc. - settings
		mRoot->setRenderSystem (rs);
		//	Setup Qt4 Widget and OGRE working environment*
		Ogre::NameValuePairList misc;
#ifdef NIFSKOPE_WIN
		misc[sEWH] = Ogre::StringConverter::toString ((long)winId ());
#else
#ifdef NIFSKOPE_X
		Display* dpy = XOpenDisplay (getenv (sDISPLAY));
    	int screen = DefaultScreen (dpy);
		QWidget *q_parent = dynamic_cast<QWidget *>(parent ());
		if (q_parent) {
			misc[sPWH] = 
				Ogre::StringConverter::toString ((unsigned long)(dpy)) + sDP +
				Ogre::StringConverter::toString ((unsigned int)(screen)) + sDP +
				Ogre::StringConverter::toString ((unsigned long)q_parent->winId ());
		} else
			return false;
		XCloseDisplay (dpy);
#else
#error No supported windowing system found
#endif /* NIFSKOPE_X */
#endif /* NIFSKOPE_WIN */
		misc[sVsync] = sTrue;// it ignores the above when "createRenderWindow"
		mWin = mRoot->initialise (false, sWTitle);
		mWin = mRoot->createRenderWindow (
			sWTitle, width (), height (), false, &misc);
    	mWin->setActive (true);
    	mWin->resize (width (), height ());
    	mWin->setVisible (true);
#ifdef NIFSKOPE_X
    	//	Get the ID of "OGRE" render window
    	WId window_id;
    	mWin->getCustomAttribute (Ogre::String("WINDOW"), &window_id);
    	//	Take over the "OGRE" created window.
    	QWidget::create (window_id); // Qt specific way*
		setAttribute (Qt::WA_PaintOnScreen, true);
		setAttribute (Qt::WA_NoSystemBackground, true);
		setAttribute (Qt::WA_NoBackground, true);
#endif /* NIFSKOPE_X */
		//	Setup the scene*
		mScn = mRoot->createSceneManager ("OctreeSceneManager", "DefaultSceneManager");
		mCam = mScn->createCamera ("camMain");
		mCam->setPosition (Ogre::Vector3 (0, 0, 0));
		mCam->lookAt (Ogre::Vector3 (0, 0, 0));// look at the center*
		mCam->setNearClipDistance (5);// near clipping plane
		mVp = mWin->addViewport (mCam);
		mVp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));
		mCam->setAspectRatio (
			(Ogre::Real)mVp->getActualWidth ()/mVp->getActualHeight ());
		Ogre::TextureManager::getSingleton ().setDefaultNumMipmaps (5);
		Ogre::ResourceGroupManager::getSingleton ().initialiseAllResourceGroups ();
		mScn->setAmbientLight (Ogre::ColourValue (0.1, 0.1, 0.1));// slight global
		Ogre::Light* l = mScn->createLight ("lightMain");
		l->setPosition (100, 100, 100);
		/*delete mRoot;
		mRoot = NULL;
		return true;*/
		QTimer *timer = new QTimer(this);
		timer->setSingleShot (false);
 		connect(timer, SIGNAL(timeout()), this, SLOT(Render()));
		ready = 1;
		timer->start (1000/25);

		// debug
		Ogre::MaterialPtr matDbg =
			Ogre::MaterialManager::getSingleton().create("debugMat",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); 
		matDbg->setReceiveShadows(false); 
		matDbg->getTechnique(0)->setLightingEnabled(false); 
		matDbg->getTechnique(0)->getPass(0)->setCullingMode (Ogre::CULL_NONE);
		matDbg->getTechnique(0)->getPass(0)->setPolygonMode (Ogre::PM_WIREFRAME);

		Ogre::ManualObject* pG = mScn->createManualObject ("pG");
		pG->begin ("debugMat", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
		int q = 5;
		Ogre::ColourValue pGc (0.8, 0.8, 0.8, 0);
		pG->position (-q, -q, -q); pG->colour (pGc);
		pG->position (-q, q, -q);
		pG->position (q, -q, -q);
		pG->position (q, q, -q);
		pG->position (q, -q, q);
		pG->position (q, q, q);
		pG->position (-q, -q, q);
		pG->position (-q, q, q);
		pG->position (-q, -q, -q);
		pG->position (-q, q, -q);
		pG->end ();
		progress = mScn->getRootSceneNode ()->createChildSceneNode ("in progress");
		progress->attachObject (pG);

		q = 3;
		Ogre::ManualObject* camFocus = mScn->createManualObject ("camFocus");
		// colour() works with no lighting material only
		// a wild guess - it uses color material when no lighting
		camFocus->begin ("debugMat", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
		//int q = 5;
		Ogre::ColourValue c1 (0, 0.8, 0, 0);
		camFocus->position (-q, -q, -q); camFocus->colour (c1);
		camFocus->position (-q, q, -q);
		camFocus->position (q, -q, -q);
		camFocus->position (q, q, -q);
		camFocus->position (q, -q, q);
		camFocus->position (q, q, q);
		camFocus->position (-q, -q, q);
		camFocus->position (-q, q, q);
		camFocus->position (-q, -q, -q);
		camFocus->position (-q, q, -q);
		camFocus->end ();

		Ogre::ManualObject* camOfs = mScn->createManualObject ("camOfs");
		// colour() works with no lighting material only
		// a wild guess - it uses color material when no lighting
		Ogre::ColourValue c2 (0.8, 0, 0, 0);
		camOfs->begin ("debugMat", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
		camOfs->position (-q, -q, -q); camOfs->colour (c2);
		camOfs->position (-q, q, -q);
		camOfs->position (q, -q, -q);
		camOfs->position (q, q, -q);
		camOfs->position (q, -q, q);
		camOfs->position (q, q, q);
		camOfs->position (-q, -q, q);
		camOfs->position (-q, q, q);
		camOfs->position (-q, -q, -q);
		camOfs->position (-q, q, -q);
		camOfs->end ();

		// Camera "navigation"
		// Described here (5th post) :
		// http://www.ogre3d.org/forums/viewtopic.php?p=241142
		Ogre::SceneNode *camMainFocus =
			mScn->getRootSceneNode ()->createChildSceneNode ("camMainFocus");
		camMainFocus->setPosition (0, 0, 0);
		Ogre::SceneNode *camMainOfs =
			camMainFocus->createChildSceneNode ("camMainOfs");
		camMainFocus->attachObject (camFocus);
		camMainOfs->setPosition (0, 0, 0);
		camMainOfs->attachObject (mCam);
		camMainOfs->attachObject (camOfs);
		mCam->lookAt (camMainFocus->getPosition ());
		mCam->setPosition (0, 0, 20);

		// show axes so one can orient what is where
		Ogre::SceneNode *axesSn =
			mScn->getRootSceneNode ()->createChildSceneNode ("Axes");
		Ogre::ManualObject* sm = mScn->createManualObject ("axesMobj");
		// colour() works with no lighting material only
		// a wild guess - it uses color material when no lighting
		sm->begin ("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
		float al = 100;
		sm->position (0, 0, 0);	sm->colour (0, 1, 0);
		sm->position (al, 0, 0); sm->colour (0, 1, 0);// X axis - green
		sm->position (0, 0, 0);	sm->colour (0, 0, 1);
		sm->position (0, al, 0); sm->colour (0, 0, 1);// Y axis - blue
		sm->position (0, 0, 0);	sm->colour (1, 0, 0);
		sm->position (0, 0, al); sm->colour (1, 0, 0);// Z axis - red
		sm->end ();
		axesSn->attachObject (sm);

		/*
		OGRE main loop when no windowing system is available - eventually
		TODO: roadmap
		mRoot->startRendering();
		while (true) {
			Ogre::WindowEventUtilities::messagePump ();
			mRoot->renderOneFrame ();
			if (mWin->isClosed ())
				return false;
		}*/
		return ready;
	}// go

	/*
	*	QWidget event handler.
	*/
	OVERRIDE void
	Qt4OGRE3D::mousePressEvent(QMouseEvent *event)
	{
		lastPos = event->pos ();
	}

	OVERRIDE void
	Qt4OGRE3D::mouseReleaseEvent(QMouseEvent *event)
	{
		// move "parent" to "child" pos in world space
		/*Ogre::SceneNode *camMainFocus = mScn->getSceneNode ("camMainFocus");
		Ogre::SceneNode *camMainOfs = mScn->getSceneNode ("camMainOfs");
		Ogre::Vector3 p3 =
			mScn->getRootSceneNode ()->convertLocalToWorldPosition (
				camMainFocus->getPosition ());
		Ogre::Vector3 p4 =
			mScn->getRootSceneNode ()->convertLocalToWorldPosition (
				camMainOfs->getPosition ());
		camMainFocus->setPosition (p4.x+p3.x, p4.y+p3.y, p4.z+p3.z);
		camMainOfs->setPosition (0, 0, 0);*/
	}

	/*
	*	QWidget event handler.
	*/
	OVERRIDE void
	Qt4OGRE3D::mouseMoveEvent(QMouseEvent *event)
	{
		if (!ready)
			return;
		// TODO: trackball
		// behaves like NifSkope right now I hope
		int dx = event->x () - lastPos.x ();
		int dy = event->y () - lastPos.y ();
		if (event->buttons () & Qt::LeftButton) {
		}

		Ogre::SceneNode *camMainOfs = mScn->getSceneNode ("camMainOfs");
		Ogre::SceneNode *camMainFocus = mScn->getSceneNode ("camMainFocus");
		if (event->buttons () & Qt::LeftButton) {
			Ogre::Real sx = (Ogre::Real)dx/-100;// TODO: these "k" are stub
			Ogre::Real sy = (Ogre::Real)dy/-100;
			camMainFocus->yaw (Ogre::Radian (sx));//, Ogre::Node::TS_WORLD);
			camMainOfs->pitch (Ogre::Radian (sy));//, Ogre::Node::TS_LOCAL);
			//camMainOfs->rotate (Ogre::Vector3 (1,0,0), Ogre::Radian (sy));
			//camMainOfs->rotate (Ogre::Vector3 (0,1,0), Ogre::Radian (sx));
			//camMainOfs->rotate (Ogre::Vector3 (0,0,1), Ogre::Radian (0));
		} else
		if (event->buttons () & Qt::MidButton) {
			Ogre::Real sx = (Ogre::Real)dx/-10;// TODO: these "k" are stub
			Ogre::Real sy = (Ogre::Real)dy/10;
           	camMainOfs->translate (sx, sy, 0, Ogre::Node::TS_LOCAL);
		} else
		if (event->buttons () & Qt::RightButton) {
			//Ogre::Real sx = (Ogre::Real)dx/100;
			//camMainOfs->roll (Ogre::Radian (sx));//, Ogre::Node::TS_LOCAL);
			//Ogre::Real sx = (Ogre::Real)dx/-10;// TODO: these "k" are stub
			//Ogre::Real sy = (Ogre::Real)dy/10;
			//Ogre::Vector3 camp = mCam->getPosition ();
			//camp.z += (sx+sy);
			//mCam->setPosition (camp);
		}
		lastPos = event->pos ();
		//update ();
	}

	/*
	*	QWidget event handler.
	*/
	OVERRIDE void
	Qt4OGRE3D::wheelEvent(QWheelEvent *event)
	{
		if (!ready)
			return;
		Ogre::Vector3 camp = mCam->getPosition ();
		camp.z += (Ogre::Real)event->delta ()/-10;// TODO: these "k" are stub
		mCam->setPosition (camp);
		//update ();
	}

	/*
	*	QWidget event handler. Occurs after the widget is resized.
	*/
	OVERRIDE void
	Qt4OGRE3D::resizeEvent(QResizeEvent *p)// overrides
	{
		if (!ready)
			return;
		mWin->resize (p->size ().width (), p->size ().height ());
		//mWin->windowMovedOrResized ();// TODO: not sure why should I call that
		mCam->setAspectRatio (
			(Ogre::Real)p->size ().width ()/p->size ().height ());
	}

	/*
	*	QTimer event handler. Qt "slot".
	*/
	void
	Qt4OGRE3D::Render()
	{
		if (!ready)
			return;

		if (progress) {
			if (rnd (100) == 42)
				ar = -ar;
			if (rnd (100) == 2)
				br = -br;
			if (rnd (100) == 4)
				cr = -cr;
			progress->yaw (Ogre::Radian (ar));
			progress->pitch (Ogre::Radian (br));
			progress->roll (Ogre::Radian (cr));
		}

		Ogre::WindowEventUtilities::messagePump ();
		mRoot->renderOneFrame ();

		// update (); causes 100% cpu load for no apparent reason
		// TODO: figure out why
	}

	/*
	*	QWidget event handler.
	*	Occurs when the widget needs to update its "outlook".
	*/
	OVERRIDE void
	Qt4OGRE3D::paintEvent(QPaintEvent *p)
	{
		if (!ready)
			return;
		Ogre::WindowEventUtilities::messagePump ();
		mRoot->renderOneFrame ();
	}

	/*
	*	NifSkope::FileIO event handler. Occurs after the parser
	*	has loaded a file.
	*/
	void
	Qt4OGRE3D::LoadNif(IEvent *sender)
	{
		// TODO: to settings
		std::string texbase ("/mnt/workspace/rain/c/nif/test/nfiskope_bin/data/");
		Ogre::ResourceGroupManager::getSingleton ().addResourceLocation (
			texbase, "FileSystem");

		NifLib::Compiler *nif = App->File.NifFile;
		std::string tex;
		std::string matname;
		int block = -1;
		for (int i = 0; i < nif->FCount (); i++) {
			NifLib::Field *f = (*nif)[i];
			if (i > 0 && f->BlockTag != ((*nif)[i-1])->BlockTag)
				block++;
			//NiTriStripsData
			NifLib::Tag *ft = f->BlockTag;
			if (!ft)
				continue;// field has no block type - its ok
			NifLib::Attr *tname = ft->AttrById (ANAME);
			if (!tname)
				continue;// a tag has no "name" - error
			int pblock = block;
			if (tname->Value.Equals ("NiSourceTexture", 15)) {
				ft = f->Tag;
				tname = ft->AttrById (ANAME);
				if (tname->Value.Equals ("Value", 5)) {
					NSINFO("found NiSourceTexture.Value at #" << block)
					//NSINFO(std::string (f->Value.buf, f->Value.len))
					std::stringstream tmp;
					tmp << std::string (f->Value.buf, f->Value.len);
					tex = tmp.str ();
					int pathlen = tex. length ();
					char *path = new char[pathlen];
					memcpy (path, tex.c_str (), pathlen);
					for (int i = 0; i < pathlen; i++)
						if (path[i] == '\\')
							path[i] = '/';
					tex = std::string (path, pathlen);
					delete [] path;
					NSINFO(tex)
					//std::stringstream matname;
					std::stringstream matnamestream;
					matnamestream << "Tex " << block;
					matname = matnamestream.str ();
					NSINFO("mat set to: \"" << matname << "\"")
					Ogre::MaterialPtr mat =
						Ogre::MaterialManager::getSingleton ().create(matname,
						Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
					//Ogre::TextureUnitState* tuisTexture =
						mat->getTechnique (0)->getPass (0)->createTextureUnitState (
						tex);
				}
			}
			else if (tname->Value.Equals ("NiTriStripsData", 15)) {
				//int vnum = 0;
				int sl = 0;
				NIFfloat *v = NULL, *n = NULL, *uv = NULL;
				NIFushort *s = NULL;
				NIFushort *si = NULL;
			while (pblock == block && ++i < nif->FCount ()) {
				//NSINFO ("found NiTriStripsData at #" << block)
				ft = f->Tag;
				tname = ft->AttrById (ANAME);
				// need to find a few things:
				// +NiGeometryData
				//  "Vertices" type="Vector3" arr1="Num Vertices" cond="Has Vertices"
				//  "Normals" type="Vector3" arr1="Num Vertices" cond="Has Normals"
				//  "UV Sets" type="TexCoord" arr1="Num UV Sets & 63" arr2="Num Vertices"
				// +NiTriStripsData
				//  "Num Strips" 155
				//  "Strip Lengths" type="ushort" arr1="Num Strips"
				//  "Points" type="ushort" arr1="Num Strips" arr2="Strip Lengths"
				if (tname->Value.Equals ("Vertices", 8)) {
					NSINFO ("found NiTriStripsData.Vertices at #" << block)
					//vnum = f->Value.len / 4;
					v = (NIFfloat *)&f->Value.buf[0];
				}
				else if (tname->Value.Equals ("Normals", 7)) {
					NSINFO ("found NiTriStripsData.Normals at #" << block)
					n = (NIFfloat *)&f->Value.buf[0];
				}
				else if (tname->Value.Equals ("UV Sets", 7)) {
					NSINFO ("found NiTriStripsData.UV Sets at #" << block)
					uv = (NIFfloat *)&f->Value.buf[0];
				}
				//if (tname->Value.Equals ("Num Strips", 10))
				//	NSINFO ("found NiTriStripsData.Num Strips at #" << block)
				else if (tname->Value.Equals ("Strip Lengths", 13)) {
					NSINFO ("found NiTriStripsData.Strip Lengths at #" << block)
					sl = f->Value.len / 2;
					s = (NIFushort *)&f->Value.buf[0];
				}
				else if (tname->Value.Equals ("Points", 6)) {
					NSINFO ("found NiTriStripsData.Points at #" << block)
					si = (NIFushort *)&f->Value.buf[0];
					int base = 0;
					std::stringstream objn;
					objn << "Mesh " << block;
					std::stringstream nn;
					nn << "Node " << block;
					Ogre::SceneNode *mySceneNode =
						mScn->getRootSceneNode ()->createChildSceneNode (nn.str ());
					mySceneNode->setScale (0.5, 0.5, 0.5);
					Ogre::ManualObject* sm = mScn->createManualObject (objn.str ());
					NSINFO("Using mat: \"" << matname << "\"")
					for (int m = 0; m < sl; m++) {
						// submesh
						/*ManualObject* sm = mScn->createManualObject ("manual");
						sm->begin ("BaseWhiteNoLighting",
							RenderOperation::OT_TRIANGLE_STRIP);*/
						//INFO ("m[" << m << "]=" << s[m])
						sm->begin (matname,// "BaseWhite",
							Ogre::RenderOperation::OT_TRIANGLE_STRIP);
						for (int vidx = base; vidx < base + s[m]; vidx++) {
							// faces
							int idx = si[vidx];
							NIFfloat *vertex = &v[3*idx];
							NIFfloat *normal = &n[3*idx];
							NIFfloat *tcoord = &uv[2*idx];
							/*INFO("#" << vidx - base << "(("
							<< vertex[0] << ", " << vertex[1] << ", " << vertex[2]
							<< ") ("
							<< normal[0] << ", " << normal[1] << ", " << normal[2]
							<< ") ("
							<< tcoord[0] << ", " << tcoord[1] << "))")*/
							sm->position (vertex[0], vertex[1], vertex[2]);
							sm->normal (normal[0], normal[1], normal[2]);
							sm->textureCoord (tcoord[0], tcoord[1]);
							//sm->index (vidx - base);
						}
						sm->end ();
						base += s[m];
					}//
					mySceneNode->attachObject (sm);
				}
				f = (*nif)[i];
				if (i > 0 && f->BlockTag != ((*nif)[i-1])->BlockTag)
					block++;
			}
			}// if "NiTriStripsData"
		}// for each field
	}
}
