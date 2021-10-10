#pragma once
#include <vector>


////////////////////////////////////////////////////////////
///		Known offsets for desired object instances
////////////////////////////////////////////////////////////

extern const int g_CThingPlayerCreatureInitalOffset = 0x00FB8A1C;
extern const std::vector<unsigned int> g_CThingPlayerCreatureOffsets = { 0x8c, 0x14, 0x0 };

extern const int g_CTCHeroStatsInitalOffset = 0x00FB8A1C; // Note the same address as g_CThingPlayerCreatureInitalOffset -- neat!
extern const std::vector<unsigned int> g_CTCHeroStatsOffsets = { 0x8c, 0x10, 0x44, 0x14, 0x0 }; // Some offsets are the same as above (0x8c), I wonder what [[fable.exe + 0x00FB8A1C] + 0x8c] points too?

extern const int g_CTCPhysicsControlledInitalOffset = 0x00FB8A1C; // Again the same address
extern const std::vector<unsigned int> g_CTCPhysicsControlledOffsets = { 0x8c, 0x14, 0x60, 0x0 };

////////////////////////////////////////////////////////////
///				Known VTable addresses
///		This is useful for comparsions when iterating 
///		structures during runtime to determine their type
////////////////////////////////////////////////////////////

extern const int g_CTCEnemyVTableAddress = 0x0126A47C;
extern const int g_CThingPlayerCreatureVTableAddress = 0x012457FC;
extern const int g_CTCHeroStatsVTableAddress = 0x0124F70C;
extern const int g_CTCPhysicsControlledVTableAddress = 0x0126616C;
extern const int g_CTCPhysicsNavigatorVTableAddress = 0x01266314;

////////////////////////////////////////////////////////////
///					Misc offsets from fable.exe
////////////////////////////////////////////////////////////

extern const int g_CTCPhysicsNavigatorinjectionCopyOffset = 0x2AFF43;
