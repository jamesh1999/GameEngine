#include "PropertyDict.h"

using namespace GameEngine::Elements;

std::string PropertyDict::GetProperty(const std::string& key)
{
	return m_properties[key];
}

bool PropertyDict::HasProperty(const std::string& key)
{
	return m_properties.find(key) != m_properties.end();
}

void PropertyDict::SetProperty(const std::string& key, const std::string& value)
{
	m_properties[key] = value;
}
