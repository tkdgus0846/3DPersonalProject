#pragma once

#include "targetver.h"

#define _HAS_STD_BYTE 0

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <windows.h>
// C ��Ÿ�� ��� �����Դϴ�.
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