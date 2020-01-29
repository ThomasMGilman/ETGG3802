#pragma once

#include <string>

namespace OgreEngine 
{
	template <class T>
	class Singleton
	{
	protected:
		static T* msSingleton;
	private:

	public:
		Singleton();

		static T* getSingletonPtr();
	};
}