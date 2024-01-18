#pragma once
#include <cstdint>

namespace rage
{
	template <typename T>
	class atArray
	{
	public:
		T *mElements;
		uint16_t mCount;
		uint16_t mCapacity;
	};
}