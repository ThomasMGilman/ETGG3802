#pragma once

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

		// Structure for messages
		struct message
		{
			float pos_x, pos_y;
			Ogre::TextAreaOverlayElement* message;
		};

		LogManager();
		message set_new_text_element(std::string caption, Ogre::ColourValue text_color = Ogre::ColourValue(), float pos_x = 0, float pos_y = 0, float width = 100, float height = 100);
		
		// Logs messages to a log file
		void log(std::string msg);

		// Outputs messages to the overlay as well as the log file
		void log_message(std::string msg, Ogre::ColourValue colour, float log_time);

		void update(Ogre::Real elapsed_time);

		~LogManager();

	private:
		// Ogre Overlay tools
		Ogre::OverlayManager& overlay_manager = Ogre::OverlayManager::getSingleton();
		Ogre::Overlay* overlay;
		Ogre::OverlayContainer* overlay_panel;

		// Container for text to display to screen
		std::vector<std::tuple<float, message>> logger;
};