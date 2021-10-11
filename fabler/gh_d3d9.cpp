#include "stdafx.h"
#include "gh_d3d9.h"
#include "structures.h"

#define M_PI 3.141f

ID3DXLine* LineL;
ID3DXFont* LFontL;
ID3DXFont* LespFontL;

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE; // skip to next window

	window = handle;
	return FALSE; // window found abort search
}

HWND GetProcessWindow()
{
	window = NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return window;
}

bool GetD3D9Device(void** pTable, size_t Size)
{
	if (!pTable)
		return false;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	IDirect3DDevice9* pDummyDevice = NULL;

	// options to create dummy device
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDeviceCreated != S_OK)
	{
		// may fail in windowed fullscreen mode, trying again with windowed mode
		d3dpp.Windowed = !d3dpp.Windowed;

		dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			pD3D->Release();
			return false;
		}
	}

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

	pDummyDevice->Release();
	pD3D->Release();
	return true;
}

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev)
{
	D3DRECT BarRect = { x, y, x + w, y + h };

	dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}


void DrawString(int x, int y, char* string, D3DCOLOR color, IDirect3DDevice9* dev)
{
	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	if (!LFontL) {
		D3DXCreateFont(dev, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &LFontL);
		D3DXCreateFont(dev, 13, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial", &LespFontL);
	}

	RECT Position;
	Position.left = x + 1;
	Position.top = y + 1;
	LFontL->DrawTextA(0, string, strlen(string), &Position, DT_NOCLIP, color);
	Position.left = x;
	Position.top = y;
	LFontL->DrawTextA(0, string, strlen(string), &Position, DT_NOCLIP, color);
}

void DrawRect(int x, int y, int width, int height, D3DCOLOR color, IDirect3DDevice9* dev)
{
	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	D3DXVECTOR2 Rect[5];
	Rect[0] = D3DXVECTOR2(x, y);
	Rect[1] = D3DXVECTOR2(x + width, y);
	Rect[2] = D3DXVECTOR2(x + width, y + height);
	Rect[3] = D3DXVECTOR2(x, y + height);
	Rect[4] = D3DXVECTOR2(x, y);
	LineL->SetWidth(1);
	LineL->Draw(Rect, 5, color);
}

void DrawBorderedRect(int x, int y, int width, int height, int fa, int fr, int fg, int fb, D3DCOLOR color, IDirect3DDevice9* dev)
{
	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	D3DXVECTOR2 Fill[2];
	Fill[0] = D3DXVECTOR2(x + width / 2, y);
	Fill[1] = D3DXVECTOR2(x + width / 2, y + height);
	LineL->SetWidth(width);
	LineL->Draw(Fill, 2, D3DCOLOR_ARGB(fa, fr, fg, fb));

	D3DXVECTOR2 Rect[5];
	Rect[0] = D3DXVECTOR2(x, y);
	Rect[1] = D3DXVECTOR2(x + width, y);
	Rect[2] = D3DXVECTOR2(x + width, y + height);
	Rect[3] = D3DXVECTOR2(x, y + height);
	Rect[4] = D3DXVECTOR2(x, y);
	LineL->SetWidth(1);
	LineL->Draw(Rect, 5, color);
}

void DrawLine(int x, int y, int x1, int y2, D3DCOLOR color, IDirect3DDevice9* dev)
{
	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x, y);
	Line[1] = D3DXVECTOR2(x1, y2);
	LineL->SetWidth(1);
	LineL->Draw(Line, 2, color);
}

//void DrawFilledRect(int x, int y, int width, int height, D3DCOLOR color, IDirect3DDevice9* dev)
//{
//	if (!LineL) {
//		D3DXCreateLine(dev, &LineL);
//	}
//
//	D3DXVECTOR2 Rect[2];
//	Rect[0] = D3DXVECTOR2(x + width / 2, y);
//	Rect[1] = D3DXVECTOR2(x + width / 2, y + height);
//	LineL->SetWidth(width);
//	LineL->Draw(Rect, 2, color);
//}

void DrawCircle(int x, int y, int radius, D3DCOLOR color, IDirect3DDevice9* dev)
{
	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	D3DXVECTOR2 DotPoints[128];
	D3DXVECTOR2 Line[128];
	int Points = 0;
	for (float i = 0; i < M_PI * 2.0f; i += 0.1f)
	{
		float PointOriginX = x + radius * cos(i);
		float PointOriginY = y + radius * sin(i);
		float PointOriginX2 = radius * cos(i + 0.1) + x;
		float PointOriginY2 = radius * sin(i + 0.1) + y;
		DotPoints[Points] = D3DXVECTOR2(PointOriginX, PointOriginY);
		DotPoints[Points + 1] = D3DXVECTOR2(PointOriginX2, PointOriginY2);
		Points += 2;
	}
	LineL->Draw(DotPoints, Points, color);
}

/// <summary>
/// Determines if the current positions is visible on the screen.
/// Writes to screen param x/y coordinates where it will appear on a 2d plane if in view.
/// </summary>
/// <param name="pos"></param>
/// <param name="screen"></param>
/// <param name="matrix"></param>
/// <param name="windowWidth"></param>
/// <param name="windowHeight"></param>
/// <returns></returns>
bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	Vector4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	//	Determines WHERE on the screen (x/y) this positions is (converting 3d space to 2d space on the screen)
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}