#ifndef __RESOURCE_INCLUDED__
#define __RESOURCE_INCLUDED__

#include <string>
#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class ResourceFactory;
		template <class TResource>
		class ResourcePtr;

		class Resource
		{
			friend class ResourceFactory;
			template <class>
			friend class ResourcePtr;

		private:
			unsigned m_refCount = 0;

			// Should only be accessible through ResourceRef<> for clean up!!
			void Destroy();

		protected:
			Engine* engine = nullptr;
			std::string m_identifier;

			virtual Resource* CloneResource() = 0;

		public:

			virtual ~Resource() { }

			std::string GetIdentifier() const
			{
				return m_identifier;
			}
		};
	}
}

#endif
