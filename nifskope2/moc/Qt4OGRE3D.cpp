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
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>

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
		mCam->setNearClipDistance (0.1);// near clipping plane
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
		//mCam->lookAt (camMainFocus->getPosition ());// this is not a state

		// Rotate around "x", because .nifs are "height"
		// alongside "z" "-" to "+". Rotate around "x" - "+" is CCW
		camMainFocus->rotate (Ogre::Vector3 (1, 0, 0), Ogre::Radian (3.14/2));
		// "Sets the position of the node relative to it's parent."
		camMainOfs->setPosition (0, 0, 20);
		// TODO: not sure how this updates the "up" vector
		mCam->lookAt (camMainFocus->getPosition ());


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

#define INFO3(P,A,B,C)\
	NSINFO(P << "("#A":" << A << ", "#B":" << B << ", "#C":" << C << ")")
#define INFO4(P,A,B,C,D)\
	NSINFO(P << "("#A":" << A << ", "#B":" << B << ", "#C":" << C << ", "#D":" << D << ")")

//http://www.visgraf.impa.br/Projects/3dp/doc/html2/trackball_8cpp-source.html
/*
* This size should really be based on the distance from the center of
* rotation to the point on the object underneath the mouse.  That
* point would then track the mouse as closely as possible.  This is a
* simple example, though, so that is left as an Exercise for the
* Programmer.
*/
#define TRACKBALLSIZE  (0.9)
//Arcball variables
int spinning = 1, moving = 0;
int beginx, beginy;
float curquat[4];
float lastquat[4];
int scaling;
float scalefactor = 1.0;
static float tb_project_to_sphere(float, float, float);
static void normalize_quat(float [4]);
static void axis_to_quat(float a[3], float phi, float q[4]);
#define vzero(v) {v[0] = 0.0; v[1] = 0.0; v[2] = 0.0;}
#define vset(v,x,y,z) {v[0] = x; v[1] = y; v[2] = z;}
void
vsub(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}
void
vcopy(const float *v1, float *v2)
{
	register int i;
	for (i = 0 ; i < 3 ; i++)
		v2[i] = v1[i];
}
void
vcross(const float *v1, const float *v2, float *cross)
{
	float temp[3];
	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy (temp, cross);
}
float
vlength(const float *v)
{
	return sqrt ((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
}
void
vscale(float *v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}
void
vnormal(float *v)
{
	vscale (v, 1.0 / vlength (v));
}
float
vdot(const float *v1, const float *v2)
{
	return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}
void
vadd(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}
/*
* Ok, simulate a track-ball.  Project the points onto the virtual
* trackball, then figure out the axis of rotation, which is the cross
* product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
* Note:  This is a deformed trackball-- is a trackball in the center,
* but is deformed into a hyperbolic sheet of rotation away from the
* center.  This particular function was chosen after trying out
* several variations.
*
* It is assumed that the arguments to this routine are in the range
* (-1.0 ... 1.0)
*/
void
trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; // Axis of rotation
	float phi;  // how much to rotate about axis
	float p1[3], p2[3], d[3];
	float t;
	if (p1x == p2x && p1y == p2y) {// Zero rotation
		vzero (q);
		q[3] = 1.0;
		return;
	}
	// First, figure out z-coordinates for projection of P1 and P2 to
	// deformed sphere
	vset (p1, p1x, p1y, tb_project_to_sphere (TRACKBALLSIZE, p1x, p1y));
	vset (p2, p2x, p2y, tb_project_to_sphere (TRACKBALLSIZE, p2x, p2y));
	// Now, we want the cross product of P1 and P2
	vcross (p2, p1, a);
	// Figure out how much to rotate around that axis.
	vsub (p1, p2, d);
	t = vlength (d) / (2.0 * TRACKBALLSIZE);
	// Avoid problems with out-of-control values...
	if (t > 1.0)
		t = 1.0;
	if (t < -1.0)
		t = -1.0;
	phi = 2.0 * asin (t);
	axis_to_quat (a, phi, q);
}
/*
*  Given an axis and angle, compute quaternion.
*/
void
axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal (a);
	vcopy (a, q);
	vscale (q, sin (phi / 2.0));
	q[3] = cos (phi / 2.0);
}
/*
* Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
* if we are away from the center of the sphere.
*/
static float
tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;
 	d = sqrt ((x * x) + (y * y));
	if (d < r * 0.70710678118654752440) {// Inside sphere
		z = sqrt ((r * r) - (d * d));
	} else { // On hyperbola
		t = r / 1.41421356237309504880;// sqr (2)
		z = (t * t) / d;
	}
	return z;
}
/*
* Given two rotations, e1 and e2, expressed as quaternion rotations,
* figure out the equivalent single rotation and stuff it into dest.
*
* This routine also normalizes the result every RENORMCOUNT times it is
* called, to keep error from creeping in.
*
* NOTE: This routine is written so that q1 or q2 may be the same
* as dest (or each other).
*/
#define RENORMCOUNT 97
static void normalize_quat(float q[4]);
void
add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count=0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy (q1, t1);
	vscale (t1, q2[3]);

	vcopy (q2, t2);
	vscale (t2, q1[3]);

	vcross (q2, q1, t3);
	vadd (t1, t2, tf);
	vadd (t3, tf, tf);
	tf[3] = (q1[3] * q2[3]) - vdot (q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat (dest);
	}
}
/*
* Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
* If they don't add up to 1.0, dividing by their magnitued will
* renormalize them.
*
* Note: See the following for more information on quaternions:
*
* - Shoemake, K., Animating rotation with quaternion curves, Computer
*   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
* - Pletinckx, D., Quaternion calculus as a basic tool in computer
*   graphics, The Visual Computer 5, 2-13, 1989.
*/
static void
normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = ((q[0] * q[0]) + (q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]));
	for (i = 0; i < 4; i++)
		q[i] /= mag;
}

