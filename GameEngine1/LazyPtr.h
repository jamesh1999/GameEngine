#ifndef __LAZY_PTR_INCLUDED__
#define __LAZY_PTR_INCLUDED__

#include "Engine.h"

namespace GameEngine
{
	namespace Utils
	{
		template <class TPointer, class TElement>
		class LazyPtr : public TPointer<TElement>
		{
		private:
			Engine* engine;

			mutable bool m_ready = false;

		public:

			TElement* Get() const
			{
				if (m_ready) return TPointer<TElement>::Get();

				*this = engine->elements[m_identifier];

				TElement* ret = TPointer<TElement>::Get();
				m_ready = ret != nullptr;

				return ret;
			}
		};
	}
}

#endif