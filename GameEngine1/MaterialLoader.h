#ifndef __MATERIAL_LOADER_INCLUDED__
#define __MATERIAL_LOADER_INCLUDED__

#include "Material.h"
#include <string>

class MaterialLoader
{
public:
	static void Load(Material*, const std::string&) {}
};

#endif