#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <variant>
#include <regex>
#include <time.h>

// Container headers
#include <unordered_set>
#include <map>
#include <list>
#include <vector>
#include <stack>

// XML Reader headers
#include <tinyxml2.h>

//SDL headers
#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_keyboard.h>
#include <SDL_gamecontroller.h>

// Ogre headers
#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlaySystem.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreApplicationContext.h>
#include <OgreFont.h>
#include <OgreFontManager.h>

// Python headers
#include<Python.h>
#include<structmember.h>

// Log Manager Paramaters
#define CHAR_HEIGHT 16
#define RAND_COLOUR_VAL static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
#define RAND_COLOUR Ogre::ColourValue(RAND_COLOUR_VAL, RAND_COLOUR_VAL, RAND_COLOUR_VAL)
#define ERROR_COLOUR Ogre::ColourValue(1, 0, 0)
#define ERROR_DISPLAY_TIME 10.0f