#include "pch.h"
#include "framework.h"
#include "SimpleDraw.h"
#include "showRMenu.h"
#include "customSlider.h"
#include "ywljDraw.h"

#define MAX_LOADSTRING	100
#define COLORSLIDE		1
#define BGSLIDE			2

#define RBUTTOMDOWNCUSTOM		1
#define CUSTOMRBUTTOMDOWN(lparam)	(lparam == RBUTTOMDOWNCUSTOM)
#define CUSTOM_REDRAW_DRAWING		(WM_USER + 20)

// 定时器
#define REDRAW				1
#define HZ					100
#define REDRAW_INTERVAL		1000 / HZ
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

HDC hdcMemFixed;			// 固定图像内存DC
HDC hdcMemPreview;			// 预览图像内存DC
HDC hdcMemCoS;				// 计算或选中图像内存DC
HBITMAP hbmMemFixed;		// 固定图像位图
HBITMAP hbmMemPreview;		// 预览图像位图
HBITMAP hbmOldFixed;		// 原固定位图
HBITMAP hbmOldPreview;		// 原预览位图
HBITMAP hbmmemCoS;			// 计算或选中图像位图
HBITMAP hbmOldCoS;			// 原计算或选中位图

// 全局变量用于保存光标的原始样式
HCURSOR originalCursor;
HCURSOR customCursor;		// 全局变量保存自定义光标句柄
HCURSOR chooseCursor;		// 选择光标,十字光标表示可以
HCURSOR moveCursor;			// 移动光标,四向箭头表示可以移动

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

