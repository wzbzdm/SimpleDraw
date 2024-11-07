#include "pch.h"
#include "framework.h"
#include "SimpleDraw.h"
#include "drawinfo.h"
#include "calculateImg.h"
#include "customSlider.h"

#define MAX_LOADSTRING 100

// 定时器
#define REDRAW 1
#define REDRAW_INTERVAL 8 // 8毫秒
bool needRedraw = false;
#define NeedRedraw()	(needRedraw = true)
#define Redraw()		(needRedraw = false)

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

int WindowMode = 2;			// 1: 画布模式 2: 坐标系模式

HDC hdcMemFixed;			// 固定图像内存DC
HDC hdcMemPreview;			// 预览图像内存DC
HBITMAP hbmMemFixed;		// 固定图像位图
HBITMAP hbmMemPreview;		// 预览图像位图
HBITMAP hbmOldFixed;		// 原固定位图
HBITMAP hbmOldPreview;		// 原预览位图
RECT canvasRect;			// 画布矩形

// 全局变量用于保存光标的原始样式
HCURSOR originalCursor;
HCURSOR customCursor;		// 全局变量保存自定义光标句柄
HCURSOR chooseCursor;		// 选择光标,十字光标表示可以
HCURSOR moveCursor;			// 移动光标,四向箭头表示可以移动

DrawUnitProperty customProperty;	// 自定义绘图
WindowRect wrect;
ChooseState cs;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                InitSmallInstance(HINSTANCE, int);
BOOL				InitSideInstance(HINSTANCE, int);
BOOL                InitStatusInstance(HINSTANCE, int);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SmallWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SideWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    CanvasWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    StatusWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void				FitCanvasCoordinate(Coordinate& coor, StoreImg& img, HWND hcwnd);
void				InitializeBuffers(HWND hWnd);
void				Cleanup();
void				RedrawFixedContent(HWND hCWnd, HDC hdc);
void				ClearPreviewContent(HDC hdc);
void				InitGDIPlus();
void				EnableMouseTracking(HWND hWnd);
void				ShutdownGDIPlus();
void 				LoadMyCustomCuser();
void				ShowAllCalPoint(HDC hdc);
void				CalAndShowPoint();
POINT				GetEditPoint(HWND edit1, HWND edit2);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SIMPLEDRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	InitMyDrawState(mst); // 初始化绘图状态
	// 初始化 GDI+
	InitGDIPlus();
	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEDRAW));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 关闭 GDI+
	ShutdownGDIPlus();

	return (int)msg.wParam;
}

//  目标: 注册窗口类。
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEDRAW));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	HBRUSH hBrushMain = CreateSolidBrush(MAINCOLOR); // 创建白色画刷
	wcex.hbrBackground = hBrushMain;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEDRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex); // 注册主窗口类

	// 注册小窗口类
	WNDCLASSEXW smallWndClass;
	smallWndClass.cbSize = sizeof(WNDCLASSEX);
	smallWndClass.style = CS_HREDRAW | CS_VREDRAW;
	smallWndClass.lpfnWndProc = SmallWndProc; // 小窗口的窗口过程
	smallWndClass.cbClsExtra = 0;
	smallWndClass.cbWndExtra = 0;
	smallWndClass.hInstance = hInstance;
	smallWndClass.hIcon = nullptr; // 不需要图标
	smallWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	HBRUSH hBrushSmall = CreateSolidBrush(SMALLCOLOR); // 创建白色画刷
	smallWndClass.hbrBackground = hBrushSmall;
	smallWndClass.lpszMenuName = nullptr;
	smallWndClass.lpszClassName = L"SmallWindowClass"; // 小窗口类名
	smallWndClass.hIconSm = nullptr;

	RegisterClassExW(&smallWndClass); // 注册小窗口类

	// 注册画布类
	WNDCLASSEXW canvasWndClass;
	canvasWndClass.cbSize = sizeof(WNDCLASSEX); // 使用正确的结构体
	canvasWndClass.style = CS_HREDRAW | CS_VREDRAW;
	canvasWndClass.lpfnWndProc = CanvasWndProc;
	canvasWndClass.cbClsExtra = 0;
	canvasWndClass.cbWndExtra = 0;
	canvasWndClass.hInstance = hInstance;
	canvasWndClass.hIcon = nullptr; // 不需要图标
	canvasWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	HBRUSH hBrushCanvas = CreateSolidBrush(CANVASCOLOR); // 创建白色画刷
	canvasWndClass.hbrBackground = hBrushCanvas;
	canvasWndClass.lpszMenuName = nullptr;
	canvasWndClass.lpszClassName = L"CanvasWindowClass";
	canvasWndClass.hIconSm = nullptr;

	RegisterClassExW(&canvasWndClass); // 注册画布类

	// 注册状态栏类
	WNDCLASSEXW statusWndClass;
	statusWndClass.cbSize = sizeof(WNDCLASSEX); // 使用正确的结构体
	statusWndClass.style = CS_HREDRAW | CS_VREDRAW;
	statusWndClass.lpfnWndProc = StatusWndProc;
	statusWndClass.cbClsExtra = 0;
	statusWndClass.cbWndExtra = 0;
	statusWndClass.hInstance = hInstance;
	statusWndClass.hIcon = nullptr; // 不需要图标
	statusWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	HBRUSH hBrushStatus = CreateSolidBrush(STATUSBARCOLOR);
	statusWndClass.hbrBackground = hBrushStatus;
	statusWndClass.lpszMenuName = nullptr;
	statusWndClass.lpszClassName = L"StatusWindowClass";
	statusWndClass.hIconSm = nullptr;

	RegisterClassExW(&statusWndClass); // 注册状态栏类

	// 注册侧边栏类
	WNDCLASSEXW sideWndClass;
	sideWndClass.cbSize = sizeof(WNDCLASSEX); // 使用正确的结构体
	sideWndClass.style = CS_HREDRAW | CS_VREDRAW;
	sideWndClass.lpfnWndProc = SideWndProc;
	sideWndClass.cbClsExtra = 0;
	sideWndClass.cbWndExtra = 0;
	sideWndClass.hInstance = hInstance;
	sideWndClass.hIcon = nullptr; // 不需要图标
	sideWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	HBRUSH hBrushSide = CreateSolidBrush(SIDEBARCOLOR);
	sideWndClass.hbrBackground = hBrushSide;
	sideWndClass.lpszMenuName = nullptr;
	sideWndClass.lpszClassName = L"SideWindowClass";
	sideWndClass.hIconSm = nullptr;

	RegisterClassExW(&sideWndClass);

	// 注册自定义滑块类
	WNDCLASSEXW customSliderWndClass;
	ZeroMemory(&customSliderWndClass, sizeof(WNDCLASSEXW)); // 清空结构体
	customSliderWndClass.cbSize = sizeof(WNDCLASSEXW);
	customSliderWndClass.style = CS_HREDRAW | CS_VREDRAW;
	customSliderWndClass.lpfnWndProc = SliderWndProc; // 确保这个指针有效
	customSliderWndClass.cbClsExtra = 0;
	customSliderWndClass.cbWndExtra = 0;
	customSliderWndClass.hInstance = hInstance; // 应该是有效的实例句柄
	customSliderWndClass.hIcon = nullptr; // 不需要图标
	customSliderWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	customSliderWndClass.hbrBackground = hBrushSide; // 确保这个画刷是有效的
	customSliderWndClass.lpszMenuName = nullptr; // 如果不需要菜单
	customSliderWndClass.lpszClassName = L"CustomSliderClass"; // 确保这个名字是唯一的
	customSliderWndClass.hIconSm = nullptr; // 可选

	RegisterClassExW(&customSliderWndClass); // 注册侧边栏类

	// 注册颜色窗口类
	WNDCLASSEXW customColorWndClass;
	ZeroMemory(&customColorWndClass, sizeof(WNDCLASSEXW)); // 清空结构体
	customColorWndClass.cbSize = sizeof(WNDCLASSEXW);
	customColorWndClass.style = CS_HREDRAW | CS_VREDRAW;
	customColorWndClass.lpfnWndProc = ColorPickerProc; // 确保这个指针有效
	customColorWndClass.cbClsExtra = 0;
	customColorWndClass.cbWndExtra = 0;
	customColorWndClass.hInstance = hInstance; // 应该是有效的实例句柄
	customColorWndClass.hIcon = nullptr; // 不需要图标
	customColorWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	customColorWndClass.hbrBackground = hBrushSide; // 确保这个画刷是有效的
	customColorWndClass.lpszMenuName = nullptr; // 如果不需要菜单
	customColorWndClass.lpszClassName = L"ColorPickerClass"; // 确保这个名字是唯一的
	customColorWndClass.hIconSm = nullptr; // 可选

	RegisterClassExW(&customColorWndClass); // 注册侧边栏类

	// 注册类型选择窗口类
	WNDCLASSEXW customTypeWndClass;
	ZeroMemory(&customTypeWndClass, sizeof(WNDCLASSEXW)); // 清空结构体
	customTypeWndClass.cbSize = sizeof(WNDCLASSEXW);
	customTypeWndClass.style = CS_HREDRAW | CS_VREDRAW;
	customTypeWndClass.lpfnWndProc = CustomComboxProc; // 确保这个指针有效
	customTypeWndClass.cbClsExtra = 0;
	customTypeWndClass.cbWndExtra = 0;
	customTypeWndClass.hInstance = hInstance; // 应该是有效的实例句柄
	customTypeWndClass.hIcon = nullptr; // 不需要图标
	customTypeWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	customTypeWndClass.hbrBackground = hBrushSide; // 确保这个画刷是有效的
	customTypeWndClass.lpszMenuName = nullptr; // 如果不需要菜单
	customTypeWndClass.lpszClassName = L"TypeComboxClass"; // 确保这个名字是唯一的
	customTypeWndClass.hIconSm = nullptr; // 可选

	ATOM e = RegisterClassExW(&customTypeWndClass); // 注册侧边栏类
	DWORD dwError = GetLastError();

	return e;
}

