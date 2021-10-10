#include "utils.h"
#include <iostream>


/// <summary>
/// Dereferences a pointer chain
/// </summary>
/// <param name="base">the module base address + an offset</param>
/// <param name=""></param>
/// <returns></returns>
uintptr_t dereferencer(uintptr_t base, std::vector<unsigned int> offsets)
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
