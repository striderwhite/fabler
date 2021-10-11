#include "stdafx.h"
#include "hack.h"
#include "structures.h"
#include "utils.h"
#include "offsets.h"
#include "gh_d3d9.h"
#include <sstream>

FILE* g_ConsoleHandle;

//	Holds the CTCPhysicsNavigator pointers
const int g_CTCPhysicsNavigatorArraySize = 255;
DWORD g_CTCPhysicsNavigators[g_CTCPhysicsNavigatorArraySize];
DWORD g_CTCPhysicsNavigatorinjectionCopyJmpBack;

//	Vars for the endscene hook
tEndScene oEndScene = nullptr;
LPDIRECT3DDEVICE9 pD3DDevice = nullptr;
void* d3d9Device[119];
bool bD3D9HookInit = false;
bool bDrawEsp = true;

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
	bool bInfiniteHealthAndMana = { true }; 
	uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle((LPCWSTR)L"fable.exe");

	// Grab instances of the objects we want
	CThingPlayerCreature* cThingPlayerCreature = (CThingPlayerCreature*)Dereferencer(moduleBaseAddress + g_CThingPlayerCreatureInitalOffset, g_CThingPlayerCreatureOffsets);
	CTCHeroStats* cCtcHeroStats = (CTCHeroStats*)Dereferencer(moduleBaseAddress + g_CTCHeroStatsInitalOffset, g_CTCHeroStatsOffsets);
	CTCPhysicsControlled* cCTCPhysicsControlled = (CTCPhysicsControlled*)Dereferencer(moduleBaseAddress + g_CTCPhysicsControlledInitalOffset, g_CTCPhysicsControlledOffsets);

	// Hook any functions we need
	DWORD targetHookFunctionAddress = moduleBaseAddress + g_CTCPhysicsNavigatorinjectionCopyOffset;	//	Calc the targeting function hooking address
	g_CTCPhysicsNavigatorinjectionCopyJmpBack = targetHookFunctionAddress + 5;	//	Calc the jump back address
	PlaceJMP((BYTE*)targetHookFunctionAddress, (DWORD)CTCPhysicsNavigatorHook); // Call subroutine to write the jmp shellcode

	// D3D9 Endscene hook
	InitD3D9EndsceneHook();

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

		//	Draw ESP Toggle (press U key)
		if (GetAsyncKeyState(0x55) & 1) {
			bDrawEsp = !bDrawEsp;
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
		// This is technically the "wrong" way to read GetAsyncKeyState
		// however, this actually causes some cool functionality which was initally not intended
		// if user holds down the "I" key, it actually causes the character to "jump" -- kind of cool!
		if (GetAsyncKeyState(0x49)) {
			//	"Teleport" hack
			float previousZ = cCTCPhysicsControlled->Position.z;
			cCTCPhysicsControlled->Position.z = previousZ + 0.3f;
		}

		// For debugging, write out all the addresses we found for CTCPhysicsNavigators
		//std::cout << "====================================" << std::endl;
		//std::cout << "CTCPhysicsNavigators:" << std::endl;
		for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
			if (g_CTCPhysicsNavigators[i] != NULL) {
				CTCPhysicsNavigator* phyNav = (CTCPhysicsNavigator*)g_CTCPhysicsNavigators[i];

				//	Filter out any results that are not the instance we are interested in.
				if (phyNav->vTablePtr != g_CTCPhysicsNavigatorVTableAddress) {
					std::cout << "The address no longer pointed to a valid VTable address, removing..." << std::endl;
					g_CTCPhysicsNavigators[i] = NULL;
				}
			}
		}

		int entryCounter = 0;
		for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
			if (g_CTCPhysicsNavigators[i] != NULL) {
				entryCounter++;
			}
		}

		std::cout << "Current Entries: " << entryCounter << std::endl;

		// To prevent CPU from thread pinning
		Sleep(5);
	}

	// Cleanup 
	fclose(g_ConsoleHandle);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

/// <summary>
/// Inits a debug console window
/// </summary>
void InitializeConsole() {
	AllocConsole();
	freopen_s(&g_ConsoleHandle, "CONOUT$", "w", stdout);
}

/// <summary>
/// A hook that reads CTCPhysicsNavigator addresses
/// </summary>
/// <remark>
/// Note: variables used in this function must be delcared outside its scope, as to not cause C2489 (below)
/// 'identifier' : initialized auto or register variable not allowed at function scope in 'naked' function
/// </remark>
DWORD ctcPhysicsNavigatorAddress = NULL;
int i; // iterator

