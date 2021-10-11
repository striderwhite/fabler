#include "stdafx.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "mem.h"
#include "structures.h";

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);

static HWND window;

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

HWND GetProcessWindow();

bool GetD3D9Device(void** pTable, size_t Size);

bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev);
void DrawString(int x, int y, char* string, D3DCOLOR color, IDirect3DDevice9* dev);
void DrawBorderedRect(int x, int y, int width, int height, int fa, int fr, int fg, int fb, D3DCOLOR color, IDirect3DDevice9* dev);
void DrawLine(int x, int y, int x1, int y2, D3DCOLOR color, IDirect3DDevice9* dev);
void DrawFilledRect(int x, int y, int width, int height, D3DCOLOR color, IDirect3DDevice9* dev);
void DrawCircle(int x, int y, int radius, D3DCOLOR color, IDirect3DDevice9* dev);