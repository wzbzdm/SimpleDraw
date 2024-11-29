#pragma once

// TODO: 工作区

#include "drawinfo.h"
#include "windowState.h"
#include <vector>

using namespace std;

#define MAX_LOADSTRING	100

typedef struct GlobalConfig {
	HDC hdcMemFixed;			// 固定图像内存DC
	HDC hdcMemPreview;			// 预览图像内存DC
	HDC hdcMemCoS;				// 计算或选中图像内存DC
	HBITMAP hbmMemFixed;		// 固定图像位图
	HBITMAP hbmMemPreview;		// 预览图像位图
	HBITMAP hbmOldFixed;		// 原固定位图
	HBITMAP hbmOldPreview;		// 原预览位图
	HBITMAP hbmmemCoS;			// 计算或选中图像位图
	HBITMAP hbmOldCoS;			// 原计算或选中位图

	// 全局变量:
	HINSTANCE hInst;                                // 当前实例
	HWND hWnd;									    // 主窗口句柄
	HWND hSmallWnd;                                 // 小窗口句柄
	HWND hSideWnd;									// 侧边栏句柄	
	HWND hCanvasWnd;                                // 画布窗口句柄
	HWND hToolBar;                                  // 工具栏句柄
	HWND hStatusBar;								// 状态栏句柄
	WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
	WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

	// 侧边栏控件
	HWND Edit1, Edit2;
	HWND Button;
	HWND bgColorSlider, ColorSlider, WidthSlider, TypeCombo;			// 新增控件变量

	// 全局变量用于保存光标的原始样式
	HCURSOR originalCursor;
	HCURSOR customCursor;		// 全局变量保存自定义光标句柄
	HCURSOR chooseCursor;		// 选择光标,十字光标表示可以
	HCURSOR moveCursor;			// 移动光标,四向箭头表示可以移动
} GlobalConfig;


typedef struct DWorkSpace {
	SYSTEMMODE systemode = DEFAULTSYSTEMMODE;
	WindowState wstate = { 800, 650, 45 };
	MyDrawState mst;								// 默认状态
	Coordinate coordinate;							// 坐标系
	StoreImg allImg;								// 存储所有的图形
	DrawingInfo drawing;							// 当前正在绘制的图形
	DrawUnitProperty customProperty;				// 自定义绘图
	WindowRect wrect;								// 各个组件的位置
	ChooseState cs;									// 工具栏状态维护
	CSDrawInfo csdraw;								// 被选中的图元
	CSDrawInfoRect	csdrect;						// 被选中区域
} DWorkSpace;

class DrawWorkSpace {
private:
	DWorkSpace workspace;
	vector<DWorkSpace> worklist;

public:
};
