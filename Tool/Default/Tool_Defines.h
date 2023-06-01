#pragma once

#include "Process.h"

#define MAX_CHAR 255

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVELID { LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern unsigned int   g_ResizeWidth, g_ResizeHeight;

using namespace Tool;