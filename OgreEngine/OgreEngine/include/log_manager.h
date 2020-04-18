#pragma once

#include <stdafx.h>
#include <singleton.h>

#define LOG_MANAGER LogManager::getSingletonPtr()

namespace OgreEngine {
	class LogManager : public Singleton<LogManager>
	{
	public:
		LogManager(float vpHeight, float vpWidth, std::string logFileName = "log.txt", int numLogs = 20);

		std::string get_current_time();

		void toggle_debug_panel();

		void update_debug_panel(Ogre::Real elapsed_time);

		// Creates a new text element area with the desired caption and color and returns it as a message structure with its position stored
		void set_text_element(std::string caption, Ogre::ColourValue text_color, float log_time_seconds);

		// Logs messages to a log file
		void log(std::string msg);

		// Outputs messages to the overlay as well as the log file
		void log_message(std::string msg, Ogre::ColourValue text_color = RAND_COLOUR, float log_time_seconds = 2);

		// Outputs an error message to the overlay and log file
		void log_error(std::string msg);

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
		Ogre::OverlayContainer* mOverlayPanel, *mOverlayDebugPanel;

		// Time object
		std::time_t mTimer;

		// Output stream
		std::ofstream mOutfile;

		// For debugging
		Ogre::Real last_elapsed;
		// Container for text to display to screen
		std::vector<msg_pack> mLogs;
		std::vector<msg_pack> mDebugLogs;
		std::stack<std::tuple<float, std::string, Ogre::ColourValue>> mPendingLogs;
	};
}