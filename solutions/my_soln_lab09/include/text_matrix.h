#pragma once
#include <stdafx.h>

namespace ssuge
{
	class TextMatrix
	{
	protected:
		/// A convenient place to store all data related to the visual log entries.
		class TextEntry
		{
		public:
			std::string mText;
			Ogre::ColourValue mColor;

			// These 3 only apply if this TextMatrix does expring text
			float mTimeToLive;
			float mOrigTimeToLive;
			float mFadeTime;				// <0: don't fade; >= 0 when this much time to live, start to fade
			

			TextEntry(std::string text, Ogre::ColourValue color, float time_to_live, float fade_time = -1.0f);
			TextEntry(std::string text, Ogre::ColourValue color);
		};

		/// The Ogre overlay used to display on-screen text
		Ogre::Overlay* mOverlay;

		/// The main panel for the overlay
		Ogre::OverlayContainer* mPanel;

		/// The visual log entries we're currently showing -- only the first n (where n is the # of elements in mTextSlots) will show.
		std::vector<TextEntry> mTextEntries;

		/// The vector of text elements used for each line on the console text.  These are pre-created when the LogManager is created
		std::vector<Ogre::TextAreaOverlayElement*> mTextSlots;

		/// If this boolean is true, text elements are meant to be added to the matrix.  They'll gradually
		/// expire and be removed.  If false, text is just a series of static slots that are meant to 
		/// be set periodically.  In this case, mTextEntries is not used
		bool mExpiringTextType;

	public:
		/// Main constructor
		TextMatrix(std::string name, int num_lines, Ogre::Vector2 origin, Ogre::Vector2 dimensions, bool text_expires);

		/// Update
		void update(float dt);

		/// Add a new element to the end
		void append(std::string t, Ogre::ColourValue c, float time, float fade_time = -1.0f);

		/// Sets the text to a specific element.  If that list is too short, append empty strings
		void update_text(std::string t, int index, Ogre::ColourValue col);
		
		/// Sets the visibility of the overlay
		void set_visibility(bool is_visible);

		/// Sets the material to be used as the background material for this entire panel.  By default,
		/// this isn't set (which makes the background transparent).  Currently, I'm assuming the user
		/// has created a .material file (somewhere in their resource paths) -- see https://ogrecave.github.io/ogre/api/1.11/_material-_scripts.html
		void set_background_material(std::string mat_name);

		/// Gets the visibility of the overlay
		bool get_visible();
	};
}
