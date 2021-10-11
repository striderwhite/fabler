#pragma once
#include <vector>
#include <windows.h> // For standard windows API datatypes and functions

uintptr_t Dereferencer(uintptr_t base, std::vector<unsigned int>);
void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length = 5);