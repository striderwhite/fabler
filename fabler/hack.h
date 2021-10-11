#pragma once
#include <windows.h> // For standard windows API datatypes and functions
#include <iostream>	// For the console

DWORD WINAPI HackLoop(HMODULE hModule);
void InitializeConsole();
void CTCPhysicsNavigatorHook();
void InitD3D9EndsceneHook();
HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 pDevice);