#include <stdafx.h>
#include <utility.h>

bool ssuge::contains_char(const std::string& s, const char c)
{
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s[i] == c)
			return true;
	}
	return false;
}


void ssuge::split_str(const std::string& s, std::vector<std::string>& result, const char c)
{
	result.clear();
	std::string partial;
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if ((c == '\0' && isspace(s[i])) || s[i] == c)
		{
			result.push_back(partial);
			partial = "";
		}
		else
			partial += s[i];
	}
	if (partial.size() > 0)
		result.push_back(partial);
}