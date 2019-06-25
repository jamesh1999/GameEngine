#ifndef __I_SERIALIZABLE_INCLUDED__
#define __I_SERIALIZABLE_INCLUDED__

namespace GameEngine
{
	namespace Utils
	{
		// Interface for all classes that can be serialized
		class ISerializable
		{
		public:
			virtual ISerializable& operator<<(std::istream&) = 0;
			virtual ISerializable& operator>>(std::ostream&) = 0;
		};
	}
}

#endif