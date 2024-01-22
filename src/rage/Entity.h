#pragma once
#include <cstdint>

class CEntity
{
private:
	uint8_t mDontCare0[44];

public:
	uint16_t mRandomSeed;

private: 
	uint8_t mDontCare2E[66];
};

static_assert(sizeof(CEntity) == 0x70);