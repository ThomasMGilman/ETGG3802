#pragma once

#include <string>
#include <exception>

namespace OgreEngine 
{
	template <class T>
	class Singleton
	{
	protected:
		static T* msSingleton;
	private:

		class BadInitException : virtual public std::exception
		{
		protected:
		public:
			virtual const char* what() const throw()
			{
				return (std::string("Singleton of class: ") + typeid(T).name() + std::string(" already created")).c_str();
			};
		};

		class BadUsageException : virtual public std::exception
		{
		protected:
		public:
			virtual const char* what() const throw()
			{
				return (std::string("Singleton of class: ") + typeid(T).name() + std::string(" has yet to be created")).c_str();
			};
		};

	public:
		Singleton()
		{
			if (msSingleton != nullptr) throw BadInitException();
			msSingleton = (T*)this;
		};

		static T* getSingletonPtr()
		{
			if (msSingleton == nullptr) throw BadUsageException();
			return msSingleton;
		};
	};
}