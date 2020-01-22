#include "log_manager.h"

LogManager::LogManager()
{
	// Get Ogres Overlay Manager and create and overlay
	overlay = overlay_manager.create("debug_overlay");

	// Create panel for drawing text to
	overlay_panel = static_cast<Ogre::OverlayContainer*>(overlay_manager.createOverlayElement("Panel", "debug_panel"));
	overlay_panel->setPosition(0.0, 0.0);
	overlay_panel->setDimensions(100, 100);

	// Add panel to overlay
	overlay->add2D(overlay_panel);
	overlay->show();
}

LogManager::message LogManager::set_new_text_element(std::string caption, Ogre::ColourValue text_color, float pos_x, float pos_y, float width, float height)
{
	Ogre::TextAreaOverlayElement* text_area = static_cast<Ogre::TextAreaOverlayElement*>(overlay_manager.createOverlayElement("TextArea", "debug_text_area"));
	text_area->setMetricsMode(Ogre::GMM_PIXELS);
	text_area->setPosition(pos_x, pos_y);
	text_area->setDimensions(width, height);
	text_area->setCaption(caption);
	text_area->setCharHeight(16);
	text_area->setFontName("SdkTrays/Value");
	text_area->setColour(text_color);

	// Add newly created Text area to overlay_panel to be displayed
	overlay_panel->addChild(text_area);

	return LogManager::message{pos_x, pos_y, text_area};
}

void LogManager::log(std::string msg)
{

}

void LogManager::log_message(std::string msg, Ogre::ColourValue colour, float log_time)
{
	// Variables to store previouse message parameters and pointers
	LogManager::message tmp, prev_text;

	// Append new message with no messages currently shown
	if (logger.empty())
		tmp = set_new_text_element(msg, colour);
	// Append new message alongside previous messages
	else
	{
		//Get last text element
		prev_text = std::get<1>(logger.back());
		tmp = set_new_text_element(msg, colour, prev_text.pos_x, prev_text.pos_y + prev_text.message->getHeight());
	}
	logger.push_back(std::tuple<float, LogManager::message>{log_time, tmp});
}

void LogManager::update(Ogre::Real elapsed_time)
{
	// Update all the log messages currently displayed
	if (!logger.empty())
	{
		auto msg = logger.begin();
		while (msg != logger.end())
		{
			// Update message looked ats time left on screen and check if it needs to be removed
			if ((std::get<0>(*msg) -= elapsed_time) <= 0)
			{
				overlay_panel->removeChild(std::get<1>(*msg).message->getName());
				logger.erase(msg);
			}

			// If message doesnt need to be removed, move to next message
			else
				msg++;
		}
		
		// update the position of all text elements after removeing previous ones
		msg = logger.begin();
		LogManager::message* current_text_area, * prev_text_area;
		float prev_ypos;
		while(msg != logger.end())
		{
			current_text_area = &std::get<1>(*msg);
			prev_ypos = current_text_area->pos_y;

			// Update First messages position if not at top of screen
			if (msg == logger.begin() && current_text_area->pos_y != 0)
				current_text_area->pos_y = 0;
			// Update current text area to below previous_text_area
			else
				current_text_area->pos_y = prev_text_area->pos_y + prev_text_area->message->getHeight();

			// If new y position is different from previous y position for current_text_area, update text areas position
			if(prev_ypos != current_text_area->pos_y)
				current_text_area->message->setPosition(current_text_area->pos_x, current_text_area->pos_y);

			prev_text_area = current_text_area;
			msg++;
		}
	}
}

LogManager::~LogManager()
{

}