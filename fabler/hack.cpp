#include "hack.h"
#include "structures.h"
#include "utils.h"
#include "offsets.h"

FILE* g_ConsoleHandle;

//	Holds the CTCPhysicsNavigator pointers
const int g_CTCPhysicsNavigatorArraySize = 255;
DWORD g_CTCPhysicsNavigators[g_CTCPhysicsNavigatorArraySize];
DWORD g_CTCPhysicsNavigatorinjectionCopyJmpBack;

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

	// Hook any functions we need
	DWORD targetHookFunctionAddress = moduleBaseAddress + g_CTCPhysicsNavigatorinjectionCopyOffset;	//	Calc the targeting function hooking address
	g_CTCPhysicsNavigatorinjectionCopyJmpBack = targetHookFunctionAddress + 5;	//	Calc the jump back address
	PlaceJMP((BYTE*)targetHookFunctionAddress, (DWORD)CTCPhysicsNavigatorHook); // Call subroutine to write the jmp shellcode

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
		// This is technically the "wrong" way to read GetAsyncKeyState
		// however, this actually causes some cool functionality which was initally not intended
		// if user holds down the "I" key, it actually causes the character to "jump" -- kind of cool!
		if (GetAsyncKeyState(0x49)) {
			//	"Teleport" hack
			float previousZ = cCTCPhysicsControlled->Position.z;
			cCTCPhysicsControlled->Position.z = previousZ + 0.3f;
		}

		// For debugging, write out all the addresses we found for CTCPhysicsNavigators
		std::cout << "====================================" << std::endl;
		std::cout << "CTCPhysicsNavigators:" << std::endl;
		for (int i = 0; i < g_CTCPhysicsNavigatorArraySize; i++) {
			if (g_CTCPhysicsNavigators[i] != NULL) {
				std::cout << "0x" << std::hex << g_CTCPhysicsNavigators[i] << std::endl;
			}
		}

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

/*Credits to InSaNe on MPGH for the original function*/
//We make Length at the end optional as most jumps will be 5 or less bytes
void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length)
{
	DWORD dwOldProtect, dwBkup, dwRelAddr;

	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(Address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
	dwRelAddr = (DWORD)(jumpTo - (DWORD)Address) - 5;

	// Write the JMP opcode @ our jump position...
	*Address = 0xE9;

	// Write the offset to where we're gonna jump
	//The instruction will then become JMP ff002123 for example
	*((DWORD*)(Address + 0x1)) = dwRelAddr;

	// Overwrite the rest of the bytes with NOPs
	//ensuring no instruction is Half overwritten(To prevent any crashes)
	for (DWORD x = 0x5; x < length; x++)
		*(Address + x) = 0x90;

	// Restore the default permissions
	VirtualProtect(Address, length, dwOldProtect, &dwBkup);
}