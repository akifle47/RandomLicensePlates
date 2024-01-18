#pragma once
#include "StringHash.h"
#include "Dictionary.h"
#include "Texture.h"
#include "../Utils.h"

namespace rage
{
	using CTxd = pgDictionary<grcTexturePC*>;

	class CTxdStore
	{
	public:
		static int32_t AddSlot(const char *name)
		{
			static auto func = (int32_t(__cdecl *)(const char *))Utils::ReadMemory(0x8EBC0);
			return func(name);
		}

		static int32_t FindSlot(uint32_t hash)
		{
			static auto func = (int32_t(__cdecl *)(uint32_t))Utils::ReadMemory(0x8F0B0);
			return func(hash);
		}

		static int32_t FindSlot(const char *name)
		{
			return FindSlot(atStringHash(name, 0));
		}

		static CTxd *GetSlot(uint32_t slot)
		{
			static auto func = (CTxd *(__cdecl *)(uint32_t))Utils::ReadMemory(0xA3B50);
			return func(slot);
		}
	};
}