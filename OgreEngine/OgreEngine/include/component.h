#pragma once
#include <stdafx.h>

namespace OgreEngine
{
	
	class GameObject;
	class Component
	{
		
	private:
		using data = std::variant<std::string, bool, int, float, double>;
		struct propertyData {
			const std::type_info* dataType;
			data d;
		};

	protected:
		GameObject* mParent;
		std::map<std::string, propertyData> mProperties;

	public:
		enum class ComponentType { CAMERA, LIGHT, MESH };

		Component(GameObject* owner) { this->mParent = owner; };

		~Component() {};

		/// Append the property of type T to the property map container
		template<typename T>
		void add_xml_property(std::string name, T d)
		{
			if (mProperties.find(name) == mProperties.end())
				mProperties[name] = { &typeid(T), d };
		};

		virtual void update(float elapsed) = NULL;

		virtual ComponentType get_type() = NULL;

		virtual void set_visible(bool isVisible) = NULL;
	};

	
}