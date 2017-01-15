/*#ifndef __MATERIAL_MANAGER_INCLUDED__
#define __MATERIAL_MANAGER_INCLUDED__

#include <vector>
#include "Material.h"

class MaterialManager
{
private:
	std::vector<Material*> materials;
	static MaterialManager* instance;

public:
	MaterialManager();
	~MaterialManager();

	static int AddMaterial(Material*);
	static void 
};

#endif*/