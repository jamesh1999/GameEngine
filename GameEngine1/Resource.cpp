#include "Resource.h"
#include "ResourceTable.h"

using namespace GameEngine::Resources;

void Resource::Destroy()
{
	engine->resources->resources.erase(m_identifier);
	delete this;
}