/*
* Build a rotation matrix, given a quaternion rotation.
*
*/
void
build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0 - 2.0 * ((q[1] * q[1]) + (q[2] * q[2]));
	m[0][1] = 2.0 * ((q[0] * q[1]) - (q[2] * q[3]));
	m[0][2] = 2.0 * ((q[2] * q[0]) + (q[1] * q[3]));
	m[0][3] = 0.0;

	m[1][0] = 2.0 * ((q[0] * q[1]) + (q[2] * q[3]));
	m[1][1]= 1.0 - 2.0 * ((q[2] * q[2]) + (q[0] * q[0]));
	m[1][2] = 2.0 * ((q[1] * q[2]) - (q[0] * q[3]));
	m[1][3] = 0.0;

	m[2][0] = 2.0 * ((q[2] * q[0]) - (q[1] * q[3]));
	m[2][1] = 2.0 * ((q[1] * q[2]) + (q[0] * q[3]));
	m[2][2] = 1.0 - 2.0 * ((q[1] * q[1]) + (q[0] * q[0]));
	m[2][3] = 0.0;

	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
}

	/*
	*	QWidget event handler.
	*/
	OVERRIDE void
	Qt4OGRE3D::mouseMoveEvent(QMouseEvent *event)
	{
		if (!ready)
			return;
		int dx = event->x () - lastPos.x ();
		int dy = event->y () - lastPos.y ();
		if (event->buttons () & Qt::LeftButton) {
		}

		Ogre::SceneNode *camMainOfs = mScn->getSceneNode ("camMainOfs");
		Ogre::SceneNode *camMainFocus = mScn->getSceneNode ("camMainFocus");
		if (event->buttons () & Qt::LeftButton) {
			Ogre::Real sx = (Ogre::Real)dx/-100;// TODO: these "k" are stub
			Ogre::Real sy = (Ogre::Real)dy/-100;
			// rotating "camMainFocus" rotates "camMainOfs" as well
			// "camMainFocus"_matrix *= Rotate
			// "camMainOfs->rotate (..., ..., TS_PARENT)" rotates "camMainOfs"
			// around its current position in "camMainFocus" and not around
			// "camMainFocus" which means its *R*T, not *T*R?
			float q[4];
			// linear interpolation to (-1, 1, 1, -1) <=> (0, width, 0, height)
			int w2 = this->width () / 2;
			int h2 = this->height () / 2;
			float wk = 2.0 / this->width ();
			float hk = 2.0 / this->height ();
			float p1x = wk * (lastPos.x () - w2);
			float p1y = -hk * (lastPos.y () - h2);// invert y axis
			float p2x = wk * (event->x () - w2);
			float p2y = -hk * (event->y () - h2);// invert y axis
			trackball (q, p1x, p1y, p2x, p2y);
			Ogre::Quaternion oq (q[3], q[0], q[1], q[2]);
			camMainFocus->rotate (oq);
			/*camMainFocus->rotate (
				Ogre::Vector3 (1, 0, 0), Ogre::Radian (sy), Ogre::Node::TS_LOCAL);
			camMainFocus->rotate (
				Ogre::Vector3 (0, 1, 0), Ogre::Radian (sx), Ogre::Node::TS_LOCAL);*/
			//camMainOfs->rotate (Ogre::Vector3 (0,0,1), Ogre::Radian (0));
		} else
		if (event->buttons () & Qt::MidButton) {
			Ogre::Real sx = (Ogre::Real)dx/-10;// TODO: these "k" are stub
			Ogre::Real sy = (Ogre::Real)dy/10;
			// translating "camMainFocus" translates "camMainOfs" as well
			// "camMainFocus"_matrix *= Translate
			camMainFocus->translate (sx, sy, 0, Ogre::Node::TS_LOCAL);
		} else
		if (event->buttons () & Qt::RightButton) {
			Ogre::Real sx = (Ogre::Real)dx/10;// TODO: these "k" are stub
			Ogre::Real sy = (Ogre::Real)dy/10;
			// manh. dist. fast'n'simple
			// "camMainOfs"_matrix *= Translate
			camMainOfs->translate (0, 0, sx + sy, Ogre::Node::TS_PARENT);
			Ogre::Vector3 p = camMainOfs->getPosition ();
			INFO3("", p.x, p.y, p.z)
		}
		lastPos = event->pos ();
	}

	/*
	*	QWidget event handler.
	*/
	OVERRIDE void
	Qt4OGRE3D::wheelEvent(QWheelEvent *event)
	{
		if (!ready)
			return;
		Ogre::SceneNode *camMainOfs = mScn->getSceneNode ("camMainOfs");
		camMainOfs->translate (0, 0, (Ogre::Real)event->delta ()/-10);
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

#define CHECKTHAT(COND,FACTN)\
	if (!(COND)) {\
		NSWRN ("LoadNif: failed: \"" << #COND << "\"")\
		FACTN;\
	}
	void
	Qt4OGRE3D::LoadNiAVObject(Ogre::SceneNode *snode, NifLib::Node *node)
	{
		//  NiAVObject.Translation": 12 "[Vector3]"
		//  NiAVObject.Rotation": 36 "[Matrix33]"
		//  NiAVObject.Scale": 4 "1"
		for (int i = 0; i < node->Nodes.Count (); i++) {
			NifLib::Node *n = node->Nodes[i];
			CHECKTHAT(n, continue)
			NifLib::Field *f = n->Value;
			CHECKTHAT(f, continue)
			std::string fn = f->Name ();
			if (fn == "Translation") {//TODO: HC dep. to "nif.xml"
				NSINFO("    " << fn << ": " << App->ToStr (f))
				CHECKTHAT(f->Value.len == 12, continue)
				NIFfloat *buf = (NIFfloat *)&(f->Value.buf[0]);
				Ogre::Real x = (Ogre::Real)buf[0];
				Ogre::Real y = (Ogre::Real)buf[1];
				Ogre::Real z = (Ogre::Real)buf[2];
				INFO3("     OGRE:", x, y, z)
				snode->translate (x, y, z, Ogre::Node::TS_PARENT);// default
			}
			else if (fn == "Rotation") {//TODO: HC dep. to "nif.xml"
				NSINFO("    " << fn << ": " << App->ToStr (f))
				CHECKTHAT(f->Value.len == 36, continue)
				NIFfloat *buf = (NIFfloat *)&(f->Value.buf[0]);
				// YX
				//[0] m11: 1, [3] m12: 0, [6] m13: 0,
				//[1] m21: 0, [4] m22: 1, [7] m23: 0,
				//[2] m31: 0, [5] m32: 0, [8] m33: 1
				INFO3("     OGRE:", buf[0], buf[3], buf[6])
				INFO3("     OGRE:", buf[1], buf[4], buf[7])
				INFO3("     OGRE:", buf[2], buf[5], buf[8])
				// XY - OGRE
				Ogre::Matrix3 m(
					(Ogre::Real)buf[0], (Ogre::Real)buf[3], (Ogre::Real)buf[6],
					(Ogre::Real)buf[1], (Ogre::Real)buf[4], (Ogre::Real)buf[7],
					(Ogre::Real)buf[2], (Ogre::Real)buf[5], (Ogre::Real)buf[8]);
				Ogre::Quaternion q(m);
				snode->rotate (q, Ogre::Node::TS_LOCAL);// default
			}
			else if (fn == "Scale") {//TODO: HC dep. to "nif.xml"
				NSINFO("    " << fn << ": unform: " << App->ToStr (f))
				CHECKTHAT(f->Value.len == 4, continue)
				NIFfloat *buf = (NIFfloat *)&(f->Value.buf[0]);
				Ogre::Real scale = (Ogre::Real)buf[0];
				NSINFO("     OGRE: scale: " << scale)
				snode->scale (scale, scale, scale);
			}
		}
	}

	void
	Qt4OGRE3D::LoadMesh(Ogre::SceneNode *snode,
		std::string matname, std::string tex_path,
		NIFfloat *v, int vc, NIFfloat *n, NIFfloat *uv,
		NIFushort *s, int sl, NIFushort *si)
	{
		// "create" the material
#define MM Ogre::MaterialManager::getSingleton ()
#define MMDEFRESGRP Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		if (!MM.resourceExists (matname)) {
			Ogre::MaterialPtr mat =	MM.create (
				matname, MMDEFRESGRP);
			if (tex_path != "") {
				//Ogre::TextureUnitState* tuisTexture =
				mat->getTechnique (0)->getPass (
					0)->createTextureUnitState (tex_path);
			}
		}
#undef MM

// TODO: check for presence
#define HBM Ogre::HardwareBufferManager::getSingleton()
		static int block = -1;
		std::stringstream objn;
		objn << "Mesh " << block++;
		std::stringstream obj2n;
		obj2n << "Mesh Entity " << block - 1;
		// http://www.ogre3d.org/docs/manual/manual_toc.html#SEC_Contents
		// http://www.ogre3d.org/tikiwiki/ManualSphereMeshes&structure=Cookbook
		std::string pMeshName = objn.str ();
		std::string pEntityName = obj2n.str ();
		NSINFO("     OGRE: creating mesh: " << pMeshName)
		NSINFO("     OGRE: creating mesh, entity: " << pEntityName)
		NSINFO("     OGRE: creating mesh, using mat: \"" << matname << "\"")
		Ogre::Real r = 142;
		Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton ().createManual (
			pMeshName, MMDEFRESGRP);
		// create the vertex buffer
		// TODO: who releases that "new"
		pMesh->sharedVertexData = new Ogre::VertexData ();
		Ogre::VertexData* vertexData = pMesh->sharedVertexData;
 		// define the vertex format
		Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
		size_t currOffset = 0;
		// v
		vertexDecl->addElement (0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		currOffset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
		// n
		vertexDecl->addElement (0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
		currOffset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
		// uv
		vertexDecl->addElement (
			0, currOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
		currOffset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT2);
		NSINFO("     OGRE: vertex buffer vertex count: " << vc)
		vertexData->vertexCount = vc;
		Ogre::HardwareVertexBufferSharedPtr vBuf = HBM.createVertexBuffer (
			vertexDecl->getVertexSize(0),
			vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false );
		Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
		binding->setBinding (0, vBuf);
		float* pVertex =
			static_cast<float*>(vBuf->lock (Ogre::HardwareBuffer::HBL_DISCARD));
		for (int i = 0; i < vc; i++) {
			NIFfloat *vertex = &v[3*i];
			NIFfloat *normal = &n[3*i];
			NIFfloat *tcoord = &uv[2*i];
			*pVertex++ = vertex[0];
			*pVertex++ = vertex[1];
			*pVertex++ = vertex[2];
			*pVertex++ = normal[0];
			*pVertex++ = normal[1];
			*pVertex++ = normal[2];
			*pVertex++ = tcoord[0];
			*pVertex++ = tcoord[1];
		}
		vBuf->unlock ();
		// create index buffers
		NSINFO("     OGRE: index buffer count: " << sl)
		int base = 0;
		for (int m = 0; m < sl; m++) {// sub-mesh
			Ogre::SubMesh *pMeshVertex = pMesh->createSubMesh ();
			pMeshVertex->useSharedVertices = true;
			pMeshVertex->operationType = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
			pMeshVertex->indexData->indexCount = s[m];
			pMeshVertex->indexData->indexBuffer = HBM.createIndexBuffer (
				Ogre::HardwareIndexBuffer::IT_16BIT,
				pMeshVertex->indexData->indexCount,
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
			Ogre::HardwareIndexBufferSharedPtr iBuf =
				pMeshVertex->indexData->indexBuffer;
			unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock (
				Ogre::HardwareBuffer::HBL_DISCARD));
			for (int vidx = base; vidx < base + s[m]; vidx++) {
				*pIndices++ = si[vidx];
			}
			iBuf->unlock ();
			base += s[m];
		}
		// "the original code was missing this line:"
		// TODO: without it there is nothing rendered - figure out why
		pMesh->_setBounds (Ogre::AxisAlignedBox (
			Ogre::Vector3 (-r, -r, -r), Ogre::Vector3 (r, r, r)), false);
		pMesh->_setBoundingSphereRadius (r);
		// "this line makes clear the mesh is loaded (avoids memory leaks)"
		pMesh->load ();
		// init scene node
		Ogre::Entity* meshEntity = mScn->createEntity (pEntityName, pMeshName);
		meshEntity->setMaterialName (matname);
		snode->attachObject (meshEntity);
#undef HBM
#undef MMDEFRESGRP
		// TODO: elseif no HardwareBuffer {
		/*int base = 0;
		static int block = -1;
		std::stringstream objn;
		objn << "Mesh " << block++;
		std::string mname = objn.str ();
		NSINFO("     OGRE: creating mesh: " << mname)
		//mySceneNode->setScale (0.5, 0.5, 0.5);
		Ogre::ManualObject* sm = mScn->createManualObject (mname);
		NSINFO("     OGRE: creating mesh, using mat: \"" << matname << "\"")
		for (int m = 0; m < sl; m++) {// sub-mesh
			sm->begin (matname,	Ogre::RenderOperation::OT_TRIANGLE_STRIP);
			for (int vidx = base; vidx < base + s[m]; vidx++) {// faces
				int idx = si[vidx];
				NIFfloat *vertex = &v[3*idx];
				NIFfloat *normal = &n[3*idx];
				NIFfloat *tcoord = &uv[2*idx];
				sm->position (vertex[0], vertex[1], vertex[2]);
				sm->normal (normal[0], normal[1], normal[2]);
				sm->textureCoord (tcoord[0], tcoord[1]);
				//sm->index (vidx - base);
			}
			sm->end ();
			base += s[m];
		}
		snode->attachObject (sm);*/
	}

	void
	Qt4OGRE3D::LoadNiTriStrips(Ogre::SceneNode *snode, NifLib::Node *node)
	{
		// snode is logical node
		//  NiBinaryExtraData
		//  NiTexturingProperty
		//	 NiSourceTexture.FileName
		//  NiMaterialProperty
		//  NiTriStripsData - geometry data
		std::string matname = "BaseWhite";
		std::string tex_path = "";
		for (int i = 0; i < node->Nodes.Count (); i++) {
			NifLib::Node *n = node->Nodes[i];
			CHECKTHAT(n, continue)
			NifLib::Field *f = n->Value;
			CHECKTHAT(f, continue)
			std::string fn = f->Name ();
			// find texture
			NifLib::Node *tex = NULL;
			if (fn == "NiTexturingProperty")//TODO: HC dep. to "nif.xml"
				tex = App->NodeByName ("NiSourceTexture", n);//TODO: HC dep. to "nif.xml"
			if (tex) {
				NifLib::Node *tn = App->GetTreeNode (tex);// compiler node
				CHECKTHAT(tn, continue)
				for (int j = 0; j < tn->Nodes.Count (); j++) {
					NifLib::Node *tnn = tn->Nodes[j];
					CHECKTHAT(tnn, continue)
					NifLib::Field *tnf = tnn->Value;
					CHECKTHAT(tnf, continue)
					std::string tnfn = tnf->Name ();
					if (tnfn == "Use External") {
						NSINFO("    " << tnfn << ": " << App->ToStr (tnf))
						if (tnf->AsNIFbyte () == 0) {
							NSWRN("Not supported yet")
							break;// no external texture, so stop searching
						}
					} else
					if (tnfn == "File Name") {
						tex_path = App->GetNodeValue (tnn);
						// adjust path separator
						int pathlen = tex_path. length ();
						CHECKTHAT(pathlen > 0, continue)
						char *path = new char[pathlen];
						CHECKTHAT(path, continue)
						memcpy (path, tex_path.c_str (), pathlen);
						for (int i = 0; i < pathlen; i++)
							if (path[i] == '\\')
								path[i] = '/';
						tex_path = std::string (path, pathlen);
						delete [] path;
						NSINFO("    " << tnfn << ": " << tex_path)
						matname = tex_path;
						NSINFO("     OGRE: material name: " << matname)
					}
				}
			} else // if (tex)
			// geometry
			if (fn == "NiTriStripsData") {//TODO: HC dep. to "nif.xml"
				NifLib::Node *gn = App->GetTreeNode (n);// compiler node
				CHECKTHAT(gn, continue)
				int sl = 0;// strip lengths
				int vc = 0;// vertex count
				NIFfloat *v = NULL, *n = NULL, *uv = NULL;
				NIFushort *s = NULL;
				NIFushort *si = NULL;
				//TODO: HC dep. to "nif.xml"
				NifLib::Field *_v = App->ByName ("Vertices", gn);//"Num Vertices"
				CHECKTHAT(_v, continue)
				NifLib::Field *_n = App->ByName ("Normals", gn);//"Num Vertices"
				CHECKTHAT(_n, continue)
				// can be more than 1, upto 63 - TODO: no idea why or what for
				NifLib::Field *_uv = App->ByName ("UV Sets", gn);//"Num Vertices"
				CHECKTHAT(_uv, continue)
				NifLib::Field *_s = App->ByName ("Strip Lengths", gn);
				CHECKTHAT(_s, continue)
				NifLib::Field *_si = App->ByName ("Points", gn);
				CHECKTHAT(_si, continue)
				v = (NIFfloat *)&(_v->Value.buf[0]);
				vc = _v->Value.len / (3 * 4);//TODO: HC dep. to "nif.xml"
				n = (NIFfloat *)&(_n->Value.buf[0]);
				uv = (NIFfloat *)&(_uv->Value.buf[0]);
				s = (NIFushort *)&(_s->Value.buf[0]);
				sl = _s->Value.len / 2;//TODO: HC dep. to "nif.xml"
				si = (NIFushort *)&(_si->Value.buf[0]);
				// delegate to OGRE  TODO: simplify
				LoadMesh (snode, matname, tex_path, v, vc, n, uv, s, sl, si);
				matname = "BaseWhite";
				tex_path = "";
			}// if (fn == "NiTriStripsData")
		}
	}
#undef INFO4
#undef INFO3
#undef CHECKTHAT

	/*
	*	NifSkope::FileIO event handler. Occurs after the parser
	*	has loaded a file.
	*/
	void
	Qt4OGRE3D::LoadNif(IEvent *sender)
	{
		// TODO: reset ()
		// TODO: to settings
		std::string texbase ("/mnt/workspace/rain/c/nif/test/nfiskope_bin/data/");
		Ogre::ResourceGroupManager::getSingleton ().addResourceLocation (
			texbase, "FileSystem");

		NifLib::Node *root = App->AsNifTree ();// logical tree
		for (int i = 0; i < root->Nodes.Count (); i++) {
			NifLib::Node *r = root->Nodes[i];// logical node
			Ogre::SceneNode *rscn = NULL;
			if (r->Value->Name () == "NiNode") {//TODO: HC dep. to "nif.xml"
				NifLib::Node *rc = App->GetTreeNode (r);// compiler node
				// form scene node name
				std::stringstream tmp;
				tmp << App->GetNodeValue (rc) << " (NiNode #" << i << ")";
				std::string rname = tmp.str ();
				NSINFO("NiNode #" << i << ": " << rname)
				// create the scene node
				rscn = mScn->getRootSceneNode ()->createChildSceneNode (rname);
				NifLib::Node *av = App->NodeByName ("NiAVObject", rc);//TODO: HC dep. to "nif.xml"
				if (av) {
					NSINFO("NiNode #" << i << ": applying transform")
					LoadNiAVObject (rscn, av);
				}
			}
			// TODO: should this be recursive ?
			for (int j = 0; j < r->Nodes.Count (); j++) {// logical tree
				NifLib::Node *g = r->Nodes[j];// logical node
				if (rscn && g->Value->Name () == "NiTriStrips") {//TODO: HC dep. to "nif.xml"
					NifLib::Node *gc = App->GetTreeNode (g);// compiler node
					// form scene node name
					std::stringstream tmp;
					tmp << App->GetNodeValue (gc) << " (NiTriStrips #" << j << ")";
					std::string rname = tmp.str ();
					NSINFO("  NiTriStrips #" << j << ": " << rname)
					// create the scene node
					Ogre::SceneNode *gscn =
						rscn->createChildSceneNode (rname);
					NifLib::Node *av = App->NodeByName ("NiAVObject", gc);//TODO: HC dep. to "nif.xml"
					if (av) {
						NSINFO("  NiTriStrips #" << j << ": applying transform")
						LoadNiAVObject (gscn, av);
					}
					LoadNiTriStrips (gscn, g);
				}
			}
		}
	}
}