POINT				GetEditPoint(HWND edit1, HWND edit2);
void				RedrawFixedContent(HWND hCWnd, HDC hdc);
void				EnableMouseTracking(HWND hWnd);
void				InitializeBuffers(HWND hWnd);
void				ClearContent(HDC hdc);
void				FitCanvasCoordinate();
void 				LoadMyCustomCuser();
void				CalAndShowPoint();
void				ClearCSState();
void				Cleanup();


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

	InitMyDrawState(mst);				// 初始化绘图状态
	InitDrawing(&drawing);				// 初始化drawing状态
	InitCSDrawInfo(csdraw);				// 初始化csdraw状态
	InitDrawUnitPro(&customProperty);	 // 初始化自定义绘图属性

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

	const INT toolids[] = {
		CHOOSE, DRAW_LINE, DRAW_CIRCLE, DRAW_RECT, DRAW_CURVE, DRAW_BCURVE, DRAW_MUTILINE, DRAW_FMULTI, FITSCREEN, CLEARIMG
	};

	const wchar_t* tooltips[] = {
	L"选择", L"线", L"圆", L"矩形", L"曲线", L"B样条", L"多义线", L"封闭多义线", L"适应屏幕", L"清空"
	};

	size_t tools = sizeof(toolids) / sizeof(INT);

	for (int i = 0; i < tools; i++) {
		tbAddBitmap.nID = toolids[i]; // 工具栏的清空位图资源
		SendMessage(hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbAddBitmap);
	}

	// 定义按钮
	TBBUTTON tbButtons[10] = {};

	for (int i = 0; i < tools; i++) {
		tbButtons[i] = { MAKELONG(i, 0), toolids[i], TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tooltips[i] };
	}

	// 初始化状态管理
	InitState(cs, 10, CHOOSE); // 初始化绘图状态

	for (int i = 0; i < tools; i++) {
		AddIdToState(cs, toolids[i]); // 默认选择按钮
	}

	// 添加按钮到工具栏
	SendMessage(hToolBar, TB_ADDBUTTONS, sizeof(tbButtons) / sizeof(TBBUTTON), (LPARAM)&tbButtons);
	SendMessage(hToolBar, TB_CHECKBUTTON, CHOOSE, TRUE);
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
	InitWindowRect(wrect, mainrect, toolbarrect, systemode.worktype);

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
			FileOpenAndSave s = SaveGTXFile(hWnd);
			switch (s) {
			case SAVEOK:
				break;
			default:
				UpdateStatusBarText(L"文件保存失败");
				break;
			}
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
			FileOpenAndSave s = OpenGTXFile(hWnd);

			switch (s) {
			case OPENOK:
				FitCanvasCoordinate(); // 适应坐标系
				RedrawFixedContent(hCanvasWnd, hdcMemFixed);
				NeedRedraw();
				break;
			case DIALOGOPENFAILE:
				UpdateStatusBarText(L"对话框打开失败");
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
			break;
		}
		case CLEARIMG:
		{
			setType(mst, mst.type);
			ClearDrawing(&(drawing)); // 清空当前绘制的图形
			ClearStoreImg(&allImg); // 清空图形
			ClearContent(hdcMemPreview); // 清空预览画布
			ClearContent(hdcMemCoS);		// 清空计算或选中画布
			RedrawFixedContent(hCanvasWnd, hdcMemFixed);
			NeedRedraw();
			break;
		}
		case DRAW_LINE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWLINE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_LINE);
			setType(mst, DRAWLINE);
			setDrawInfoType(&(drawing.info), LINE);

			break;
		case DRAW_CIRCLE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWCIRCLE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_CIRCLE);
			setType(mst, DRAWCIRCLE);
			setDrawInfoType(&(drawing.info), CIRCLE);

			break;
		case CHOOSE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, CHOOSEIMG, 0);
			SetToolBarCheck(hToolBar, cs, CHOOSE);
			setType(mst, CHOOSEIMG);

			break;
		case DRAW_RECT:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWRECTANGLE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_RECT);
			setType(mst, DRAWRECTANGLE);
			setDrawInfoType(&(drawing.info), RECTANGLE);

			break;
		case DRAW_CURVE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWCURVE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_CURVE);
			setType(mst, DRAWCURVE);
			setDrawInfoType(&(drawing.info), CURVE);

			break;
		case DRAW_BCURVE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWBCURVE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_BCURVE);
			setType(mst, DRAWBCURVE);
			setDrawInfoType(&(drawing.info), BCURVE);

			break;
		case DRAW_MUTILINE:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWMULTILINE, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_MUTILINE);
			setType(mst, DRAWMULTILINE);
			setDrawInfoType(&(drawing.info), MULTILINE);

			break;
		case DRAW_FMULTI:
			SendMessage(hSideWnd, CUSTOM_DRAWSTATE_CHANGE, DRAWFMULTI, 0);
			SetToolBarCheck(hToolBar, cs, DRAW_FMULTI);
			setType(mst, DRAWFMULTI);
			setDrawInfoType(&(drawing.info), FMULTILINE);

			break;
		case FITSCREEN:
			ClearContent(hdcMemPreview);
			FitCanvasCoordinate(); // 适应坐标系
			RedrawFixedContent(hCanvasWnd, hdcMemFixed);
			RedrawCoSContent(hCanvasWnd, hdcMemCoS);
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
		RefreshWindowRect(wrect, width, height, systemode.worktype); // 更新窗口矩形

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

		// TODO: 工作区完成可以拖动多个文件
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
			SaveGTXFile(hWnd);
		}

		// 将GTX文件加载到画布中
		FileToStoreImg(&allImg, filePath);
		FitCanvasCoordinate(); // 适应坐标系
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

		// 创建线宽滑块
		WidthSlider = CreateWindow(L"CustomSliderClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 140, 150, 60, hSWnd, (HMENU)3, hInst, NULL);

		SendMessage(WidthSlider, CUSTOM_VALUE_CHANGE, NULL, (LPARAM)DEFAULTLINEWID);
		SendMessage(WidthSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"线宽:");

		ColorSlider = CreateWindow(L"ColorPickerClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 220, 150, 65, hSWnd, (HMENU)4, hInst, NULL);

		SendMessage(ColorSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"颜色:");
		SendMessage(ColorSlider, CUSTOM_SET_NUM, NULL, (LPARAM)COLORSLIDE);

		bgColorSlider = CreateWindow(L"ColorPickerClass", NULL, WS_CHILD | WS_VISIBLE,
			25, 300, 150, 65, hSWnd, (HMENU)5, hInst, NULL);

		SendMessage(bgColorSlider, CUSTOM_TITLE_CHANGE, NULL, (LPARAM)L"背景:");
		SendMessage(bgColorSlider, CUSTOM_SET_NUM, NULL, (LPARAM)BGSLIDE);
		SendMessage(bgColorSlider, CUSTOM_SET_COLOR, (WPARAM)2, (LPARAM)DEFAULTPADCOR);

		DWORD dwError = GetLastError();

		break;
	}
	case CUSTOM_COLOR_CHANGE:
	{
		switch ((INT)lParam) {
		case COLORSLIDE:
			SetColorWithColorRef(&customProperty, (COLORREF)wParam);
			break;
		case BGSLIDE:
			SetBgColorWithColorRef(&customProperty, (COLORREF)wParam);
			break;
		}
		// 重绘drawing
		SendMessage(hCanvasWnd, CUSTOM_REDRAW_DRAWING, NULL, NULL);
		break;
	}
	case CUSTOM_WIDTH_CHANGE:
	{
		SetWidth(&customProperty, (int)wParam);
		// 重绘drawing
		SendMessage(hCanvasWnd, CUSTOM_REDRAW_DRAWING, NULL, NULL);
		break;
	}
	case CUSTOM_TYPE_CHANGE:
	{
		SetType(&customProperty, (gctype)wParam);
		// 重绘drawing
		SendMessage(hCanvasWnd, CUSTOM_REDRAW_DRAWING, NULL, NULL);
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

		if (InDraw()) {
			SendMessage(hCanvasWnd, WM_RBUTTONDOWN, NULL, (LPARAM)RBUTTOMDOWNCUSTOM);
		}

		if (InState(mst, CHOOSEN)) {
			// 清除选中状态
			ClearCSState();
			NeedRedraw();
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
void ClearContent(HDC hdc) {
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
		RedrawCoSContent(hCanvasWnd, hdcMemCoS);
		NeedRedraw();
		break;
	}
	case WM_COMMAND:
	{
		HMENU hMenu = GetMenu(hCWnd);
		MENUITEMINFO itemInfo;
		ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
		itemInfo.cbSize = sizeof(MENUITEMINFO);
		itemInfo.fMask = MIIM_DATA;
		if (GetMenuItemInfo(hMenu, LOWORD(wParam), FALSE, &itemInfo)) {
			// 获取附加的数据，即处理函数指针
			MenuItemHandler handler = (MenuItemHandler)itemInfo.dwItemData;
			if (handler) {
				handler(hCWnd);
			}
		}
		break;
	}
	case WM_PAINT:
	{
		switch (systemode.worktype) {
		case CanvasMode:
			break;
		case CoordinateMode:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hCWnd, &ps);

			// 获取窗口大小
			RECT canvasRect;
			GetClientRect(hCWnd, &canvasRect);

			int canvasWidth = canvasRect.right - canvasRect.left;
			int canvasHeight = canvasRect.bottom - canvasRect.top;

			// 先将固定的图形叠加到预览hdc中到窗口
			BitBlt(hdcMemPreview, 0, 0, canvasWidth, canvasHeight, hdcMemFixed, 0, 0, SRCAND);

			// 将CoS窗口叠加到预览hdc中到窗口, 辅助线段或计算结果保存到此
			BitBlt(hdcMemPreview, 0, 0, canvasWidth, canvasHeight, hdcMemCoS, 0, 0, SRCAND);

			// 然后将预览内容叠加到窗口上
			BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMemPreview, 0, 0, SRCCOPY);

			EndPaint(hCWnd, &ps);
			break;
		}
	}
	break;
	case CUSTOM_REDRAW_DRAWING:
	{
		ClearContent(hdcMemPreview);
		drawDrawing(hdcMemPreview, &drawing, &customProperty);
		NeedRedraw();
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);

		switch (mst.type) {
		case CHOOSEIMG:
		{
			csdraw.index = GetChooseIndex(point);
			if (csdraw.index != -1) {
				setType(mst, CHOOSEN);
				// 将图元从 allImg 中取出
				PopStoreImgToCSDraw(allImg, csdraw);
				// 重绘Fix, Preview
				RedrawFixedContent(hCanvasWnd, hdcMemFixed);
				ClearContent(hdcMemPreview);
				ClearContent(hdcMemCoS);
				drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
				drawCosCSDraw(hdcMemCoS, &csdraw);
				NeedRedraw();
			}
		}
		break;
		case CHOOSEN:
		{
			SetChosen(PointInCSDraw(point));
			if (IsChosen()) break;

			int index = GetChooseIndex(point);
			if (index != csdraw.index) {
				RestoreCSDraw(allImg, csdraw);
				csdraw.index = index;
				if (index != -1) {
					// 将图元从 allImg 中取出
					PopStoreImgToCSDraw(allImg, csdraw);
				}
				else {
					setType(mst, CHOOSEIMG);
				}
				// 重绘Fix, Preview
				RedrawFixedContent(hCanvasWnd, hdcMemFixed);
				RedrawCoSContent(hCanvasWnd, hdcMemCoS);
				ClearContent(hdcMemPreview);
				drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
				NeedRedraw();
			}
		}
		break;
		case DRAWLINE:
		{
			if (!InDraw()) {
				StartDraw();
				LineFirstPoint(point);
			}
			else {
				// 在固定图像上绘制线
				DrawLine(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				LineNextPoint(point);
				EndDraw();
			}
			break;
		}
		case DRAWCIRCLE:
		{
			if (!InDraw()) {
				StartDraw();
				CircleFirstPoint(point);
			}
			else {
				// 画圆
				DrawCircle(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				// 画圆,第一个点为圆心，第二个点为半径
				CircleNextPoint(point);
				EndDraw();
			}
			break;
		}
		case DRAWRECTANGLE:
		{
			if (!InDraw()) {
				StartDraw();
				RectFirstPoint(point);
			}
			else {
				// 画矩形
				DrawRectangle(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				// 保存矩形
				RectNextPoint(point);
				EndDraw();
			}
			break;
		}
		case DRAWFMULTI:
		case DRAWMULTILINE:
		{
			// 多义线绘制
			if (!InDraw()) {
				StartDraw();
				MultiPFirstPoint(point);
			}
			else {
				// 画一条线
				DrawLine(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				// 将上一个点保存到DrawInfo中
				MultiPNextPoint(point);
			}
			break;
		}
		case DRAWBCURVE:
		{
			// B样条绘制
			// 将当前点保存到DrawInfo中
			if (!InDraw()) {
				StartDraw();
				// 初始化 mst.
				BCurveFirstPoint(point);
			}
			// 1个点
			else if(drawing.info.multipoint.numPoints == 1){
				// 将当前点保存到DrawInfo中
				DrawXLine(hdcMemCoS, mst.lastLButtonDown, point, HELPLINECORLOR, 1);
				BCurveNextPoint(point);
			}
			// 两个点
			else {
				BCurveNextPoint(point);

				ScanMultipoint(&(drawing.info.multipoint));
				POINT* pts;
				if (drawing.info.multipoint.numPoints < BSPLINE + 1) {
					pts = mapMyPoints(drawing.info.multipoint.points, drawing.info.multipoint.numPoints, drawing.info.multipoint.numPoints);
					DrawABCurveHelp(hdcMemCoS, pts[0], pts[1], pts[2], BSPLINE);
					delete[] pts;
					break;
				}
				pts = mapLastMyPoints(drawing.info.multipoint.points, BSPLINE + 1, drawing.info.multipoint.numPoints);
				POINT* pts2 = new POINT[3];
				for (int i = 0; i < 3; i++) {
					pts2[i] = pts[i + BSPLINE - 2];
				}
				DrawABCurveHelp(hdcMemCoS, pts2[0], pts2[1], pts2[2], BSPLINE);
				DrawFBCurve(hdcMemFixed, pts, BSPLINE, &customProperty);
				delete[] pts;
				delete[] pts2;
			}
			break;
		}
		case DRAWCURVE:
		{
			// 曲线绘制, 先设置起点和终点
			if (!InDraw()) {
				StartDraw();
				MultiPFirstPoint(point);
			}
			else {
				MultiPNextPoint(point);
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
				ClearContent(hdcMemPreview);
				if (csdraw.index == -1) break;
				DrawInfo choose = allImg.img[csdraw.index];
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

		LButtonDown(mst, point);
		break;
	}
	// 抬起绘制完毕
	case WM_LBUTTONUP:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);
		if (!HFPoint(&point)) break;

		HDC hdc = GetDC(hCWnd);
		// 创建画笔
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // 黑色画笔
		switch (mst.type) {
		case CHOOSEIMG:
		{
			
			break;
		}
		case CHOOSEN:
		{
			if (LButtonClick(point)) {
				ClearContent(hdcMemCoS);
				if (csdraw.index == -1) {
					break;
				}
				DrawInfo choose = allImg.img[csdraw.index];
				switch (choose.type) {
				case CIRCLE:
				{
					// 在预览窗口绘制圆心
					MyPoint mp = choose.circle.center;
					POINT pt = mapCoordinate(coordinate, mp.x, mp.y);
					// 画圆心
					SelectObject(hdcMemCoS, hPen);
					SelectObject(hdcMemCoS, hBlackBrush);
					Ellipse(hdcMemCoS, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
					// 显示坐标
					ShowPointInWindow(hdcMemCoS, mp);
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
			}
			else {

			}
			SetChosen(false);
			break;
		}
		case DRAWLINE:
		{
			// 如果当前的点与上次的点位置相同，则为两点画线
			if (InDraw() && !LButtonClick(point)) {
				ClearContent(hdcMemPreview);
				// 在固定图像上绘制线
				DrawLine(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				// 画线
				LineNextPoint(point);
				EndDraw();
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		case DRAWCIRCLE:
		{
			// 画圆
			if (InDraw() && !LButtonClick(point)) {
				// 画圆
				ClearContent(hdcMemPreview);
				DrawCircle(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				// 画圆,第一个点为圆心，第二个点为半径
				CircleNextPoint(point);
				EndDraw();
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		case DRAWRECTANGLE:
		{
			// 画矩形
			if (InDraw() && !LButtonClick(point)) {
				// 画矩形
				ClearContent(hdcMemPreview);
				DrawRectangle(hdcMemFixed, mst.lastLButtonDown, point, &customProperty);
				RectNextPoint(point);
				EndDraw();
				// 触发重绘
				NeedRedraw();
			}
			break;
		}
		default:
			break;
		}
		LButtonUp(mst, point);
		break;
	}
	case WM_MOUSELEAVE:
	{
		// 鼠标离开窗口时的处理
		switch (mst.type) {
		case MMOUSEMOVE:
		{
			// 触发 WM_MBUTTONUP
			SendMessage(hCanvasWnd, WM_MBUTTONUP, NULL, NULL);

			break;
		}
		case CHOOSEN:
		case CHOOSEIMG:
			break;
		default:
			// 设置离开时的坐标
			MyPoint mp;

			if (HFPoint(&(mst.lastMouseP))) {
				PointToCoordinate(coordinate, mst.lastMouseP, mp.x, mp.y);
				drawing.lastRem = mp;
			}
			
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
			// 如果还没开始绘制，则回到选择状态
			if (!InDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
			}
			// 开始绘制
			else {
				ClearContent(hdcMemPreview);
				NeedRedraw();
			}
			
			break;
		}
		case DRAWMULTILINE:
		{
			// 完成绘制逻辑
			if (!InDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				break;
			}
			if (OnlyOnePoint()) {
				ClearDrawing(&(drawing));
				ClearContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			MultiPDone();
			ClearContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case DRAWFMULTI:
		{
			// 完成绘制逻辑
			if (!InDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				break;
			}
			if (OnlyOnePoint()) {
				ClearDrawing(&(drawing));
				ClearContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			// 画一条第一个点到最后一个点的直线
			POINT p1, p2;
			p1 = mapCoordinate(coordinate, drawing.info.multipoint.points[0].x, drawing.info.multipoint.points[0].y);
			int last = drawing.info.multipoint.endNum - 1;
			p2 = mapCoordinate(coordinate, drawing.info.multipoint.points[last].x, drawing.info.multipoint.points[last].y);
			DrawLine(hdcMemFixed, p1, p2, &customProperty);

			POINT* pts = mapMyPoints(drawing.info.multipoint.points, drawing.info.multipoint.numPoints, drawing.info.multipoint.endNum);
			DrawFMultiLine(hdcMemFixed, pts, drawing.info.multipoint.numPoints, &customProperty);
			delete[] pts;

			MultiPDone();

			ClearContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case DRAWBCURVE:
		{
			if (!InDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				break;
			}
			if (BCurvePointCannotDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				ClearDrawing(&(drawing));
				ClearContent(hdcMemPreview);
				ClearContent(hdcMemCoS);
				NeedRedraw();
				break;
			}
			BCurveDone();

			// 清空状态
			ClearContent(hdcMemPreview);
			ClearContent(hdcMemCoS);
			NeedRedraw();
			break;
		}
		case DRAWCURVE:
		{
			// 完成绘制逻辑
			// 保存绘图信息
			if (!InDraw() && !CUSTOMRBUTTOMDOWN(lParam)) {
				SendMessage(hWnd, WM_COMMAND, (WPARAM)CHOOSE, 0);
				break;
			}
			if (OnlyOnePoint()) {
				ClearDrawing(&(drawing));
				ClearContent(hdcMemPreview);
				NeedRedraw();
				break;
			}
			// 绘制到固定画布上
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing.info.multipoint.numPoints];
			int count = 0;
			for (int i = 0; i < drawing.info.multipoint.endNum; i++) {
				MyPoint pt = drawing.info.multipoint.points[i];
				if (pt.x == ILLEGELMYPOINT || pt.y == ILLEGELMYPOINT) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing.info.multipoint.numPoints)
			{
				// GDI+绘图
				Graphics graphics(hdcMemFixed);
				int color = customProperty.color;

				Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), customProperty.width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing.info.multipoint.numPoints);
			}

			delete[] gdiplusPoints;

			// 保存绘图信息
			MultiPDone();
			// 清空状态
			ClearContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case MMOUSEMOVE:
		{
			// 清除状态
			RestoreFormLastType(mst);
			ClearContent(hdcMemPreview);
			NeedRedraw();
			break;
		}
		case CHOOSEN:
		{
			if (csdraw.index == -1) break;
			DrawInfo choose = allImg.img[csdraw.index];
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
				ClearContent(hdcMemPreview);
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

		EndDraw();
		break;
	}
	// 更新预览状态
	case WM_MOUSEMOVE:
	{
		// 如果尚未启用鼠标跟踪，则启用
		if (!mouseTracking) {
			// 去除上一次坐标
			drawing.lastRem = { ILLEGELMYPOINT, ILLEGELMYPOINT };
			EnableMouseTracking(hCWnd);
			mouseTracking = TRUE;
		}
		// 获取鼠标点击的位置
		POINT point = getClientPos(lParam);

		MyPoint mp;
		PointToCoordinate(coordinate, point, mp.x, mp.y);
		// 鼠标位置状态栏
		UpdateStatusBarCoordinates(mp.x, mp.y);

		switch (mst.type) {
		case CHOOSEN:
		{
			// 获得移动距离
			int x = point.x - mst.lastMouseP.x;
			int y = point.y - mst.lastMouseP.y;
			if (IsChosen()) {
				ClearContent(hdcMemPreview);
				// 更新图元位置
				MoveCSDrawInPoint(x, y);
				RedrawCoSContent(hCWnd, hdcMemCoS);
				drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
				NeedRedraw();
			}
			
			break;
		}
		case MMOUSEMOVE:
		{
			ClearContent(hdcMemPreview);
			// 获得移动距离
			int x = point.x - mst.lastMouseP.x;
			int y = point.y - mst.lastMouseP.y;
			if (IsChosen()) {
				// 更新图元位置
				MoveCSDrawInPoint(x, y);
				RedrawCoSContent(hCWnd, hdcMemCoS);
				drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
			}
			else {
				// 更新坐标系中心
				coordinate.center.x += x;
				coordinate.center.y += y;
				RedrawFixedContent(hCWnd, hdcMemFixed);
				RedrawCoSContent(hCWnd, hdcMemCoS);
				drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
			}
			
			// 触发重绘
			NeedRedraw();
			break;
		}
		case DRAWLINE:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			DrawLine(hdcMemPreview, mst.lastLButtonDown, point, &customProperty);
			// 触发重绘
			NeedRedraw();

			break;
		}
		case DRAWCIRCLE:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			// 画圆
			DrawCircle(hdcMemPreview, mst.lastLButtonDown, point, &customProperty);
			// 触发重绘
			NeedRedraw();

			break;
		}
		case DRAWRECTANGLE:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			// 画矩形
			DrawRectangle(hdcMemPreview, mst.lastLButtonDown, point, &customProperty);
			// 触发重绘
			NeedRedraw();

			break;
		}
		case DRAWMULTILINE:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			// 画线
			DrawLine(hdcMemPreview, mst.lastLButtonDown, point, &customProperty);
			// 触发重绘
			NeedRedraw();

			break;
		}
		case DRAWFMULTI:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			MyPoint firstM = drawing.info.multipoint.points[0];
			POINT first = mapCoordinate(coordinate, firstM.x, firstM.y);

			// 填充
			if (drawing.info.multipoint.numPoints >= 2) {
				POINT* pts = mapPointsAddOne(drawing.info.multipoint.points, drawing.info.multipoint.numPoints, drawing.info.multipoint.endNum, point);
				PadColor(hdcMemPreview, pts, drawing.info.multipoint.numPoints + 1, customProperty.bgcolor, customProperty.type);
				delete[]pts;
			}
			
			// 画虚线
			if (drawing.info.multipoint.numPoints > 1) {
				DrawXLine(hdcMemPreview, point, first, &customProperty);
			}
			// 画线
			DrawLine(hdcMemPreview, mst.lastLButtonDown, point, &customProperty);
			// 触发重绘
			NeedRedraw();
			
			break;
		}
		case DRAWBCURVE:
		{
			if (!InDraw()) break;
			ClearContent(hdcMemPreview);
			if (drawing.info.multipoint.numPoints == 1) {
				DrawXLine(hdcMemPreview, mst.lastLButtonDown, point, HELPLINECORLOR, 1);
				NeedRedraw();
				break;
			}
			else {
				ScanMultipoint(&(drawing.info.multipoint));
				POINT* pts;
				if (drawing.info.multipoint.numPoints < BSPLINE) {
					pts = mapLastMyPointsAddOne(drawing.info.multipoint.points, 2, drawing.info.multipoint.numPoints, point);
					DrawABCurveHelp(hdcMemPreview, pts[0], pts[1], pts[2], BSPLINE);
					NeedRedraw();
					delete[] pts;
					break;
				}
				pts = mapLastMyPointsAddOne(drawing.info.multipoint.points, BSPLINE, drawing.info.multipoint.endNum, point);
				POINT* pts2 = new POINT[3];
				for (int i = 0; i < 3; i++) {
					pts2[i] = pts[i + BSPLINE - 2];
				}
				DrawABCurveHelp(hdcMemPreview, pts2[0], pts2[1], pts2[2], BSPLINE);
				DrawFBCurve(hdcMemPreview, pts, BSPLINE, &customProperty);
				NeedRedraw();
				delete[] pts;
				delete[] pts2;
			}
			break;
		}
		case DRAWCURVE:
		{
			if (!InDraw()) break;
			// GDI+绘图
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing.info.multipoint.numPoints + 1];
			int count = 0;
			for (int i = 0; i < drawing.info.multipoint.endNum; i++) {
				MyPoint pt = drawing.info.multipoint.points[i];
				if (pt.x == ILLEGELMYPOINT || pt.y == ILLEGELMYPOINT) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing.info.multipoint.numPoints) {
				ClearContent(hdcMemPreview);
				gdiplusPoints[count] = Gdiplus::Point(point.x, point.y);
				// 画曲线
				Graphics graphics(hdcMemPreview);
				int color = customProperty.color;

				Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), customProperty.width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing.info.multipoint.numPoints + 1);
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
				ClearContent(hdcMemPreview);
				// 预览垂线,为选中的线段的垂线
				if (csdraw.index == -1) break;
				DrawInfo choose = allImg.img[csdraw.index];
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
		MouseMove(mst, point);
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
		StartMMove();
		POINT point = getClientPos(hCWnd);
		SetChosen(PointInCSDraw(point));
		setTypeWithLastType(mst, MMOUSEMOVE);
		ClearContent(hdcMemPreview);
		MMouseDown(mst, point);

		break;
	}
	case WM_MBUTTONUP:
	{
		// 重置状态
		POINT point = getClientPos(hCWnd);
		RestoreFormLastType(mst);
		SetChosen(false);
		MMouseUp(mst, point);
		EndMMove();
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// 获取鼠标点击的位置
		POINT point = getClientPos(hCWnd);

		MyPoint mp;
		PointToCoordinate(coordinate, point, mp.x, mp.y);

		if (MyPointCSDraw(mp)) {
			double radius = GetRadiusFromWParam(wParam);
			ZoomCSDrawMyPoint(mp, radius);
			RedrawCoSContent(hCWnd, hdcMemCoS);
			ClearContent(hdcMemPreview);
			drawCSDraw(hdcMemPreview, &csdraw, &customProperty);
			NeedRedraw();
			break;
		}

		// 鼠标位置状态栏
		UpdateStatusBarCoordinates(mp.x, mp.y);

		// 放大时,坐标系radius减小，缩小时，坐标系radius增大
		RefreshRadius(wParam);

		// 更新Radius
		UpdateStatusBarRadius(coordinate.radius);

		RedrawFixedContent(hCWnd, hdcMemFixed); // 重绘固定内容
		RedrawCoSContent(hCWnd, hdcMemCoS);
		ClearContent(hdcMemPreview);
		drawCSDraw(hdcMemPreview, &csdraw, &customProperty);

		// 触发鼠标移动事件
		LPARAM l = MAKELPARAM(point.x, point.y);
		WPARAM w = 0;
		SendMessage(hCWnd, WM_MOUSEMOVE, w, l);
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

void FitCanvasCoordinate() {
	// 获得画布窗口大小
	RECT canvasRect;
	GetClientRect(hCanvasWnd, &canvasRect);
	FitCoordinate(coordinate, allImg, canvasRect);
}

void InitializeBuffers(HWND hWnd) {
	// 获取画布窗口大小
	RECT canvasRect;
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

	hdcMemCoS = CreateCompatibleDC(hdcScreen);
	hbmmemCoS = CreateCompatibleBitmap(hdcScreen, canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top);
	hbmOldCoS = (HBITMAP)SelectObject(hdcMemCoS, hbmmemCoS);

	// 设置为白色背景
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdcMemFixed, &canvasRect, hBrush);
	FillRect(hdcMemPreview, &canvasRect, hBrush);
	FillRect(hdcMemCoS, &canvasRect, hBrush);

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

	SelectObject(hdcMemCoS, hbmOldCoS);
	DeleteObject(hbmmemCoS);
	DeleteDC(hdcMemCoS);
}

void CalAndShowPoint() {
	// 计算交点
	CalculateImg(allImg, csdraw.index);
	// 显示交点
	ShowAllCalPoint(hdcMemPreview, coordinate);
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

void ClearCSState() {
	RestoreCSDraw(allImg, csdraw);
	ClearContent(hdcMemPreview);
	ClearContent(hdcMemCoS);
	RedrawFixedContent(hCanvasWnd, hdcMemFixed);
}