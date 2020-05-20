#pragma once
#include <stdafx.h>
#include <singleton.h>
#include <text_matrix.h>

// A way to shorten the code to access LogManager's singleton
#define LOG_MANAGER ssuge::LogManager::get_singleton_ptr()

namespace ssuge
{
	/// A convenience way to color certain types of messages when we display on the 
	/// visual log.
	enum class LogPreset { WARNING, ERROR, DIAGNOSTIC };

	/// The LogManager is used to save log message to a log file (re-created each time the program is run)
	/// and to optionally display messages on-screen in a "console".
	class LogManager : public Singleton<LogManager>
	{
		// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The file (which is opened when the log manager is created and stays open until it is destroyed)
		std::ofstream mFile;

		/// The visual log
		TextMatrix mMatrix;

		// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// Constructor
		LogManager(std::string fname, unsigned int num_text_lines, Ogre::Vector2 upper_left, Ogre::Vector2 dimensions);

		/// Destructor
		virtual ~LogManager();

		// @@@@@ METHODS @@@@@
	public:
		/// Should be called once per frame (to update the text fade and "bumping")
		virtual void update(float dt);

		/// Log a message to the log file.  
		virtual void log(std::string msg);

		/// Log a message to the log file *and* show it on-screen
		virtual void log(std::string msg, Ogre::ColourValue, float time_to_live);

		/// Show a visual log using a preset value
		virtual void log(std::string msg, LogPreset type);

		/// Sets the visibility of the visual log
		virtual void set_visibility(bool show) { mMatrix.set_visibility(show); }
	};
}


