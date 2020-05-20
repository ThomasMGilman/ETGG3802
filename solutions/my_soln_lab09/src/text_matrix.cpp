#include <stdafx.h>
#include <text_matrix.h>
#include <utility.h>


ssuge::TextMatrix::TextEntry::TextEntry(std::string text, Ogre::ColourValue color, float time_to_live, float fade_time) :
	mText(text), mColor(color), mTimeToLive(time_to_live), mOrigTimeToLive(time_to_live), mFadeTime(fade_time)
{
	// Intentionally empty -- we did everything in the initializer list
}


ssuge::TextMatrix::TextEntry::TextEntry(std::string text, Ogre::ColourValue color) : mText(text), mColor(color)
{
	// Intentionally empty -- we did everything in the initializer list
}






ssuge::TextMatrix::TextMatrix(std::string name, int num_lines, Ogre::Vector2 origin, Ogre::Vector2 dimensions, bool expiring_text) : mExpiringTextType(expiring_text)
{
	// Create the overlay
	Ogre::OverlayManager* mgr = Ogre::OverlayManager::getSingletonPtr();
	mOverlay = mgr->create(name);

	// Create a panel to hold all text elements
	mPanel = (Ogre::OverlayContainer*)(mgr->createOverlayElement("Panel", name + "\\Panel"));
	mPanel->setMetricsMode(Ogre::GMM_RELATIVE);
	mPanel->setPosition(origin.x, origin.y);
	mPanel->setDimensions(dimensions.x, dimensions.y);
	mOverlay->add2D(mPanel);

	// Create the text elements
	//float y_step = 1.0f / num_lines;
	float y_step = dimensions.y / num_lines;
	for (int i = 0; i < num_lines; i++)
	{
		Ogre::TextAreaOverlayElement* text = (Ogre::TextAreaOverlayElement*)mgr->createOverlayElement("TextArea", name + "\\Text" + std::to_string(i));
		text->setMetricsMode(Ogre::GMM_RELATIVE);
		text->setFontName("SdkTrays/Value");
		text->setPosition(0, i * y_step);
		text->setDimensions(dimensions.x, y_step);
		text->setCharHeight(y_step);
		mPanel->addChild(text);
		mTextSlots.push_back(text);

		if (!mExpiringTextType)
			mTextEntries.push_back(TextEntry("", Ogre::ColourValue::White, 1.0f));
	}

	// Show the overlay
	mOverlay->show();
}


void ssuge::TextMatrix::update(float dt)
{
	// Update the slots that have a visual log to fill them.
	for (unsigned int i = 0; i < mTextEntries.size(); )
	{
		if (mExpiringTextType)
			mTextEntries[i].mTimeToLive -= dt;
		if (mTextEntries[i].mTimeToLive <= 0.0f)
		{
			// This entry is done -- remove it.  Note: I *don't* increment i
			// in this case because there's now possibly another element there.
			mTextEntries.erase(mTextEntries.begin() + i);
		}
		else
		{
			// It's still alive -- update it
			if (i < mTextSlots.size())
			{
				// Update the text 
				mTextSlots[i]->setCaption(mTextEntries[i].mText);
				
				// optional effect -- makes the color fade as time passes
				float alpha = 1.0f;
				//if (mTextEntries[i].mFadeTime >= 0 && 
				//	mTextEntries[i].mTimeToLive < mTextEntries[i].mFadeTime)
				//	alpha = mTextEntries[i].mTimeToLive / mTextEntries[i].mFadeTime;
				Ogre::ColourValue color = mTextEntries[i].mColor;
				color.a = alpha;
				mTextSlots[i]->setColour(color);
			}

			// Move on to the next thing in the vector.
			i++;
		}
	}
	// Clear the text in all other elements (needed because we might have removed an element 
	// in the above loop)
	for (size_t i = mTextEntries.size(); i < mTextSlots.size(); i++)
		mTextSlots[i]->setCaption("");
}


void ssuge::TextMatrix::append(std::string t, Ogre::ColourValue c, float time, float fade_time)
{
	if (!mExpiringTextType)
		return;

	if (ssuge::contains_char(t, '\n'))
	{
		std::vector<std::string> elements;
		split_str(t, elements, '\n');
		for (std::string s : elements)
		{
			mTextEntries.push_back(TextEntry(s, c, time));
		}
	}
	else
	{
		mTextEntries.push_back(TextEntry(t, c, time));
	}
}

/// Updates the text / color for a static element
void ssuge::TextMatrix::update_text(std::string t, int index, Ogre::ColourValue col)
{
	if (mExpiringTextType || index < 0 || index >= mTextSlots.size())
		return;

	mTextSlots[index]->setCaption(t);
	mTextSlots[index]->setColour(col);
}


void ssuge::TextMatrix::set_visibility(bool is_visible)
{
	if (is_visible)
		mOverlay->show();
	else
		mOverlay->hide();
}


void ssuge::TextMatrix::set_background_material(std::string mat_name)
{
	mPanel->setMaterialName(mat_name);
}


bool ssuge::TextMatrix::get_visible()
{
	return mOverlay->isVisible();
}