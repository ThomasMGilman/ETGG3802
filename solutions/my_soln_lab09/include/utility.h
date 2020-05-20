#pragma once
#include <stdafx.h>

namespace ssuge
{
	/// Returns true if the given string has any occurrences of the given char
	bool contains_char(const std::string& s, const char c);

	/// Fills in the given vector (replacing the contents) where the given
	/// string is broken into sub-strings based on char c (if none given, break on
	/// whitespace)
	void split_str(const std::string& s, std::vector<std::string>& result, const char c = '\0');
}
