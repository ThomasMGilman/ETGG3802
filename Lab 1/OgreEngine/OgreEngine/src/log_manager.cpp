#include "log_manager.h"

LogManager::LogManager(float vpHeight, std::string logFileName, int numLogs)
{
	// Get Ogres Overlay Manager and create and overlay
	mOverlay = mOverlayManager.create("debug_overlay");

	// Create panel for drawing text to
	mOverlayPanel = static_cast<Ogre::OverlayContainer*>(mOverlayManager.createOverlayElement("Panel", "debug_panel"));
	mOverlayPanel->setPosition(0.0, 0.0);
	mOverlayPanel->setDimensions(100, 100);

	// Add panel to overlay
	mOverlay->add2D(mOverlayPanel);
	mOverlay->show();

	// Open logfile to write to
	mOutfile = std::ofstream(logFileName, std::ofstream::out | std::ofstream::trunc);

	int yPosAmount = vpHeight / numLogs;
	// setup text Areas
	for (int i = 0; i < numLogs; i++)
	{
		std::string name = "TextArea";
		std::string localName = "debug_text_area" + std::to_string(i);
		Ogre::TextAreaOverlayElement* text_area = static_cast<Ogre::TextAreaOverlayElement*>(mOverlayManager.createOverlayElement(name, localName));
		text_area->setMetricsMode(Ogre::GMM_PIXELS);
		text_area->setPosition(0, i * yPosAmount);
		text_area->setDimensions(100, 100);
		text_area->setCharHeight(16);
		text_area->setFontName("SdkTrays/Value");

		mOverlayPanel->addChild(text_area);

		mLogs.push_back(msg_pack{ 0, text_area });
	}
}

// Fix here
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
	mOutfile << clt->tm_mon + 1 << "/" << clt->tm_mday << "/" << clt->tm_year + 1900 << "@" << hour << ":" << clt->tm_min << ":" << clt->tm_sec << (is_pm ? "pm " : "am ") << msg << std::endl; 
	mOutfile.flush();
}

void LogManager::log_message(std::string msg, Ogre::ColourValue color, float log_time_seconds)
{
	// Log Message sent and set a text area with message
	log(msg);
	set_text_element(msg, color, log_time_seconds);
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
}

LogManager::~LogManager()
{
	// Close Output log file
	mOutfile.close();
}