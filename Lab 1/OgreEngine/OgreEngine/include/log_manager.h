#pragma once

#include <time.h>
#include <stack>
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include "OgreOverlaySystem.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreFont.h"
#include "OgreFontManager.h"

class LogManager
{
	public:
		LogManager(float vpHeight, std::string logFileName = "log.txt", int numLogs = 20);

		// Creates a new text element area with the desired caption and color and returns it as a message structure with its position stored
		void set_text_element(std::string caption, Ogre::ColourValue text_color, float log_time_seconds);
		
		// Logs messages to a log file
		void log(std::string msg);

		// Outputs messages to the overlay as well as the log file
		void log_message(std::string msg, Ogre::ColourValue text_color = Ogre::ColourValue(), float log_time_seconds = 2);
		
		// Updates the messages on the screen and removes messages after their time is up
		void update(Ogre::Real elapsed_time);

		~LogManager();

	private:

		struct msg_pack 
		{
			float msgTimeLeft;
			Ogre::TextAreaOverlayElement* msgArea;
		};

		// Ogre Overlay tools
		Ogre::OverlayManager& mOverlayManager = Ogre::OverlayManager::getSingleton();
		Ogre::Overlay* mOverlay;
		Ogre::OverlayContainer* mOverlayPanel;

		// Time object
		std::time_t mTimer;

		// Output stream
		std::ofstream mOutfile;

		// Container for text to display to screen
		std::vector<msg_pack> mLogs;
		std::stack<std::tuple<float, std::string, Ogre::ColourValue>> mPendingLogs;
};