#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <commdlg.h>
#include <objidl.h>   // 包含IStream的定义
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
