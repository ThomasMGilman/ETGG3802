#pragma once
#include <Ogre.h>

namespace OgreEngine
{
	class GameObject;
	class Component
	{
	private:
		
		struct propertyData {
			type_info dataType;
			union data
			{
				std::string sVal;
				bool bVal;
				int iVal;
				float fVal;
				double dVal;
			};
		};

	protected:
		GameObject* Parent;
		//std::map<std::string, propertyData> mProperties;
		std::map<std::string, std::string> mStringProperties;
		std::map<std::string, bool> mBoolProperties;
		std::map<std::string, int> mIntProperties;
		std::map<std::string, float> mFloatProperties;
		std::map<std::string, double> mDoubleProperties;
	

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

		/*/// Append the property of type T to the property map container
		template<typename T>
		void add_xml_property(std::string name, T d)
		{
			if (mProperties.find(name) == mProperties.end())
			{
				
				//struct propertyData pData = {typeid(T), d};
				//memcpy_s((void*)&pData.val, sizeof(d), (void*)&d, sizeof(d));
				mProperties[name] = std::make_tuple(typeid(T), d);
			}
		};*/

		virtual void update(float elapsed) = NULL;

		virtual ComponentType get_type() = NULL;

		virtual void set_visible(bool isVisible) = NULL;
	};

	
}