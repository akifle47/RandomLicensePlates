#pragma once
#include "../Utils.h"

namespace rage
{
	uint32_t atStringHash(const char *string, uint32_t seed)
	{
		auto func = (uint32_t(__cdecl *)(const char *, uint32_t))Utils::ReadMemory(0x5CF50);
		return func(string, seed);
	}
}