__declspec(naked) void CTCPhysicsNavigatorHook() {

	//	Execute the stolen instructions and push all general purpose registers
	_asm {
		push esi
		mov esi, ecx
		mov eax, DWORD PTR [esi]
		pushad // Push all general purpose registers onto the stack as to not corrupt them during our operations below
	}

	//	Copy the ESI register into a variable -- ESI holds the address of a CTCPhysicsNavigator
	_asm {
		mov ctcPhysicsNavigatorAddress, esi //  Now ctcPhysicsNavigatorAddress should contain the address of a CTCPhysicsNavigator
	}

	//	If some reason the read failed, leave execution
	if (ctcPhysicsNavigatorAddress == NULL) {
		goto DONE;
	}

	//	Ignore this, as it does not point to a valid instance!
	if (((CTCPhysicsNavigator*)ctcPhysicsNavigatorAddress)->vTablePtr != g_CTCPhysicsNavigatorVTableAddress) {
		goto DONE;
	}


	//	See if the entry already exists, break execution if it does
	for (i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
		if (g_CTCPhysicsNavigators[i] == ctcPhysicsNavigatorAddress) {
			goto DONE;
		}
	}

	//	If an entry does not exist, push it there, break execution
	for (i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
		if (g_CTCPhysicsNavigators[i] == NULL) {
			g_CTCPhysicsNavigators[i] = ctcPhysicsNavigatorAddress;
			break;
		}
	}

	//	Cleanup and jmp back
DONE:
	_asm {
		popad // Restore all gp registers 
		jmp[g_CTCPhysicsNavigatorinjectionCopyJmpBack] // jmp back 
	}
}

/// <summary>
/// Inits the D3D9 Endscene Hook
/// This is done by create a "dummy" d3d9 device in order to get its vtable pointer
/// then "hooking" the vtable by overriding the pointer to our endscene hook (this function)
/// </summary>
void InitD3D9EndsceneHook() {
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
	{
		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}
}

/// <summary>
/// The primary hook for D3D9 responsible for drawing on screen
/// This functions is called by D3D9 endscene via the vtable, our hook overrode that pointer to here.
/// 
/// Afterwards we call the original endscene address, this is returned to us by the TrampHook function
/// 
/// This allows us to call the OG function after we do our stuff.
/// </summary>
/// <param name="pDevice"></param>
/// <returns></returns>
HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (bD3D9HookInit == false)
	{
		pD3DDevice = pDevice;
		bD3D9HookInit = true;
	}

	int entryCounter = 0;
	for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
		if (g_CTCPhysicsNavigators[i] != NULL) {
			entryCounter++;
		}
	}

	if (bDrawEsp) {

		//	Draw how many entries are in the array for debugging
		std::ostringstream oss;
		oss << entryCounter;
		DrawString(100, 100, (char*)oss.str().c_str(), D3DCOLOR_ARGB(255, 255, 0, 0), pDevice);


		char* nameStr = (char*)"XXX";
		float viewMatrix[16];

		//	Need to find the model view matrix address
		//uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle((LPCWSTR)L"fable.exe");
		//memcpy(&viewMatrix, (PBYTE)(moduleBaseAddress + g_ViewModelProjectionMatrixOffset), sizeof(viewMatrix));

		// 1 ? 013BC760 -- these are guesses
		memcpy(&viewMatrix, (PBYTE)0x013BC760, sizeof(viewMatrix));

		for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
			if (g_CTCPhysicsNavigators[i] != NULL) {
				Vector2 vScreen;
				CTCPhysicsNavigator* phyNav = (CTCPhysicsNavigator*)g_CTCPhysicsNavigators[i];
				if (WorldToScreen(phyNav->position, vScreen, viewMatrix, 800, 600)) {
					DrawString(vScreen.x, vScreen.y, nameStr, D3DCOLOR_ARGB(255, 0, 255, 0), pDevice);
				}
			}
		}

		// 2 ? 013BC7DC -- these are guesses
		memcpy(&viewMatrix, (PBYTE)0x013BC7DC, sizeof(viewMatrix));

		for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
			if (g_CTCPhysicsNavigators[i] != NULL) {
				Vector2 vScreen;
				CTCPhysicsNavigator* phyNav = (CTCPhysicsNavigator*)g_CTCPhysicsNavigators[i];
				if (WorldToScreen(phyNav->position, vScreen, viewMatrix, 800, 600)) {
					DrawString(vScreen.x, vScreen.y, nameStr, D3DCOLOR_ARGB(255, 0, 0, 255), pDevice);
				}
			}
		}

	}

	return oEndScene(pDevice);
}