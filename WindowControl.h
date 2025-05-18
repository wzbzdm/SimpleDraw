#pragma once

#include <Windows.h>
#include "windowState.h"

#define DEFAULTSYSTEMMODE	{ CoordinateMode }

typedef struct windowPos {
	int x;
	int y;
	int width;
	int height;
} WindowsPos;

typedef struct windowRect {
	RECT mainrect;
	RECT toolbarrect;
	WindowsPos smallrect;
	WindowsPos canvasrect;
	WindowsPos sidebarrect;
	WindowsPos statusbar;
} WindowRect;

typedef enum WorkType {
	CanvasMode,
	CoordinateMode,
	ThreeDMode
} WorkType;

typedef struct SYSTEMMODE {
	WorkType worktype;
} SYSTEMMODE;

typedef struct WindowHwnd {
	HINSTANCE hInst;                                // 当前实例
	HWND hWnd;									    // 主窗口句柄
	HWND hSmallWnd;                                 // 小窗口句柄
	HWND hSideWnd;									// 侧边栏句柄	
	HWND hCanvasWnd;                                // 画布窗口句柄
	HWND hToolBar;                                  // 工具栏句柄
	HWND hStatusBar;								// 状态栏句柄
} WindowHwnd;

typedef struct WindowControl {
	SYSTEMMODE mode;	// 当前模式
	WindowHwnd cdw;		// 控制窗口
} WindowControl;

POINT getClientPos(LPARAM lParam) {
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	return point;
}

POINT getClientPos(HWND hWnd) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);
	return point;
}