#include "Resource.h"
#include <fstream>
#include "ResourceTable.h"

using namespace GameEngine::Resources;

void Resource::Destroy()
{
	engine->resources->Erase(m_identifier);
	delete this;
}

Resource& Resource::operator>>(std::ostream& out)
{
	// Output the resource type only
	out << typeid(*this).name() << '\n';
	return *this;
}

Resource& Resource::operator<<(std::istream& in)
{
	// Read in the resource type only
	std::string type;
	std::getline(in, type);
	return *this;
}

std::string Resource::GetIdentifier() const
{
	return m_identifier;
}

void Resource::Save(const std::string& filename)
{
	// Update resource name
	m_identifier = filename;

	// Serialize & save to new location
	std::ofstream out(filename);
	out << typeid(*this).name();
	*this >> out;
}