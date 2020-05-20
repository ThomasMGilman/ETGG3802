#include <stdafx.h>
#include <utility.h>

using namespace OgreEngine;

bool OgreEngine::contains_char(const std::string& s, const char c)
{
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s[i] == c)
			return true;
	}
	return false;
}


void OgreEngine::split_str(const std::string& s, std::vector<std::string>& result, const char c)
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

float OgreEngine::magnitude(Ogre::Vector3 container, bool squared)
{
	float amount = pow(container.x, 2) + pow(container.y, 2) + pow(container.z, 2);
	return (squared ? amount : sqrt(amount));
}

void OgreEngine::normalized(Ogre::Vector3& dir)
{
	Ogre::Vector3 copy = dir;
	float mag = magnitude(dir);
	if (mag != 0)
	{
		for (int i = 0; i < 3; i++)
			copy[i] /= mag;
		dir = copy;
	}
}

Ogre::Vector3 OgreEngine::normalized(Ogre::Vector3 dir)
{
	Ogre::Vector3 copy = dir;
	float mag = magnitude(dir);
	if (mag == 0)
		return copy;

	for (int i = 0; i < 3; i++)
		copy[i] /= mag;
	return copy;
}