HWND CreateSimpleToolbar(HWND hWndParent)
{
	// 创建工具栏
	HWND hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE | TBSTYLE_FLAT, // 工具栏风格
		0, 0, wstate.width, wstate.toolbarHeight, hWndParent, (HMENU)IDC_SIMPLEDRAW, hInst, NULL);

	// 初始化工具栏
	SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hToolBar, TB_SETPADDING, 0, MAKELPARAM(20, 20));  // 填充
	SendMessage(hToolBar, TB_SETBITMAPSIZE, 0, MAKELPARAM(30, 30));
	SendMessage(hToolBar, TB_SETBUTTONSIZE, 0, MAKELPARAM(50, 50)); // 设置按钮大小以确保图标居中

	// 定义工具栏按钮的位图
	TBADDBITMAP tbAddBitmap;
	tbAddBitmap.hInst = hInst;

	tbAddBitmap.nID = CHOOSE; // 工具栏的选择位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_LINE;  // 工具栏的线位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_CIRCLE; // 工具栏的圆位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_RECT; // 工具栏的矩形位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_CURVE; // 工具栏的曲线位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_MUTILINE; // 工具栏的多边形位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = DRAW_FMULTI;	// 工具栏的封闭多边形位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = FITSCREEN; // 工具栏的适应屏幕位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	tbAddBitmap.nID = CLEARIMG; // 工具栏的清空位图资源
	SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);

	const wchar_t* tooltips[] = {
	L"选择", L"线", L"圆", L"矩形", L"曲线", L"多义线", L"封闭多义线", L"适应屏幕", L"清空"
	};

	// 定义按钮
	TBBUTTON tbButtons[9] = {
		{ MAKELONG(0, 0), CHOOSE, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[0]},
		{ MAKELONG(1, 0), DRAW_LINE,   TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[1] },
		{ MAKELONG(2, 0), DRAW_CIRCLE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[2] },
		{ MAKELONG(3, 0), DRAW_RECT, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[3] },
		{ MAKELONG(4, 0), DRAW_CURVE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[4] },
		{ MAKELONG(5, 0), DRAW_MUTILINE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[5] },
		{ MAKELONG(6, 0), DRAW_FMULTI, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[6] },
		{ MAKELONG(7, 0), FITSCREEN, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[7] },
		{ MAKELONG(8, 0), CLEARIMG, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[8] }
	};

	// 初始化状态管理
	InitState(cs, 9, CHOOSE); // 初始化绘图状态
	AddIdToState(cs, CHOOSE); // 默认选择按钮
	AddIdToState(cs, DRAW_LINE);
	AddIdToState(cs, DRAW_CIRCLE);
	AddIdToState(cs, DRAW_RECT);
	AddIdToState(cs, DRAW_CURVE);
	AddIdToState(cs, DRAW_MUTILINE);
	AddIdToState(cs, DRAW_FMULTI);
	AddIdToState(cs, FITSCREEN);
	AddIdToState(cs, CLEARIMG);

	// 添加按钮到工具栏
	SendMessage(hToolBar, TB_ADDBUTTONS, sizeof(tbButtons) / sizeof(TBBUTTON), (LPARAM)&tbButtons);

	SendMessage(hToolBar, TB_SETMAXTEXTROWS, 0, 0);

	// 手动设置每个按钮的工具提示文本（悬停提示）
	HWND hTooltip = (HWND)SendMessage(hToolBar, TB_GETTOOLTIPS, 0, 0);

	TOOLINFO ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hToolBar;

	for (int i = 0; i < 9; ++i) {
		ti.uId = (UINT_PTR)tbButtons[i].idCommand;
		ti.lpszText = (LPWSTR)tooltips[i];
		SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti); // 添加工具提示
	}

	// 设置工具提示的背景颜色和文本颜色
	SendMessage(hTooltip, TTM_SETTIPBKCOLOR, (WPARAM)RGB(255, 255, 255), 0); // 背景色为淡黄色
	SendMessage(hTooltip, TTM_SETTIPTEXTCOLOR, (WPARAM)RGB(0, 0, 0), 0); // 文本色为黑色

	RECT rect;
	GetClientRect(hWndParent, &rect);  // 获取父窗口的大小
	SetWindowPos(hToolBar, NULL, 0, 0, rect.right, wstate.toolbarHeight, SWP_NOZORDER);  // 设置工具栏高度为45像素，宽度为窗口宽度

	return hToolBar;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中
	customProperty = DEFAULTDRAWPROPERTY; // 初始化自定义绘图属性

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wstate.width, wstate.height, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;
	// 在窗口创建时
	DragAcceptFiles(hWnd, TRUE);
	// 加载自定义光标
	LoadMyCustomCuser();

	// 创建工具栏
	hToolBar = CreateSimpleToolbar(hWnd);

	// 获得主窗口和工具栏的大小
	RECT mainrect;
	GetWindowRect(hWnd, &mainrect);
	GetClientRect(hWnd, &mainrect);
	RECT toolbarrect;
	GetWindowRect(hToolBar, &toolbarrect);
	MapWindowPoints(HWND_DESKTOP, GetParent(hToolBar), (LPPOINT)&toolbarrect, 2);
	InitWindowRect(wrect, mainrect, toolbarrect, WindowMode);

	// 小窗口初始化
	if (!InitSmallInstance(hInstance, nCmdShow)) return FALSE;

	// 创建侧边栏
	if (!InitSideInstance(hInstance, nCmdShow)) return FALSE;

	// 状态栏初始化
	if (!InitStatusInstance(hInstance, nCmdShow)) return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

