#ifndef _STDAFX_H_
#define _STDAFX_H_

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
// Put any headers here that won't change (like fstream or Ogre.h).  DO NOT put any
// ssuge-related headers here.  Make sure, in every .h and .cpp file we create in the
// project, to include <stdafx.h> first (it's mostly a Visual Studio thing, but it is also
// good so we don't have to include the headers (indirectly) included here in our files.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 

// system headers
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <sstream>

// Ogre headers
#pragma warning(disable : 4251)				// Gets rid of some pesky dll-linkage warnings 
#pragma warning(disable : 4275)
#include <Ogre.h>
#include <OgreStringVector.h>
#include <OgreApplicationContext.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayContainer.h>
#include <Overlay/OgreOverlaySystem.h>

// SDL headers
#include <SDL.h>

// Python headers
#include <python.h>
#include <structmember.h>


#endif
