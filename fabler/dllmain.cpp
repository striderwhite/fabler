// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "hack.h"

/// <summary>
/// Called when the DLL is injected and initalized by the injector, creates a thread to init the hack loop
/// </summary>
/// <param name="hModule">The pointer to the newly injected DLL</param>
/// <param name="ul_reason_for_call"></param>
/// <param name="lpReserved"></param>
/// <returns></returns>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    // True when the dll is injected and attached to the process
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CloseHandle(
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HackLoop, hModule, NULL, NULL)
        );
    }

    return TRUE;
}

