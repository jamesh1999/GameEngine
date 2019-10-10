#include "Resource.h"
#include <fstream>
#include "ResourceTable.h"

using namespace GameEngine;
using namespace GameEngine::Resources;

void Resource::DestroyResource()
{
	GetEngine()->resources->Erase(m_identifier);
	Element::Destroy();
}

void Resource::Destroy()
{
	exit(-1);
}

Resource& Resource::operator>>(std::ostream& out)
{
	// Chain call base
	Element::operator>>(out);
	out << m_identifier << '\n';
	return *this;
}

// Read nothing this is the job of the ResourceFactory
Resource& Resource::operator<<(std::istream& in) { return *this; }

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
	*this >> out;
}