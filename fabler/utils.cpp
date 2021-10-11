#include "stdafx.h"
#include "utils.h"
#include <iostream>


/// <summary>
/// Dereferences a pointer chain
/// </summary>
/// <param name="base">the module base address + an offset</param>
/// <param name=""></param>
/// <returns></returns>
uintptr_t Dereferencer(uintptr_t base, std::vector<unsigned int> offsets)
{
	uintptr_t lookupAddress = base;

	//	Walk the chain
	for (unsigned int i = 0; i < offsets.size(); i++) {
		// Performs a multiple indirection lookup:
		// 1. Cast lookupAddress as a pointer to a pointer (multiple indirection), then dereference it
		lookupAddress = *(uintptr_t*)lookupAddress;
		// 2. Now add the offset 
		lookupAddress += offsets[i];
	}

	return lookupAddress;
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