BOOL InitSmallInstance(HINSTANCE hInstance, int nCmdShow)
{
	// 创建小窗口
	hSmallWnd = CreateWindow(L"SmallWindowClass", L"小窗口", WS_CHILD | WS_VISIBLE,
		wrect.smallrect.x, wrect.smallrect.y,
		wrect.smallrect.width, wrect.smallrect.height,
		hWnd, nullptr, hInstance, nullptr);

	if (!hSmallWnd) {
		MessageBox(NULL, L"小窗口创建失败", L"错误", MB_OK);
		return FALSE;
	}

	// 创建画布窗口
	hCanvasWnd = CreateWindow(L"CanvasWindowClass", L"画布", WS_CHILD | WS_VISIBLE,
		wrect.canvasrect.x, wrect.canvasrect.y,
		wrect.canvasrect.width, wrect.canvasrect.height,
		hSmallWnd, nullptr, hInstance, nullptr);

	if (!hCanvasWnd) {
		MessageBox(NULL, L"画布创建失败", L"错误", MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL InitSideInstance(HINSTANCE hInstance, int nCmdShow)
{
	// 创建侧边栏
	hSideWnd = CreateWindow(L"SideWindowClass", L"侧边栏", WS_CHILD | WS_VISIBLE,
		wrect.sidebarrect.x, wrect.sidebarrect.y,
		wrect.sidebarrect.width, wrect.sidebarrect.height,
		hWnd, nullptr, hInstance, nullptr);

	if (!hSideWnd) {
		MessageBox(NULL, L"侧边栏创建失败", L"错误", MB_OK);
		return FALSE;
	}

	return TRUE;
}

// 状态栏初始化
BOOL InitStatusInstance(HINSTANCE hInstance, int nCmdShow) {
	// 创建状态栏
	hStatusBar = CreateWindow(STATUSCLASSNAME, L"状态栏", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		wrect.statusbar.x, wrect.statusbar.y,
		wrect.statusbar.width, wrect.statusbar.height,
		hWnd, NULL, hInstance, NULL);

	if (!hStatusBar) {
		MessageBox(NULL, L"状态栏创建失败", L"错误", MB_OK);
		return FALSE;
	}

	int parts[] = { 125, 250, 375, -1 }; // 状态栏分割
	SendMessage(hStatusBar, SB_SETPARTS, sizeof(parts) / sizeof(int), (LPARAM)parts);

	// 设置状态栏初始文本（可用作显示坐标）
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)L"X: 0.00");
	SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)L"Y: 0.00");
	SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)L"R: 0.04");
	SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)L"OK");

	return TRUE;
}

// 更新状态栏中的坐标
void UpdateStatusBarCoordinates(double x, double y) {
	wchar_t textX[100], textY[100];

	// 更新 X 坐标文本
	swprintf_s(textX, L"X: %.2f", x);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)textX);

	// 更新 Y 坐标文本
	swprintf_s(textY, L"Y: %.2f", y);
	SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)textY);

	return;
}

void UpdateStatusBarRadius(double r) {
	wchar_t textR[100];

	swprintf_s(textR, L"R: %f", r);
	SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)textR);

	return;
}

void UpdateStatusBarText(const wchar_t* text) {
	SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)text);
	return;
}

void SaveGTXFile() {
	// 创建一个 OPENFILENAME 结构体
	OPENFILENAME ofn;
	wchar_t szFile[260];       // 文件路径缓冲区

	// 初始化 OPENFILENAME 结构体
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd; // 父窗口句柄
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0'; // 初始化文件名
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Images (*.gtx)\0*.gtx\0All Files (*.*)\0*.*\0"; // 文件类型过滤器
	ofn.lpstrTitle = L"Save Image File"; // 对话框标题
	ofn.Flags = OFN_OVERWRITEPROMPT; // 如果文件已存在，提示覆盖

	// 填充默认后缀
	wcscpy_s(szFile, L"yourImgName.gtx");

	// 弹出文件保存对话框
	if (GetSaveFileName(&ofn)) {
		FileOpenAndSave s = StoreImgToFile(&allImg, ofn.lpstrFile); // 保存到文件
		switch (s) {
		case SAVEOK:
			break;
		default:
			UpdateStatusBarText(L"文件保存失败");
			break;
		}
	}
}

