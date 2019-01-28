#ifndef __PROPERTY_DICT_INCLUDED__
#define __PROPERTY_DICT_INCLUDED__

#include <unordered_map>
#include <string>
#include "Component.h"

namespace GameEngine
{
	namespace Elements
	{
		class PropertyDict : public Component
		{
			std::unordered_map<std::string, std::string> m_properties;

		public:
			std::string GetProperty(const std::string&);
			bool HasProperty(const std::string&);
			void SetProperty(const std::string&, const std::string&);
		};
	}
}

#endif