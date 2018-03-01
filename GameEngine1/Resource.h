#ifndef __RESOURCE_INCLUDED__
#define __RESOURCE_INCLUDED__

#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class ResourceFactory;
		template <class TResource>
		class ResourceRef;

		class Resource
		{
			friend class ResourceFactory;
			template <class>
			friend class ResourceRef;

		private:
			unsigned m_refCount = 0;

		protected:
			Engine* engine = nullptr;
		};
	}
}

#endif