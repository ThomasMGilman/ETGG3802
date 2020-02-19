#pragma once
#include <Ogre.h>

namespace OgreEngine
{
	class GameObject;
	class Component
	{
	protected:
		GameObject* Parent;
		std::map<std::string, std::string> mStringProperties;
		std::map<std::string, bool> mBoolProperties;
		std::map<std::string, int> mIntProperties;
		std::map<std::string, float> mFloatProperties;
		std::map<std::string, double> mDoubleProperties;
	private:

	public:
		enum class ComponentType { CAMERA, LIGHT, MESH };

		Component(GameObject* owner) { this->Parent = owner; };

		~Component() {};

		void add_xml_string_property(std::string name, std::string data)
		{
			if (mStringProperties.find(name) == mStringProperties.end())
				mStringProperties[name] = data;
		}

		void add_xml_bool_property(std::string name, bool data)
		{
			if (mBoolProperties.find(name) == mBoolProperties.end())
				mBoolProperties[name] = data;
		}

		void add_xml_int_property(std::string name, int data)
		{
			if (mIntProperties.find(name) == mIntProperties.end())
				mIntProperties[name] = data;
		}

		void add_xml_float_property(std::string name, float data)
		{
			if (mFloatProperties.find(name) == mFloatProperties.end())
				mFloatProperties[name] = data;
		}

		void add_xml_double_property(std::string name, double data)
		{
			if (mDoubleProperties.find(name) == mDoubleProperties.end())
				mDoubleProperties[name] = data;
		}

		/*/// Append the property of type T to its appropriate map container
		template<typename T>
		void add_xml_property(std::string name, T data)
		{
			type_info ti = typeid(T);
			if (ti.hash_code() == typeid(std::string).hash_code())
			{
				if (mStringProperties.find(name) == mStringProperties.end())
					mStringProperties[name] = reinterpret_cast<std::string>(data);
			}
			else if (ti.hash_code() == typeid(bool).hash_code())
			{
				if (mBoolProperties.find(name) == mBoolProperties.end())
					mBoolProperties[name] = reinterpret_cast<bool>(data);
			}
			else if (ti.hash_code() == typeid(int).hash_code())
			{
				if (mIntProperties.find(name) == mIntProperties.end())
					mIntProperties[name] = reinterpret_cast<int>(data);
			}
			else if (ti.hash_code() == typeid(float).hash_code())
			{
				if (mFloatProperties.find(name) == mFloatProperties.end())
					mFloatProperties[name] = reinterpret_cast<float>(data);
			}
			else if (ti.hash_code() == typeid(double).hash_code())
			{
				if (mDoubleProperties.find(name) == mDoubleProperties.end())
					mDoubleProperties[name] = reinterpret_cast<double>(data);
			}
			else
				throw new std::exception(("TYPE ERROR!!! Did not provide valid data type given to append to mapped properties!!\n\tGot Type: " + std::string(typeid(T).name())).c_str());
		};*/

		virtual void update(float elapsed) = NULL;

		virtual ComponentType get_type() = NULL;

		virtual void set_visible(bool isVisible) = NULL;
	};

	
}