//  目标: 处理主窗口的消息。
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// 重绘定时器
		SetTimer(hWnd, REDRAW, REDRAW_INTERVAL, NULL);
		break;
	}
	case WM_TIMER:
	{
		if (wParam == REDRAW) {
			if (needRedraw) {
				// 使用定时器清空背景
				InvalidateRect(hCanvasWnd, NULL, FALSE);
				Redraw();
			}
		}
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_SAVE: {
			SaveGTXFile();
			break;
		}
		case ID_XY_SYSTEM:
		{
			SetType(&customProperty, SetDrawType(customProperty.type, DRAWSYSTEM));
			break;
		}
		case ID_XY_BRE:
		{
			SetType(&customProperty, SetDrawType(customProperty.type, DRAWBRE));
			break;
		}
		case ID_XY_MID:
		{
			SetType(&customProperty, SetDrawType(customProperty.type, DRAWMID));
			break;
		}
		case ID_PAD_SYSTEM:
		{
			SetType(&customProperty, SetPadType(customProperty.type, PADSYSTEM));
			break;
		}
		case ID_PAD_SCAN:
		{
			SetType(&customProperty, SetPadType(customProperty.type, PADSCAN));
			break;
		}
		case ID_PAD_ZL:
		{
			SetType(&customProperty, SetPadType(customProperty.type, PADZL));
			break;
		}
		case IDM_OPEN:
		{
			// 创建一个 OPENFILENAME 结构体
			OPENFILENAME ofn;
			wchar_t szFile[260];       // 文件路径缓冲区

			// 初始化 OPENFILENAME 结构体
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd; // 父窗口句柄
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0'; // 初始化文件名
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Images (*.gtx)\0*.gtx\0All Files (*.*)\0*.*\0"; // 文件类型过滤器
			ofn.lpstrTitle = L"Open Image File"; // 对话框标题
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; // 文件必须存在且路径必须有效

			// 弹出文件打开对话框
			if (GetOpenFileName(&ofn)) {
				FileOpenAndSave s = FileToStoreImg(&allImg, ofn.lpstrFile); // 加载文件
				switch (s) {
				case OPENOK:
					FitCanvasCoordinate(coordinate, allImg, hCanvasWnd); // 适应坐标系
					RedrawFixedContent(hCanvasWnd, hdcMemFixed);
					NeedRedraw();
					break;
				case FILEHEADERINVALID:
					UpdateStatusBarText(L"文件头部无效");
					break;
				case FILEVERSIONINVALID:
					UpdateStatusBarText(L"文件版本无效");
					break;
				case FILEOPENFAILE:
					UpdateStatusBarText(L"文件打开失败");
					break;
				case MEMORRYALLOCFAIL:
					break;
				}
			}
			break;
		}
		case CLEARIMG:
		{
			setType(mst, mst.type);
			ClearDrawingImg(&drawing); // 清空当前绘制的图形
			ClearStoreImg(&allImg); // 清空图形
			ClearPreviewContent(hdcMemPreview); // 清空预览画布
			RedrawFixedContent(hCanvasWnd, hdcMemFixed);
			NeedRedraw();
			break;
		}
		case DRAW_LINE:
			SetToolBarCheck(hToolBar, cs, DRAW_LINE);
			setType(mst, DRAWLINE);
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWLINE, 0);

			break;
		case DRAW_CIRCLE:
			SetToolBarCheck(hToolBar, cs, DRAW_CIRCLE);
			setType(mst, DRAWCIRCLE);
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWCIRCLE, 0);

			break;
		case CHOOSE:
			SetToolBarCheck(hToolBar, cs, CHOOSE);
			setType(mst, CHOOSEIMG);
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, CHOOSEIMG, 0);

			break;
		case DRAW_RECT:
			SetToolBarCheck(hToolBar, cs, DRAW_RECT);
			setType(mst, DRAWRECTANGLE);
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWRECTANGLE, 0);

			break;
		case DRAW_CURVE:
			SetToolBarCheck(hToolBar, cs, DRAW_CURVE);
			setType(mst, DRAWCURVE);
			drawing.type = CURVE;
			drawing.proper = customProperty;
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWCURVE, 0);

			break;
		case DRAW_MUTILINE:
			SetToolBarCheck(hToolBar, cs, DRAW_MUTILINE);
			setType(mst, DRAWMULTILINE);
			drawing.type = MULTILINE;
			drawing.proper = customProperty;
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWMULTILINE, 0);

			break;
		case DRAW_FMULTI:
			SetToolBarCheck(hToolBar, cs, DRAW_FMULTI);
			setType(mst, DRAWFMULTI);
			drawing.type = FMULTILINE;
			drawing.proper = customProperty;
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWFMULTI, 0);

			break;
		case FITSCREEN:
			FitCanvasCoordinate(coordinate, allImg, hCanvasWnd); // 适应坐标系
			RedrawFixedContent(hCanvasWnd, hdcMemFixed);
			NeedRedraw();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
	{
		// 获取主窗口的宽度和高度
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		// 设置工具栏的新宽度
		int toolbarWidth = width; // 工具栏的宽度等于窗口宽度

		// 获取工具栏的当前高度
		RECT toolbarRect;
		GetWindowRect(hToolBar, &toolbarRect);
		MapWindowPoints(HWND_DESKTOP, hWnd, (LPPOINT)&toolbarRect, 2);
		int toolbarHeight = toolbarRect.bottom - toolbarRect.top;

		// 调整工具栏大小
		MoveWindow(hToolBar, 0, 0, toolbarWidth, toolbarHeight, TRUE);

		// 调整小窗口和画布区域
		RefreshWindowRect(wrect, width, height, WindowMode); // 更新窗口矩形

		// 调整小窗口大小
		MoveWindow(hSmallWnd, wrect.smallrect.x, wrect.smallrect.y, wrect.smallrect.width, wrect.smallrect.height, TRUE);

		// 调整画布窗口大小
		MoveWindow(hCanvasWnd, wrect.canvasrect.x, wrect.canvasrect.y, wrect.canvasrect.width, wrect.canvasrect.height, TRUE);

		// 调整状态栏大小
		MoveWindow(hStatusBar, wrect.statusbar.x, wrect.statusbar.y, wrect.statusbar.width, wrect.statusbar.height, TRUE);

		// 调整侧边栏大小
		MoveWindow(hSideWnd, wrect.sidebarrect.x, wrect.sidebarrect.y, wrect.sidebarrect.width, wrect.sidebarrect.height, TRUE);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// 设置文本颜色
		SetTextColor(hdc, RGB(0, 0, 0)); // 黑色
		SetBkMode(hdc, TRANSPARENT); // 透明背景

		RECT rect;
		GetClientRect(hWnd, &rect);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

		// 获取子窗口的最小尺寸
		RECT childRect;
		GetWindowRect(hSmallWnd, &childRect);
		int childMinWidth = 550;
		int childMinHeight = 400;

		// 设置父窗口的最小尺寸，确保不小于子窗口的最小尺寸
		pMinMax->ptMinTrackSize.x = childMinWidth + 20;  // 添加边距
		pMinMax->ptMinTrackSize.y = childMinHeight + 100; // 添加边距
		break;
	}
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		// 获取拖拽的文件数量
		UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

		UINT maxCount = 1;
		if (fileCount > maxCount) {
			UpdateStatusBarText(L"只能处理一个文件");
			DragFinish(hDrop); // 释放内存
			break;
		}

		WCHAR filePath[MAX_PATH]; // 使用数组来存储文件路径
		// 获取文件路径
		DragQueryFile(hDrop, 0, filePath, MAX_PATH);

		int result = MessageBox(hWnd, L"是否保存当前编辑的数据", L"提示", MB_YESNO);
		if (result == IDYES) {
			SaveGTXFile();
		}

		// 将GTX文件加载到画布中
		FileToStoreImg(&allImg, filePath);
		FitCanvasCoordinate(coordinate, allImg, hCanvasWnd); // 适应坐标系
		RedrawFixedContent(hCanvasWnd, hdcMemFixed);
		NeedRedraw();
		// 释放内存
		DragFinish(hDrop);
		break;
	}
	case WM_DESTROY:
		KillTimer(hWnd, REDRAW);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 小窗口区域, 管理画布窗口
LRESULT CALLBACK SmallWndProc(HWND hCWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	default:
		return DefWindowProc(hCWnd, message, wParam, lParam);
	}

	return 0;
}

