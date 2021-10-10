#include "hack.h"
#include "structures.h"
#include "utils.h"
#include "offsets.h"

FILE* ConsoleHandle;


/// <summary>
///	Controls the main hook loop thread
/// 
/// 
/// Usage:
/// "P" Key - Eject the DLL
/// "O" Key - Enable Inf Health
/// "I" Key - Teleports the player up into the air, like a "jump"
/// 
/// </summary>
DWORD WINAPI HackLoop(HMODULE hModule) {

	// Create Console and indicate we are injected
	InitializeConsole();
	std::cout << "Fabler is injected" << std::endl;

	// Declare and define our variables
	bool bInfiniteHealthAndMana = { false }; 
	uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle((LPCWSTR)L"fable.exe");

	// Grab instances of the objects we want
	CThingPlayerCreature* cThingPlayerCreature = (CThingPlayerCreature*)dereferencer(moduleBaseAddress + g_CThingPlayerCreatureInitalOffset, g_CThingPlayerCreatureOffsets);
	CTCHeroStats* cCtcHeroStats = (CTCHeroStats*)dereferencer(moduleBaseAddress + g_CTCHeroStatsInitalOffset, g_CTCHeroStatsOffsets);
	CTCPhysicsControlled* cCTCPhysicsControlled = (CTCPhysicsControlled*)dereferencer(moduleBaseAddress + g_CTCPhysicsControlledInitalOffset, g_CTCPhysicsControlledOffsets);

	std::cout << "===============================================" << std::endl;
	std::cout << "Found addresses: " << std::endl;
	std::cout << "cThingPlayerCreature: " << std::hex << cThingPlayerCreature << std::endl;
	std::cout << "cCtcHeroStats: " << std::hex << cCtcHeroStats << std::endl;
	std::cout << "cCTCPhysicsControlled: " << std::hex << cCTCPhysicsControlled << std::endl;
	std::cout << "===============================================" << std::endl;

	while(true) {


		//////////////////////////////////////////
		///				Toggles
		//////////////////////////////////////////

		//	Abort (press P key)
		if (GetAsyncKeyState(0x50) & 1) {
			break; // break the execution
		}

		// Toggle Inf Health + Mana (press O key)
		if (GetAsyncKeyState(0x4F) & 1) {
			bInfiniteHealthAndMana = !bInfiniteHealthAndMana;
			bInfiniteHealthAndMana ? std::cout << "Infinite Health and Mana Active" << std::endl : std::cout << "Infinite Health and Mana Deactivated" << std::endl;
		}


		//////////////////////////////////////////
		///				Hack Executions
		//////////////////////////////////////////

		//	Keep writing to the address (Freeze hack)
		if (bInfiniteHealthAndMana) {
			cThingPlayerCreature->current_health = 500;
			cThingPlayerCreature->max_health = 500;
			cCtcHeroStats->current_will = 15000;
			cCtcHeroStats->max_will = 15000;
		}

		// (press I key)
		if (GetAsyncKeyState(0x49) & 1) {
			//	"Teleport" hack
			float previousZ = cCTCPhysicsControlled->Position.z;
			cCTCPhysicsControlled->Position.z = previousZ + 10.0f;

		}

		// To prevent CPU from thread pinning
		Sleep(5);
	}

	// Cleanup 
	fclose(ConsoleHandle);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

/// <summary>
/// Inits a debug console window
/// </summary>
void InitializeConsole() {
	AllocConsole();
	freopen_s(&ConsoleHandle, "CONOUT$", "w", stdout);
}
