#pragma once

#include "targetver.h"

#define _HAS_STD_BYTE 0

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <string.h>

#include <vector>
#include <list>
#include <map>
#include <stack>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "ImGuiFileDialog.h"

using namespace std;