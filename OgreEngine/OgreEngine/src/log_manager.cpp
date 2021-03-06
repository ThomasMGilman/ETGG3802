#include <stdafx.h>
#include "log_manager.h"
#include <application.h>

using namespace OgreEngine;

LogManager* LogManager::msSingleton = nullptr;

LogManager::LogManager(float vpHeight, float vpWidth, std::string logFileName, int numLogs)
{
	// Get Ogres Overlay Manager and create and overlay
	mOverlay = mOverlayManager.create("debug_overlay");

	// Create panel for drawing text to
	mOverlayPanel = static_cast<Ogre::OverlayContainer*>(mOverlayManager.createOverlayElement("Panel", "log_panel"));
	mOverlayPanel->setPosition(0.0, 0.0);
	mOverlayPanel->setDimensions(100, 100);

	// Add panel to overlay
	mOverlay->add2D(mOverlayPanel);
	mOverlay->show();

	// Open logfile to write to
	mOutfile = std::ofstream(logFileName, std::ofstream::out | std::ofstream::trunc);

	int yPosAmount = CHAR_HEIGHT; //vpHeight / numLogs;
	// setup text Areas
	for (int i = 0; i < numLogs; i++)
	{
		std::string name = "TextArea";
		std::string localName = "log_text_area" + std::to_string(i);
		Ogre::TextAreaOverlayElement* text_area = static_cast<Ogre::TextAreaOverlayElement*>(mOverlayManager.createOverlayElement(name, localName));
		text_area->setMetricsMode(Ogre::GMM_PIXELS);
		text_area->setPosition(0, i * yPosAmount);
		text_area->setDimensions(100, 100);
		text_area->setCharHeight(CHAR_HEIGHT);
		text_area->setFontName("SdkTrays/Value");

		mOverlayPanel->addChild(text_area);

		mLogs.push_back(msg_pack{ 0, text_area });
	}

	// Create Debug panel
	mOverlayDebugPanel = static_cast<Ogre::OverlayContainer*>(mOverlayManager.createOverlayElement("Panel", "debug_panel"));
	mOverlayDebugPanel->setPosition(0.0, 0.0);
	mOverlayDebugPanel->setDimensions(100, 100);

	// Add panel to overlay
	mOverlay->add2D(mOverlayDebugPanel);
	mOverlay->show();

	yPosAmount = CHAR_HEIGHT;
	for (int i = 0; i < 5; i++)
	{
		std::string name = "TextArea";
		std::string localName = "debug_text_area" + std::to_string(i);
		Ogre::TextAreaOverlayElement* text_area = static_cast<Ogre::TextAreaOverlayElement*>(mOverlayManager.createOverlayElement(name, localName));
		text_area->setMetricsMode(Ogre::GMM_PIXELS);
		text_area->setPosition(vpWidth - 170, i * yPosAmount);
		text_area->setDimensions(100, 100);
		text_area->setCharHeight(CHAR_HEIGHT);
		text_area->setFontName("SdkTrays/Value");
		text_area->setColour(DEBUG_COLOUR);

		mOverlayDebugPanel->addChild(text_area);
		mDebugLogs.push_back(msg_pack{ 0, text_area });
	}
}

std::string OgreEngine::LogManager::get_current_time()
{
	// Get time since Epoch, 1970-01-01 00:00:00 +0000 (UTC) https://linux.die.net/man/2/time
	mTimer = std::time(nullptr);
	std::tm* clt = std::localtime(&mTimer);

	// get hour by am/pm format
	int hour = clt->tm_hour;
	bool is_pm = false;
	if (hour > 11)
	{
		hour -= 10;
		is_pm = true;
	}
	else if (hour == 11)
		is_pm = true;
	return (std::to_string(clt->tm_mon + 1) + "/" + std::to_string(clt->tm_mday) + "/" + std::to_string(clt->tm_year + 1900) + "@" + std::to_string(hour) + ":" + std::to_string(clt->tm_min) + ":" + std::to_string(clt->tm_sec) + (is_pm ? "pm " : "am "));
}

void OgreEngine::LogManager::toggle_debug_panel()
{
	if (mOverlayDebugPanel->isVisible())
		mOverlayDebugPanel->hide();
	else
		mOverlayDebugPanel->show();
}