// 状态栏
LRESULT CALLBACK StatusWndProc(HWND hSWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	default:
		return DefWindowProc(hSWnd, message, wParam, lParam);
	}

	return 0;
}

POINT GetEditPoint(HWND edit1, HWND edit2) {
	// 按钮被点击，获取输入框的内容
	wchar_t buffer1[100]; // 用于存储输入框1的内容
	wchar_t buffer2[100]; // 用于存储输入框2的内容

	// 从输入框中获取文本
	GetWindowText(Edit1, buffer1, 100);
	GetWindowText(Edit2, buffer2, 100);

	double value1 = wcstod(buffer1, NULL); // 转换为浮点数
	double value2 = wcstod(buffer2, NULL);

	// 获取 hCanvasWnd 的窗口左上角的屏幕坐标
	RECT rectCanvas;
	GetWindowRect(hCanvasWnd, &rectCanvas);

	return mapCoordinate(coordinate, value1, value2);
}

// 侧边栏
LRESULT CALLBACK SideWndProc(HWND hSWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		if (!InDraw(mst)) {
			UpdateStatusBarText(L"请进入绘图模式再输入坐标!");
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hSWnd, &ps);

		SetTextColor(hdc, RGB(0, 0, 0)); // 黑色
		SetBkMode(hdc, TRANSPARENT); // 透明背景

		// 绘制两个输入框的标签
		TextOut(hdc, 25, 30, L"X:", 2);
		TextOut(hdc, 25, 60, L"Y:", 2);

		break;
	}
	case WM_CREATE:
	{
		// 创建输入框和按钮
		Edit1 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			50, 30, 75, 20, hSWnd, NULL, hInst, NULL);

		Edit2 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			50, 60, 75, 20, hSWnd, NULL, hInst, NULL);

		Button = CreateWindow(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			25, 90, 100, 20, hSWnd, (HMENU)1, hInst, NULL);

		EnableWindow(Edit1, FALSE);
		EnableWindow(Edit2, FALSE);
		EnableWindow(Button, FALSE);

		//// 创建类型选择下拉框
		//TypeCombo = CreateWindow(L"TypeComboxClass", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		//	25, 140, 150, 60, hSWnd, (HMENU)2, hInst, NULL);

		// 创建线宽滑块
		WidthSlider = CreateWindow(L"CustomSliderClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 140, 150, 60, hSWnd, (HMENU)3, hInst, NULL);

		SendMessage(WidthSlider, CUSTOM_VALUE_CHANGE, NULL, (LPARAM)DEFAULTLINEWID);
		SendMessage(WidthSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"线宽:");

		ColorSlider = CreateWindow(L"ColorPickerClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 220, 150, 65, hSWnd, (HMENU)4, hInst, NULL);

		SendMessage(ColorSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"颜色:");
		SendMessage(ColorSlider, CUSTOM_SET_NUM, NULL, (LPARAM)1);

		bgColorSlider = CreateWindow(L"ColorPickerClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 300, 150, 65, hSWnd, (HMENU)5, hInst, NULL);

		SendMessage(bgColorSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"背景:");
		SendMessage(ColorSlider, CUSTOM_SET_NUM, NULL, (LPARAM)2);

		DWORD dwError = GetLastError();

		break;
	}
	case CUSTOM_COLOR_CHANGE:
	{
		switch ((INT)lParam) {
		case 1:
			SetColorWithColorRef(&customProperty, (COLORREF)wParam);
			break;
		case 2:
			SetBgColorWithColorRef(&customProperty, (COLORREF)wParam);
			break;
		}

		break;
	}
	case CUSTOM_WIDTH_CHANGE:
	{
		SetWidth(&customProperty, (int)wParam);
		break;
	}
	case CUSTOM_TYPE_CHANGE:
	{
		SetType(&customProperty, (gctype)wParam);
		break;
	}
	case CUSTOM_DRAWSTATE_CHANGE:
	{
		if (InDraw(mst)) {
			EnableWindow(Edit1, TRUE);
			EnableWindow(Edit2, TRUE);
			EnableWindow(Button, TRUE);
		}
		else {
			EnableWindow(Edit1, FALSE);
			EnableWindow(Edit2, FALSE);
			EnableWindow(Button, FALSE);
		}
		break;
	}
	case WM_COMMAND:
	{
		// 判断按钮是否被点击
		if (LOWORD(wParam) == 1) {
			if (!InDraw(mst)) break;
			// 确定被点击
			POINT pt = GetEditPoint(Edit1, Edit2); // 获取输入框的内容

			// 构造 lParam，将 X 和 Y 坐标打包为 LPARAM 格式
			LPARAM lParam = MAKELPARAM(pt.x, pt.y);  // 低位字为 X 坐标，高位字为 Y 坐标

			// 发送 WM_LBUTTONDOWN 消息
			PostMessage(hCanvasWnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);

			// 发送 WM_LBUTTONUP 消息
			PostMessage(hCanvasWnd, WM_LBUTTONUP, 0, lParam);

			// 重置输入框
			SetWindowText(Edit1, L"");
			SetWindowText(Edit2, L"");
		}
		else if (HIWORD(wParam) == EN_CHANGE) {
			if (!InDraw(mst)) break;
			POINT pt = GetEditPoint(Edit1, Edit2); // 获取输入框的内容

			PostMessage(hCanvasWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
		}
		break;
	}
	default:
		return DefWindowProc(hSWnd, message, wParam, lParam);
	}

	return 0;
}

// 清空预览画布
void ClearPreviewContent(HDC hdc) {
	RECT rect;
	GetClientRect(hCanvasWnd, &rect);
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

// 画布窗口，处理具体的画图逻辑，使用文件存储绘制的图像，方便重绘和对图像进行操作等
LRESULT CALLBACK CanvasWndProc(HWND hCWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HPEN hPen = NULL; // 静态变量，以便在多个消息中共享
	static HBRUSH hNullBrush = NULL; // 静态变量
	static HBRUSH hBlackBrush = NULL; // 静态变量
	static BOOL mouseTracking = FALSE; // 记录是否已启用鼠标跟踪

	switch (message)
	{
	case WM_SETCURSOR:
	{
		if (LOWORD(lParam) == HTCLIENT) {
			switch (mst.type) {
			case CHOOSEN:
				SetCursor(chooseCursor);
				break;
			case MMOUSEMOVE:
				SetCursor(moveCursor);
				break;
			case CHOOSEIMG:
				SetCursor(originalCursor);
				break;
			default:
				SetCursor(customCursor);
				break;
			}
		}
		else {
			SetCursor(originalCursor);
		}
		break;
	}
	case WM_CREATE:
	{
		InitializeBuffers(hCWnd);
		RedrawFixedContent(hCWnd, hdcMemFixed);
		InitStoreImg(&allImg);
		// 创建黑色画笔
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		// 创建无色画刷
		LOGBRUSH lbb;
		lbb.lbStyle = BS_NULL;
		lbb.lbColor = RGB(0, 0, 0);
		lbb.lbHatch = 0;
		hNullBrush = CreateBrushIndirect(&lbb);
		// 创建黑色画刷
		hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
		UpdateStatusBarRadius(coordinate.radius);

		break;
	}
	case WM_DESTROY:
	{
		Cleanup();
		// 释放画笔和画刷
		if (hPen) {
			DeleteObject(hPen);
			hPen = NULL; // 清空指针
		}
		if (hNullBrush) {
			DeleteObject(hNullBrush);
			hNullBrush = NULL; // 清空指针
		}
		if (hBlackBrush) {
			DeleteObject(hBlackBrush);
			hBlackBrush = NULL; // 清空指针
		}
		break;
	}
	case WM_SIZE:
	{
		// 窗口大小改变时，需要更新
		Cleanup();
		InitializeBuffers(hCWnd);
		RedrawFixedContent(hCWnd, hdcMemFixed);
		NeedRedraw();
		break;
	}
	case WM_PAINT:
	{
		switch (WindowMode) {
		case 1:
			break;
		case 2:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hCWnd, &ps);

			// 获取窗口大小
			RECT canvasRect;
			GetClientRect(hCWnd, &canvasRect);

			int canvasWidth = canvasRect.right - canvasRect.left;
			int canvasHeight = canvasRect.bottom - canvasRect.top;

			// 先将固定的图形叠加到预览hdc中到窗口
			BitBlt(hdcMemPreview, 0, 0, canvasWidth, canvasHeight, hdcMemFixed, 0, 0, SRCAND);

			// 然后将预览内容叠加到窗口上
			BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMemPreview, 0, 0, SRCCOPY);

			EndPaint(hCWnd, &ps);
			break;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);

		switch (mst.type) {
		case CHOOSEIMG:
		case CHOOSEN:
		{
			mst.lastLButtonPoint = point;
			cs.count = ChooseImg(allImg, coordinate, point);
			if (cs.count != -1) {
				mst.type = CHOOSEN;
			}
			else {
				mst.type = CHOOSEIMG;
			}
			break;
		}
		case DRAWLINE:
		{
			if (DrawStateInit(mst)) {
				mst.lastLButtonPoint = point;
			}
			else {
				// 在固定图像上绘制线
				DrawLine(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				// 保存线
				StoreLineTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
			}
			break;
		}
		case DRAWCIRCLE:
		{
			if (DrawStateInit(mst)) {
				mst.lastLButtonPoint = point;
			}
			else {
				// 画圆
				DrawCircle(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				// 画圆,第一个点为圆心，第二个点为半径
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				StoreCircleTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
			}
			break;
		}
		case DRAWRECTANGLE:
		{
			if (DrawStateInit(mst)) {
				mst.lastLButtonPoint = point;
			}
			else {
				// 画矩形
				DrawRectangle(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				// 保存矩形
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				StoreRectangleTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
			}
			break;
		}
		case DRAWFMULTI:
		case DRAWMULTILINE:
		{
			// 多义线绘制
			if (DrawStateInit(mst)) {
				mst.lastLButtonPoint = point;
				// 初始化 mst.
				InitMultiline(&(drawing.multiline));
				MyPoint lastPoint;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, lastPoint.x, lastPoint.y);
				AddPointToMultiline(&(drawing.multiline), lastPoint);
			}
			else {
				// 画一条线
				DrawLine(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				// 将上一个点保存到DrawInfo中
				mst.lastLButtonPoint = point;
				MyPoint lastPoint;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, lastPoint.x, lastPoint.y);
				AddPointToMultiline(&(drawing.multiline), lastPoint);
			}
			break;
		}
		case DRAWCURVE:
		{
			// 曲线绘制, 先设置起点和终点
			if (DrawStateInit(mst)) {
				mst.lastLButtonPoint = point;
				InitCurve(&(drawing.curve));
				MyPoint lastPoint;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, lastPoint.x, lastPoint.y);
				AddPointToCurve(&(drawing.curve), lastPoint);
			}
			else {
				MyPoint nowpoint;
				PointToCoordinate(coordinate, point, nowpoint.x, nowpoint.y);
				AddPointToCurve(&(drawing.curve), nowpoint);
				mst.lastLButtonPoint = point;
			}
			break;
		}
		case KZDRAW:
		{
			// 扩展功能
			switch (mst.kztype) {
			case DRAWCX:
			{
				// 保存线段
				ClearPreviewContent(hdcMemPreview);
				if (cs.count == -1) break;
				DrawInfo choose = allImg.img[cs.count];
				if (choose.type != LINE) break;
				MyPoint start = choose.line.start;
				MyPoint end = choose.line.end;
				MyPoint mp;
				PointToCoordinate(coordinate, point, mp.x, mp.y);
				// 计算垂线
				MyPoint p = CalPerpendicular(start, end, mp);

				// 保存线段
				StoreLineTo(&allImg, mp, p, customProperty);

				// 画线
				POINT pt = mapCoordinate(coordinate, p.x, p.y);
				DrawLine(hdcMemFixed, point, pt, &customProperty);
				break;
			}
			}
		}
		default:
			break;
		}

		break;
	}
	// 抬起绘制完毕
	case WM_LBUTTONUP:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);
		HDC hdc = GetDC(hCWnd);
		// 创建画笔
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // 黑色画笔
		switch (mst.type) {
		case CHOOSEIMG:
		{
			// 没有选中
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case CHOOSEN:
		{
			ClearPreviewContent(hdcMemPreview);
			if (cs.count == -1) {
				break;
			}
			DrawInfo choose = allImg.img[cs.count];
			switch (choose.type) {
			case CIRCLE:
			{
				// 在预览窗口绘制圆心
				MyPoint mp = choose.circle.center;
				POINT pt = mapCoordinate(coordinate, mp.x, mp.y);
				// 画圆心
				SelectObject(hdcMemPreview, hPen);
				SelectObject(hdcMemPreview, hBlackBrush);
				Ellipse(hdcMemPreview, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
				// 显示坐标
				ShowPointInWindow(hdcMemPreview, mp);
				CalAndShowPoint();
				break;
			}
			case LINE:
			{
				CalAndShowPoint();
				break;
			}
			}
			NeedRedraw();
			break;
		}
		case DRAWLINE:
		{
			// 如果当前的点与上次的点位置相同，则为两点画线
			if (!DrawStateInit(mst) && TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				// 在固定图像上绘制线
				DrawLine(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				// 画线
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				StoreLineTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		case DRAWCIRCLE:
		{
			// 画圆
			if (!DrawStateInit(mst) && TwoPointDraw(mst.lastLButtonPoint, point)) {
				// 画圆
				ClearPreviewContent(hdcMemPreview);
				DrawCircle(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				// 画圆,第一个点为圆心，第二个点为半径
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				StoreCircleTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		case DRAWRECTANGLE:
		{
			// 画矩形
			if (!DrawStateInit(mst) && TwoPointDraw(mst.lastLButtonPoint, point)) {
				// 画矩形
				ClearPreviewContent(hdcMemPreview);
				DrawRectangle(hdcMemFixed, mst.lastLButtonPoint, point, &customProperty);
				MyPoint start, end;
				PointToCoordinate(coordinate, mst.lastLButtonPoint, start.x, start.y);
				PointToCoordinate(coordinate, point, end.x, end.y);
				StoreRectangleTo(&allImg, start, end, customProperty);
				mst.lastLButtonPoint = { -1, -1 };
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_MOUSELEAVE:
	{
		// 鼠标离开窗口时的处理
		switch (mst.type) {
		case MMOUSEMOVE:
		{
			// 清除状态
			RestoreFormLastType(mst);
			break;
		}
		default:
			break;
		}
		mouseTracking = FALSE; // 重置鼠标跟踪状态
		NeedRedraw();
		break;
	}
	case WM_RBUTTONDOWN:
	{
		switch (mst.type) {
		case DRAWLINE:
		case DRAWCIRCLE:
		case DRAWRECTANGLE:
		{
			// 清空状态
			// 如果为绘制状态，则清空绘制状态
			if (!DrawStateInit(mst)) {
				setType(mst, mst.type);
			}
			// 如果为初始绘制状态，则回到选择状态
			else {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
			}
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case DRAWMULTILINE:
		{
			// 完成绘制逻辑
			// 保存绘图信息
			if (DrawStateInit(mst) || !drawing.multiline.points) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				ClearPreviewContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			DrawInfo mline;
			mline.type = MULTILINE;
			mline.proper = drawing.proper;
			InitFromMultiline(&(mline.multiline), &(drawing.multiline));
			AddDrawInfoToStoreImg(&allImg, mline);
			// 清空状态
			mst.lastLButtonPoint = { -1, -1 };
			ClearMultiline(&(drawing.multiline));
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case DRAWFMULTI:
		{
			// 完成绘制逻辑
			// 保存绘图信息
			if (DrawStateInit(mst) || !drawing.multiline.points) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				ClearPreviewContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			// 画一条第一个点到最后一个点的直线
			POINT p1, p2;
			p1 = mapCoordinate(coordinate, drawing.multiline.points[0].x, drawing.multiline.points[0].y);
			int last = drawing.multiline.endNum - 1;
			p2 = mapCoordinate(coordinate, drawing.multiline.points[last].x, drawing.multiline.points[last].y);
			DrawLine(hdcMemFixed, p1, p2, &customProperty);

			DrawInfo mline;
			mline.type = FMULTILINE;
			mline.proper = drawing.proper;
			InitFromMultiline(&(mline.multiline), &(drawing.multiline));
			AddDrawInfoToStoreImg(&allImg, mline);
			// 清空状态
			mst.lastLButtonPoint = { -1, -1 };
			ClearMultiline(&(drawing.multiline));
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case DRAWCURVE:
		{
			// 完成绘制逻辑
			// 保存绘图信息
			if (DrawStateInit(mst) || !drawing.multiline.points) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				break;
			}
			DrawInfo curve;
			curve.proper = drawing.proper;
			curve.type = CURVE;
			InitFromCurve(&(curve.curve), &(drawing.curve));
			AddDrawInfoToStoreImg(&allImg, curve);
			// 绘制到固定画布上
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing.curve.numPoints];
			int count = 0;
			for (int i = 0; i < drawing.curve.endNum; i++) {
				MyPoint pt = drawing.curve.controlPoints[i];
				if (pt.x == DBL_MAX || pt.y == DBL_MAX) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing.curve.numPoints)
			{
				// GDI+绘图
				Graphics graphics(hdcMemFixed);
				int color = drawing.proper.color;
				// 提取 ARGB 组件
				int red = color & 0xFF;   // 提取 Red 分量
				int green = (color >> 8) & 0xFF;  // 提取 Green 分量
				int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

				Pen pen(Color(255, red, green, blue), drawing.proper.width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing.curve.numPoints);
			}

			delete[] gdiplusPoints;
			// 清空状态
			mst.lastLButtonPoint = { -1, -1 };
			ClearCurve(&(drawing.curve));
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case MMOUSEMOVE:
		{
			// 清除状态
			RestoreFormLastType(mst);
			ClearPreviewContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case CHOOSEN:
		{
			if (cs.count == -1) break;
			DrawInfo choose = allImg.img[cs.count];
			switch (choose.type) {
			case LINE:
			{
				setKZType(mst, DRAWCX);
			}
			break;
			default:
				break;
			}
			break;
		}
		case KZDRAW:
		{
			switch (mst.kztype) {
			case DRAWCX:
			{
				EndKZType(mst);
				ClearPreviewContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	// 更新预览状态
	case WM_MOUSEMOVE:
	{
		// 如果尚未启用鼠标跟踪，则启用
		if (!mouseTracking) {
			EnableMouseTracking(hCWnd);
			mouseTracking = TRUE;
		}
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);

		MyPoint mp;
		PointToCoordinate(coordinate, point, mp.x, mp.y);
		// 鼠标位置状态栏
		UpdateStatusBarCoordinates(mp.x, mp.y);

		if (!CanRefresh(mst)) break;

		switch (mst.type) {
		case CHOOSEN:
		{
			// 更新状态, 计算是否继续选中
			DrawInfo choose = allImg.img[cs.count];
			if (!ContinueChooseDrawInfo(choose, coordinate, point)) {
				setType(mst, CHOOSEIMG);
				cs.count = -1;
			}
			break;
		}
		case MMOUSEMOVE:
		{
			ClearPreviewContent(hdcMemPreview);
			// 获得移动距离
			int x = point.x - mst.lastMMousemovePoint.x;
			int y = point.y - mst.lastMMousemovePoint.y;
			// 更新上次鼠标移动的位置
			mst.lastMMousemovePoint = point;
			// 更新坐标系中心
			coordinate.center.x += x;
			coordinate.center.y += y;
			RedrawFixedContent(hCWnd, hdcMemFixed);
			// 触发重绘
			NeedRedraw();
			break;
		}
		case DRAWLINE:
		{
			if (TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				DrawLine(hdcMemPreview, mst.lastLButtonPoint, point, &customProperty);
				// 触发重绘
				NeedRedraw();
			}

			break;
		}
		case DRAWMULTILINE:
		{
			if (TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				// 画线
				DrawLine(hdcMemPreview, mst.lastLButtonPoint, point, &customProperty);
				// 触发重绘
				NeedRedraw();
			}

			break;
		}
		case DRAWFMULTI:
		{
			if (TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				MyPoint firstM = drawing.multiline.points[0];
				POINT first = mapCoordinate(coordinate, firstM.x, firstM.y);

				// 画虚线
				if (drawing.multiline.numPoints > 1) {
					DrawXLine(hdcMemPreview, point, first, &customProperty);
				}
				// 画线
				DrawLine(hdcMemPreview, mst.lastLButtonPoint, point, &customProperty);
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		case DRAWCIRCLE:
		{
			if (TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				// 画圆
				DrawCircle(hdcMemPreview, mst.lastLButtonPoint, point, &customProperty);
				// 触发重绘
				NeedRedraw();
			}

			break;
		}
		case DRAWRECTANGLE:
		{
			if (TwoPointDraw(mst.lastLButtonPoint, point)) {
				ClearPreviewContent(hdcMemPreview);
				// 画矩形
				DrawRectangle(hdcMemPreview, mst.lastLButtonPoint, point, &customProperty);
				// 触发重绘
				NeedRedraw();
			}

			break;
		}
		case DRAWCURVE:
		{
			// GDI+绘图
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing.curve.numPoints + 1];
			int count = 0;
			for (int i = 0; i < drawing.curve.endNum; i++) {
				MyPoint pt = drawing.curve.controlPoints[i];
				if (pt.x == DBL_MAX || pt.y == DBL_MAX) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing.curve.numPoints) {
				ClearPreviewContent(hdcMemPreview);
				gdiplusPoints[count] = Gdiplus::Point(point.x, point.y);
				// 画曲线
				Graphics graphics(hdcMemPreview);
				int color = drawing.proper.color;
				// 提取 ARGB 组件
				int red = color & 0xFF;   // 提取 Red 分量
				int green = (color >> 8) & 0xFF;  // 提取 Green 分量
				int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

				Pen pen(Color(255, red, green, blue), drawing.proper.width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing.curve.numPoints + 1);
				// 触发重绘
				NeedRedraw();
			}
			delete[] gdiplusPoints;

			break;
		}
		case KZDRAW:
		{
			// 扩展功能
			switch (mst.kztype) {
			case DRAWCX:
			{
				ClearPreviewContent(hdcMemPreview);
				// 预览垂线,为选中的线段的垂线
				if (cs.count == -1) break;
				DrawInfo choose = allImg.img[cs.count];
				if (choose.type != LINE) break;
				MyPoint start = choose.line.start;
				MyPoint end = choose.line.end;
				// 计算垂线
				MyPoint p = CalPerpendicular(start, end, mp);
				POINT pt = mapCoordinate(coordinate, p.x, p.y);
				// 画线
				DrawLine(hdcMemPreview, point, pt, &customProperty);
				// 触发重绘
				NeedRedraw();
				break;
			}
			}
		}
		default:
			break;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		break;
	}
	case WM_KEYUP:
	{
		break;
	}
	case WM_MBUTTONDOWN:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(hCWnd);
		setTypeWithLastType(mst, MMOUSEMOVE);
		mst.lastMMousemovePoint = point;
		ClearPreviewContent(hdcMemPreview);
		break;
	}
	case WM_MBUTTONUP:
	{
		// 重置状态
		RestoreFormLastType(mst);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(hCWnd);
		MyPoint mp;
		PointToCoordinate(coordinate, point, mp.x, mp.y);
		// 鼠标位置状态栏
		UpdateStatusBarCoordinates(mp.x, mp.y);

		// 放大时,坐标系radius减小，缩小时，坐标系radius增大
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		// 缩放因子，可以调整为更适合的值
		double scaleFactor = RADIUSCHANGESPEED;

		// 使用指数缩放，确保 radius 始终大于0
		coordinate.radius *= exp(-scaleFactor * zDelta);

		// 防止缩放比例过小或过大
		if (coordinate.radius < MINRADIUS) {
			coordinate.radius = MINRADIUS; // 限制最小缩放比例
		}
		else if (coordinate.radius > MAXRADIUS) {
			coordinate.radius = MAXRADIUS; // 限制最大缩放比例
		}
		// 更新Radius
		UpdateStatusBarRadius(coordinate.radius);

		RedrawFixedContent(hCWnd, hdcMemFixed); // 重绘固定内容
		ClearPreviewContent(hdcMemPreview);
		// 重新绘制窗口（触发 WM_PAINT）
		NeedRedraw();
		break;
	}
	default:
		return DefWindowProc(hCWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	HWND hEdit;
	switch (message)
	{
	case WM_INITDIALOG:
		// 创建一个缓冲区来存储加载的文本
		hEdit = GetDlgItem(hDlg, IDC_EDIT1);

		SendMessage(hEdit, EM_SETREADONLY, TRUE, 0);

		TCHAR helpText[1024]; // 根据需要调整缓冲区大小
		LoadString(GetModuleHandle(NULL), IDS_HELP, helpText, sizeof(helpText) / sizeof(TCHAR));
		SetWindowText(hEdit, helpText);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void FitCanvasCoordinate(Coordinate& coor, StoreImg& img, HWND hcwnd) {
	// 获得画布窗口大小
	RECT canvasRect;
	GetClientRect(hcwnd, &canvasRect);
	FitCoordinate(coordinate, allImg, canvasRect);
}

void InitializeBuffers(HWND hWnd) {
	// 获取画布窗口大小
	GetClientRect(hWnd, &canvasRect);

	// 创建内存DC用于固定图像
	HDC hdcScreen = GetDC(hWnd);
	hdcMemFixed = CreateCompatibleDC(hdcScreen);
	hbmMemFixed = CreateCompatibleBitmap(hdcScreen, canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top);
	hbmOldFixed = (HBITMAP)SelectObject(hdcMemFixed, hbmMemFixed);

	// 创建内存DC用于预览图像
	hdcMemPreview = CreateCompatibleDC(hdcScreen);
	hbmMemPreview = CreateCompatibleBitmap(hdcScreen, canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top);
	hbmOldPreview = (HBITMAP)SelectObject(hdcMemPreview, hbmMemPreview);

	// 设置为白色背景
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdcMemFixed, &canvasRect, hBrush);
	FillRect(hdcMemPreview, &canvasRect, hBrush);

	DeleteObject(hBrush);
	ReleaseDC(hWnd, hdcScreen);
}

void Cleanup() {
	// 清理内存DC和位图
	SelectObject(hdcMemFixed, hbmOldFixed);
	DeleteObject(hbmMemFixed);
	DeleteDC(hdcMemFixed);

	SelectObject(hdcMemPreview, hbmOldPreview);
	DeleteObject(hbmMemPreview);
	DeleteDC(hdcMemPreview);
}

// 初始化 GDI+
void InitGDIPlus() {
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// 关闭 GDI+
void ShutdownGDIPlus() {
	GdiplusShutdown(gdiplusToken);
}

void ShowAllCalPoint(HDC hdc) {
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBlackBrush);
	// 在预览窗口显示所有计算点
	for (int i = 0; i < CalculatePoints.size(); i++) {
		MyPoint mp = CalculatePoints[i];
		POINT pt = mapCoordinate(coordinate, mp.x, mp.y);
		// 画圆心
		SelectObject(hdc, hPen);
		SelectObject(hdc, hBlackBrush);
		Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
		// 显示坐标
		ShowPointInWindow(hdc, mp);
	}
	DeleteObject(hPen);
	DeleteObject(hBlackBrush);
}

void CalAndShowPoint() {
	// 计算交点
	CalculateImg(allImg, cs.count);
	// 显示交点
	ShowAllCalPoint(hdcMemPreview);
	return;
}

void LoadMyCustomCuser() {
	// 加载自定义光标
	customCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_DRAWING));
	originalCursor = LoadCursor(NULL, IDC_ARROW);
	chooseCursor = LoadCursor(NULL, IDC_HAND);
	moveCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_MOVE));

	return;
}
