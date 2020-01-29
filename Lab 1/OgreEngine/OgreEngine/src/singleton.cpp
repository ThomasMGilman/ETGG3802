#include <singleton.h>

using namespace OgreEngine;

template<class T>
Singleton<T>::Singleton()
{
	if (msSingleton != nullptr) throw std::string("Singleton of class: " + typeid(T).name() + " already created");
	msSingleton = this;
}

template<class T>
static T* Singleton<T>::getSingletonPtr()
{
	if (msSingleton == nullptr) throw std::string("Singleton of class: " + typeid(T).name() + " has yet to be created");
	return msSingleton;
}