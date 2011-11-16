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

#ifndef __QT4OGRE3D_H__
#define __QT4OGRE3D_H__

#include "Qt43D.h"

// OGRE
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>

#include <OgreLogManager.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgrePlugin.h>

#include "ICommand.h"
#include "IEvent.h"

namespace NifSkope
{
	class Qt4OGRE3D: public Qt43D
	{
	Q_OBJECT
		ICommand *handleNifLoaded;
	private:
		int ready;
		QPoint lastPos;
	protected:
		// Qt delegates
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void resizeEvent(QResizeEvent *p);
		void paintEvent(QPaintEvent *p);
	public:
		//static Qt4OGRE3D * create();
		Qt4OGRE3D(void);
		~Qt4OGRE3D(void);
		bool go();
		// handlers
		void LoadNif(IEvent *sender);
 	public slots:
		void Render();
	protected:
		Ogre::Root *mRoot;
		Ogre::Camera *mCam;
		Ogre::SceneManager *mScn;
		Ogre::RenderWindow *mWin;
		Ogre::Viewport *mVp;
		Ogre::SceneNode *progress;
		Ogre::Real ar, br, cr;
		Ogre::NameValuePairList *misc;
	};
}

#endif /*__QT4OGRE3D_H__*/
