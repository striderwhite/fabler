#pragma once
#include <cstdint>

//	Standard vec3 struct
struct Vector3 {
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

struct Vector2
{
	int x, y;
};

struct Matrix4x4 {
	float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4, w1, w2, w3, w4;
};

class CTCHeroStats
{
public:
	char pad_0000[60]; //0x0000
	int32_t gold; //0x003C
	int32_t highest_amount_of_gold; //0x0040
	char pad_0044[4]; //0x0044
	int32_t total_gold_aquired; //0x0048
	int32_t total_gold_spent; //0x004C
	char pad_0050[8]; //0x0050
	int32_t current_will; //0x0058
	int32_t max_will; //0x005C
	char pad_0060[16]; //0x0060
	int32_t renown; //0x0070
	char pad_0074[136]; //0x0074
	int32_t total_fines; //0x00FC
}; //Size: 0x0100
static_assert(sizeof(CTCHeroStats) == 0x100, "Error");

//	Holds health for the player
class CThingPlayerCreature
{
public:
	char pad_0004[176]; //0x0004
	float max_health; //0x00B0
	float current_health; //0x00B4
}; //Size: 0xB8
static_assert(sizeof(CThingPlayerCreature) == 0xB8, "Error");

//	holds XYZ for the player
class CTCPhysicsControlled
{
public:
	char pad_0000[12]; //0x0000
	Vector3 Position; //0x000C
}; //Size: 0x0018
static_assert(sizeof(CTCPhysicsControlled) == 0x18, "Error");


//	Holds XYZ information for the AI navigation components
class CTCPhysicsNavigator
{
public:
	int32_t vTablePtr; //0x0000
	char pad_0000[8]; //0x0004
	Vector3 position; //0x000C
	Vector3 rotation; //0x0018
}; //Size: 0x0024
static_assert(sizeof(CTCPhysicsNavigator) == 0x24, "Error");

class CGameCameraManager
{
public:
	int32_t vTablePtr; //0x0000
	Vector3 cameraPosition; //0x0004
	char pad_0010[8]; //0x0010
	Vector3 viewX; //0x0018
	Vector3 viewY; //0x0024
	char pad_0030[20]; //0x0030
	Vector3 viewZ; //0x0044
	Vector3 viewW; //0x0050
}; //Size: 0x005C
static_assert(sizeof(CGameCameraManager) == 0x5C, "Error");