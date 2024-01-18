#pragma once
#include "Base.h"
#include "Array.h"
#include "../Utils.h"

namespace rage
{
	template<typename T>
	class pgDictionary : pgBase
	{
	public:
		pgDictionary *mParent;
		int32_t mRefCount;
		atArray<uint32_t> mCodes;
		atArray<T> mEntries;

		T Lookup(uint32_t code)
		{
			static auto func = (T (__thiscall *)(pgDictionary *, uint32_t))Utils::ReadMemory(0x3A490);
			return func(this, code);
		}
	};
}