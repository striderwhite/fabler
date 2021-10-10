#pragma once
#include <windows.h> // For standard windows API datatypes and functions
#include <iostream>	// For the console

DWORD WINAPI HackLoop(HMODULE hModule);
void InitializeConsole();
void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length = 5);
void CTCPhysicsNavigatorHook();