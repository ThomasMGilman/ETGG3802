#include <stdafx.h>
#include <log_manager.h>
#include <utility.h>


// Necessary to initialize the static singleton LogManager inherited from Singleton
template<> ssuge::LogManager* ssuge::Singleton<ssuge::LogManager>::msSingleton = NULL;










ssuge::LogManager::LogManager(std::string fname, unsigned int num_text_lines, Ogre::Vector2 upper_left, Ogre::Vector2 dimensions) : mFile(fname), mMatrix("log_manager", num_text_lines, upper_left, dimensions, true)
{
	// Make the log manager have a cloudy gray background
	mMatrix.set_background_material("TransparentGray");

	// Set the file to user zero-padding
	mFile << std::setfill('0');

}




ssuge::LogManager::~LogManager()
{
	// Close our log file
	mFile.close();
}



void ssuge::LogManager::update(float dt)
{
	mMatrix.update(dt);
}



void ssuge::LogManager::log(std::string msg)
{
	// New addition: if the msg we're given has newlines, break it into sub-parts
	if (ssuge::contains_char(msg, '\n'))
	{
		std::vector<std::string> elements;
		split_str(msg, elements, '\n');
		for (std::string s : elements)
			log(s);
	}
	else
	{
		// Reference: https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
		time_t t = time(NULL);
		struct tm now;
		localtime_s(&now, &t);


		mFile << std::setw(2) << now.tm_mon + 1 << "/" << std::setw(2) << now.tm_mday << "/" << now.tm_year + 1900;
		mFile << "@" << std::setw(2) << now.tm_hour << ":" << std::setw(2) << now.tm_min << ":" << std::setw(2) << now.tm_sec << "    ";
		mFile << msg << std::endl;
	}
}



void ssuge::LogManager::log(std::string msg, Ogre::ColourValue color, float time_on_gui)
{
	mMatrix.append(msg, color, time_on_gui, 1.0f);
	log(msg);
}


void ssuge::LogManager::log(std::string msg, ssuge::LogPreset type)
{
	Ogre::ColourValue col;
	float time;

	switch (type)
	{
	case LogPreset::ERROR:
		col = Ogre::ColourValue(1, 0, 0);
		time = 20.0f;
		break;
	case LogPreset::WARNING:
		col = Ogre::ColourValue(1, 1, 0);
		time = 10.0f;
		break;
	case LogPreset::DIAGNOSTIC:
		col = Ogre::ColourValue(0.2f, 1.0f, 0.2f);
		time = 5.0f;
		break;
	}

	log(msg, col, time);
}

