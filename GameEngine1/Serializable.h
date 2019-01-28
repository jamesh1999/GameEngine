#ifndef __SERIALIZABLE_INCLUDED__
#define __SERIALIZABLE_INCLUDED__

#include <istream>
#include <ostream>

namespace GameEngine
{
	namespace Utils
	{
		class Serializable
		{
			virtual std::istream& operator>>(std::istream&) = 0;
		};
	}
}

#endif