void OgreEngine::LogManager::update_debug_panel(Ogre::Real elapsed_time)
{
	if (mOverlayDebugPanel->isVisible())
	{
		for (int i = 0; i < mDebugLogs.size(); i++)
		{
			switch (i)
			{
			case 0:			// Current time
				mDebugLogs[i].msgArea->setCaption("Time: " + get_current_time());
				break;
			case 1:			// FPS
				mDebugLogs[i].msgArea->setCaption("FPS: " + std::to_string(1 / elapsed_time));
				break;
			case 2:			// Triangles
				mDebugLogs[i].msgArea->setCaption("TRIS: " + std::to_string(APPLICATION->getRenderWindow()->getStatistics().triangleCount));
				break;
			case 3:			// elapsed time since last frame
				mDebugLogs[i].msgArea->setCaption("Elapsed T: " + std::to_string(elapsed_time));
				break;
			case 4:			// last elapsed time since last frame
				mDebugLogs[i].msgArea->setCaption("Last Elapsed T: " + std::to_string(last_elapsed));
				break;
			default:
				break;
			}
		}
	}
	last_elapsed = elapsed_time;
}


/*
Logs a message into one of the text areas with the specified colour for the set amount of time if a slot is available.
Otherwise it is added to the stack for later.
*/
void LogManager::set_text_element(std::string caption, Ogre::ColourValue text_color, float log_time_seconds)
{
	bool textSet = false;
	for (int i = 0; i < mLogs.size(); i++)
	{
		if (mLogs[i].msgTimeLeft <= 0)
		{
			mLogs[i].msgTimeLeft = log_time_seconds;
			mLogs[i].msgArea->setCaption(caption);
			mLogs[i].msgArea->setColour(text_color);
			textSet = true;
			break;
		}
	}
	if (!textSet)
	{
		mPendingLogs.push(std::tuple<float, std::string, Ogre::ColourValue>(log_time_seconds, caption, text_color));
	}
}

void LogManager::log(std::string msg)
{
	// Get time since Epoch, 1970-01-01 00:00:00 +0000 (UTC) https://linux.die.net/man/2/time
	mTimer = std::time(nullptr);
	std::tm* clt = std::localtime(&mTimer);

	// get hour by am/pm format
	int hour = clt->tm_hour;
	bool is_pm = false;
	if (hour > 11)
	{
		hour -= 10;
		is_pm = true;
	}
	else if (hour == 11)
		is_pm = true;
	
	// output to file
	mOutfile << get_current_time() << msg << std::endl;
	mOutfile.flush();
}

void LogManager::log_message(std::string msg, Ogre::ColourValue color, float log_time_seconds)
{
	// Log Message sent and set a text area with message
	std::vector<std::string> lines;
	log(msg);
	int lastNewLine = 0;
	for (int i = 0; i < msg.length(); i++)
	{
		if (msg[i] == '\n')
		{
			lines.push_back(msg.substr(lastNewLine, i));
			lastNewLine = i+1;
		}
	}
	if(lines.size() == 0)
		set_text_element(msg, color, log_time_seconds);
	else
	{
		for (int i = 0; i < lines.size(); i++)
			set_text_element(lines[i], color, log_time_seconds);
	}
}

void OgreEngine::LogManager::log_error(std::string msg)
{
	log_message(msg, ERROR_COLOUR, ERROR_DISPLAY_TIME);
}

// Fix here
void LogManager::update(Ogre::Real elapsed_time)
{
	auto msgs = mLogs.begin();
	while (msgs != mLogs.end())
	{
		if (msgs->msgTimeLeft <= 0)
			break;
		else
		{
			if ((msgs->msgTimeLeft -= elapsed_time) <= 0)
			{
				auto msgUpdater = msgs;
				while (msgUpdater != mLogs.end())
				{
					if (msgUpdater != mLogs.end() - 1)
					{
						msgUpdater->msgTimeLeft = (msgUpdater + 1)->msgTimeLeft;
						msgUpdater->msgArea->setCaption((msgUpdater + 1)->msgArea->getCaption());
						msgUpdater->msgArea->setColour((msgUpdater + 1)->msgArea->getColour());
					}
					else
					{
						msgUpdater->msgTimeLeft = 0;
						msgUpdater->msgArea->setCaption("");
					}
					msgUpdater++;
				}
				continue;
			}
		}
		msgs++;
	}
	// Update values for text areas if logs are available and areas are available
	while (msgs != mLogs.end() && !mPendingLogs.empty())
	{
		std::tuple<float, std::string, Ogre::ColourValue> newLog = mPendingLogs.top();
		msgs->msgTimeLeft = std::get<0>(newLog);
		msgs->msgArea->setCaption(std::get<1>(newLog));
		msgs->msgArea->setColour(std::get<2>(newLog));

		mPendingLogs.pop();
		msgs++;
	}

	update_debug_panel(elapsed_time);
}

LogManager::~LogManager()
{
	// Close Output log file
	mOutfile.close();
}