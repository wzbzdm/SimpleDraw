#pragma once

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "SimpleDraw.h"
#pragma comment(lib, "Comctl32.lib")

#define SIDEBORDER	25
#define SLIDEWIDTH (OTHERW - 2*SIDEBORDER)

#define WM_GET_COLOR			(WM_USER + 1)
#define CUSTOM_WIDTH_CHANGE		(WM_USER + 2)
#define CUSTOM_COLOR_CHANGE		(WM_USER + 3)
#define CUSTOM_TYPE_CHANGE		(WM_USER + 4)
#define CUSTOM_DRAWSTATE_CHANGE	(WM_USER + 5)
#define CUSTOM_TITLE_CHANGE		(WM_USER + 6)
#define CUSTOM_SET_NUM			(WM_USER + 7)
#define CUSTOM_VALUE_CHANGE		(WM_USER + 8)


LRESULT CALLBACK TrackbarSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);

class CustomSlider {
public:
	HWND hWnd; // 自定义滑块窗口句柄
	HWND hName; // 名称标签
	HWND hTrackbar; // 滑块控件
	HFONT hFont;	// 字体
	INT number; // 当前滑块编号
	INT value; // 当前滑块值

	CustomSlider(HWND parent) {
		// 创建滑块窗口
		hWnd = parent;

		// 创建名称标签
		hName = CreateWindow(L"STATIC", L"STATIC:", WS_CHILD | WS_VISIBLE,
			5, 5, 100, 20, hWnd, NULL, NULL, NULL);

		// 创建滑块控件
		hTrackbar = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_TOOLTIPS,
			0, 30, SLIDEWIDTH, 25, hWnd, NULL, NULL, NULL);

		// 创建字体
		hFont = CreateFontW(
			16,                  // 字体高度
			0,                   // 字体宽度（默认）
			0,                   // 倾斜度
			0,                   // 方向
			FW_MEDIUM,          // 粗体
			FALSE,              // 斜体
			FALSE,              // 下划线
			FALSE,              // 删除线
			DEFAULT_CHARSET,    // 字符集
			OUT_DEFAULT_PRECIS, // 输出精度
			CLIP_DEFAULT_PRECIS,// 剪裁精度
			ANTIALIASED_QUALITY,// 渲染质量
			DEFAULT_PITCH | FF_DONTCARE, // 间距和家族
			L"Arial"            // 字体名称
		);

		// 应用字体到静态控件
		SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);

		if (hTrackbar) {
			// 设置子类化，确保在滑块获取焦点时移除虚线框
			SetWindowSubclass(hTrackbar, TrackbarSubclassProc, 0, 0);
		}

		SendMessage(hTrackbar, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100)); // 设置范围
		SendMessage(hTrackbar, TBM_SETPOS, TRUE, 1); // 默认值
		value = 1; // 初始化值

		// 处理重绘
		SendMessage(hWnd, WM_PAINT, 0, 0);
	}

	// 设置滑块的默认大小
	INT SetDefaultValue(int value) {
		SendMessage(hTrackbar, TBM_SETPOS, TRUE, value);

		return 0;
	}

	INT SetTitle(LPCWSTR title) {
		SetWindowText(hName, title);
		// 应用字体到静态控件
		SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);

		return 0;
	}

	~CustomSlider() {
		DeleteObject(hFont);
	}
};

// 子类化过程函数，拦截 WM_SETFOCUS 消息
LRESULT CALLBACK TrackbarSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (message) {
	case WM_SETFOCUS:
		// 将焦点设置到父窗口，避免滑块显示虚线框
		SetFocus(GetParent(hWnd));
		return 0;

	case WM_KILLFOCUS:
		// 如果需要，可以在此处理失去焦点时的行为
		// 比如更新界面，或做一些状态保存
		break;

	case WM_DESTROY:
		// 移除子类化过程
		RemoveWindowSubclass(hWnd, TrackbarSubclassProc, uIdSubclass);
		break;
	}

	// 继续处理其他消息
	return DefSubclassProc(hWnd, message, wParam, lParam);
}


LRESULT CALLBACK SliderWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static CustomSlider* slider = nullptr; // 保存自定义滑块指针
	static HBRUSH hBrushStaticBackground; // 静态控件背景画刷
	switch (message) {
	case WM_CREATE: {
		// 初始化自定义滑块
		slider = new CustomSlider(hWnd);
		hBrushStaticBackground = CreateSolidBrush(SIDEBARCOLOR); // 创建背景画刷
		break;
	}
	case CUSTOM_TITLE_CHANGE:
	{
		LPCWSTR title = (LPCWSTR)lParam;
		slider->SetTitle(title);
		break;
	}
	case CUSTOM_VALUE_CHANGE:
	{
		int value = (int)lParam;
		slider->SetDefaultValue(value);
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		HWND hStatic = (HWND)lParam;

		// 设置静态控件的文本颜色
		SetTextColor(hdcStatic, RGB(0, 0, 0)); // 黑色文字
		// 设置静态控件的背景模式和颜色
		SetBkMode(hdcStatic, TRANSPARENT);  // 背景透明
		SetBkColor(hdcStatic, SIDEBARCOLOR);  // 设置背景颜色

		return (INT_PTR)hBrushStaticBackground; // 返回背景画刷
	}
	case WM_HSCROLL:
	{
		if (lParam == (LPARAM)slider->hTrackbar) {
			// 获取滑块当前位置
			int pos = SendMessage(slider->hTrackbar, TBM_GETPOS, 0, 0);
			if (slider->value != pos) {
				slider->value = pos;
				SendMessage(GetParent(hWnd), CUSTOM_WIDTH_CHANGE, pos, 0);
				InvalidateRect(hWnd, NULL, TRUE); // 触发重绘
			}
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 清除背景
		FillRect(hdc, &ps.rcPaint, hBrushStaticBackground); // 使用自定义背景画刷

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		DeleteObject(hBrushStaticBackground); // 清理资源
		delete slider; // 清理内存
		slider = nullptr;
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

class ColorPicker {
public:
	HWND hWnd;		// 窗口句柄
	HWND bhWnd;		// 按钮句柄
	HWND hName; // 名称标签
	HFONT hFont;	
	COLORREF lastColor; // 上一次颜色
	COLORREF currentColor; // 当前颜色
	HBITMAP hBitmap;  // 位图句柄
	INT number; // 当前滑块编号

	ColorPicker(HWND hwnd) {
		currentColor = RGB(0, 0, 0); // 默认颜色为黑色
		this->hWnd = hwnd;

		this->lastColor = currentColor;
		// 加载位图资源
		hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_COLOR));

		this->hName = CreateWindow(L"STATIC", L"STATIC:", WS_CHILD | WS_VISIBLE,
			5, 5, 100, 20, hWnd, NULL, NULL, NULL);

		// 创建字体
		hFont = CreateFontW(
			16,                  // 字体高度
			0,                   // 字体宽度（默认）
			0,                   // 倾斜度
			0,                   // 方向
			FW_MEDIUM,          // 粗体
			FALSE,              // 斜体
			FALSE,              // 下划线
			FALSE,              // 删除线
			DEFAULT_CHARSET,    // 字符集
			OUT_DEFAULT_PRECIS, // 输出精度
			CLIP_DEFAULT_PRECIS,// 剪裁精度
			ANTIALIASED_QUALITY,// 渲染质量
			DEFAULT_PITCH | FF_DONTCARE, // 间距和家族
			L"Arial"            // 字体名称
		);

		// 应用字体到静态控件
		SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);

		this->bhWnd = CreateWindow(
			L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_BITMAP,
			110, 25, 40, 40, hWnd, (HMENU)1, NULL, NULL
		);

		// 设置按钮的位图
		SendMessage(bhWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	}

	INT SetTitle(LPCWSTR title) {
		SetWindowText(hName, title);
		// 应用字体到静态控件
		SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);

		return 0;
	}

	INT SetNumber(INT num) {
		this->number = num;
		return 0;
	}

	COLORREF GetColor() {
		return currentColor;
	}

	void ShowColorDialog() {
		// 创建颜色选择结构
		CHOOSECOLOR cc;
		static COLORREF acrCustClr[16] = {
			RGB(255, 0, 0),			// 红色
			RGB(0, 255, 0),			// 绿色
			RGB(0, 0, 255),			// 蓝色
			RGB(255, 255, 0),		// 黄色
			RGB(255, 0, 255),		// 品红
			RGB(0, 255, 255),		// 青色
			RGB(192, 192, 192),		// 灰色
			RGB(128, 0, 0),			// 深红
			RGB(0, 128, 0),			// 深绿
			RGB(0, 0, 128),			// 深蓝
			RGB(128, 128, 0),		// 橄榄色
			RGB(128, 0, 128),		// 紫色
			RGB(0, 128, 128),		// 深青色
			RGB(0, 0, 0),			// 黑色
			RGB(255, 255, 255),		// 白色
			RGB(255, 165, 0)		// 橙色
		};

		ZeroMemory(&cc, sizeof(cc));
		cc.lStructSize = sizeof(cc);
		cc.hwndOwner = hWnd;
		cc.rgbResult = currentColor; // 当前颜色
		cc.lpCustColors = acrCustClr; // 自定义颜色
		cc.Flags = CC_FULLOPEN | CC_RGBINIT;

		lastColor = currentColor; // 保存上一次颜色

		// 显示颜色选择对话框
		if (ChooseColor(&cc)) {
			currentColor = cc.rgbResult; // 更新当前颜色
			InvalidateRect(hWnd, NULL, TRUE); // 触发重绘
		}
	}

	bool ColorChanged() {
		return lastColor != currentColor;
	}

	~ColorPicker() {
		if (hBitmap) {
			DeleteObject(hBitmap);
		}
	}
};

// 窗口过程函数
LRESULT CALLBACK ColorPickerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static ColorPicker* picker = nullptr;
	static HBRUSH hBrushStaticBackground; // 静态控件背景画刷
	switch (message) {
	case WM_CREATE: {
		picker = new ColorPicker(hWnd);
		hBrushStaticBackground = CreateSolidBrush(SIDEBARCOLOR); // 创建背景画刷
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == 1) { // 按钮 ID
			picker->ShowColorDialog();
			if (picker->ColorChanged()) {
				SendMessage(GetParent(hWnd), CUSTOM_COLOR_CHANGE, picker->GetColor(), picker->number);
			}
		}
		break;
	}
	case CUSTOM_TITLE_CHANGE:
	{
		LPCWSTR title = (LPCWSTR)lParam;
		picker->SetTitle(title);
		break;
	}
	case CUSTOM_SET_NUM:
	{
		INT num = (INT)lParam;
		picker->SetNumber(num);
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		HWND hStatic = (HWND)lParam;

		// 设置静态控件的文本颜色
		SetTextColor(hdcStatic, RGB(0, 0, 0)); // 黑色文字
		// 设置静态控件的背景模式和颜色
		SetBkMode(hdcStatic, TRANSPARENT);  // 背景透明
		SetBkColor(hdcStatic, SIDEBARCOLOR);  // 设置背景颜色

		return (INT_PTR)hBrushStaticBackground; // 返回背景画刷
	}
	case WM_GET_COLOR:
	{
		return picker->GetColor();
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 清除背景
		FillRect(hdc, &ps.rcPaint, hBrushStaticBackground); // 使用自定义背景画刷

		// 绘制当前颜色
		RECT rect;
		GetClientRect(picker->hWnd, &rect);
		rect.top = 25;
		rect.right = 100; // 左侧区域宽度
		FillRect(hdc, &rect, CreateSolidBrush(picker->currentColor));

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		DeleteObject(hBrushStaticBackground); // 清理资源
		delete picker;
		picker = nullptr;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// 自定义组合框子类过程
LRESULT CALLBACK ComboBoxSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
	static HBRUSH hBrushBorder; // 边框画刷
	switch (message) {
	case WM_CREATE:
	{
		hBrushBorder = CreateSolidBrush(RGB(127, 127, 127));
		break;
	}
	case WM_NCPAINT: {
		HDC hdc = GetWindowDC(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		OffsetRect(&rect, -rect.left, -rect.top);

		// 设置边框颜色
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushBorder);
		FrameRect(hdc, &rect, hBrushBorder);
		SelectObject(hdc, hOldBrush);
		ReleaseDC(hWnd, hdc);
		return 0;
	}
	case WM_DESTROY:
	{
		DeleteObject(hBrushBorder);
		break;
	}
	default:
		break;
	}
	return DefSubclassProc(hWnd, message, wParam, lParam);
}

class CustomCombox {
	HWND hWnd;      // 父窗口句柄
	HWND Combox;    // 组合框句柄
	HWND hName;     // 名称标签句柄
	HFONT hFont;	 // 字体
	gctype type;    // 当前选择的绘图类型

public:
	CustomCombox(HWND hwnd) {
		this->type = SYSTEM; // 默认类型为 SYSTEM
		this->hWnd = hwnd;

		// 创建名称标签
		hName = CreateWindow(L"STATIC", L"API:", WS_CHILD | WS_VISIBLE,
			5, 5, 100, 20, hWnd, NULL, NULL, NULL);

		// 创建绘图类型选择组合框
		this->Combox = CreateWindow(
			L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_SIMPLE | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
			0, 30, SLIDEWIDTH, 150, hWnd, (HMENU)2, NULL, NULL
		);

		// 创建字体
		hFont = CreateFontW(
			16,                  // 字体高度
			0,                   // 字体宽度（默认）
			0,                   // 倾斜度
			0,                   // 方向
			FW_MEDIUM,          // 粗体
			FALSE,              // 斜体
			FALSE,              // 下划线
			FALSE,              // 删除线
			DEFAULT_CHARSET,    // 字符集
			OUT_DEFAULT_PRECIS, // 输出精度
			CLIP_DEFAULT_PRECIS,// 剪裁精度
			ANTIALIASED_QUALITY,// 渲染质量
			DEFAULT_PITCH | FF_DONTCARE, // 间距和家族
			L"Arial"            // 字体名称
		);

		// 应用字体到静态控件和ComboBox
		SendMessage(Combox, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);
		// 设置组合框高度
		SendMessage(Combox, CB_SETITEMHEIGHT, -1, 25);

		// 子类化组合框窗口过程
		SetWindowSubclass(Combox, ComboBoxSubclassProc, 0, 0);
		// 添加选项
		SendMessage(Combox, CB_ADDSTRING, 0, (LPARAM)L"系统");
		SendMessage(Combox, CB_ADDSTRING, 0, (LPARAM)L"Bresenham");
		SendMessage(Combox, CB_ADDSTRING, 0, (LPARAM)L"中点法");

		// 设置默认选择
		SendMessage(Combox, CB_SETCURSEL, 0, 0); // 默认选择第一个选项
	}

	~CustomCombox() {
		DeleteObject(hFont);
	}

	gctype GetType() {
		return type; // 返回当前类型
	}

	void UpdateType() {
		// 获取当前选择项并更新 type
		int index = SendMessage(Combox, CB_GETCURSEL, 0, 0);
		switch (index) {
		case 0:
			type = SYSTEM;
			break;
		case 1:
			type = CUSTOM1;
			break;
		case 2:
			type = CUSTOM2;
		default:
			break;
		}
	}
};

LRESULT CALLBACK CustomComboxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static CustomCombox* combo = nullptr;
	static HBRUSH hBrushStaticBackground; // 静态控件背景画刷
	static HBRUSH hBrushComboBackground; // 组合框背景画刷

	switch (message) {
	case WM_CREATE: {
		combo = new CustomCombox(hWnd); // 创建 CustomCombox 实例
		hBrushStaticBackground = CreateSolidBrush(SIDEBARCOLOR); // 创建背景画刷
		hBrushComboBackground = CreateSolidBrush(RGB(240, 240, 240)); // 创建背景画刷
		break;
	}
	case WM_MEASUREITEM:
	{
		LPMEASUREITEMSTRUCT pmis = (LPMEASUREITEMSTRUCT)lParam;
		if (pmis->CtlType == ODT_COMBOBOX) {
			pmis->itemHeight = 30;
		}
	}
	break;
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
		if (pdis->CtlType == ODT_COMBOBOX) {
			HDC hdc = pdis->hDC;
			RECT rc = pdis->rcItem;
			HBRUSH hBrush;

			// 绘制背景
			if ((pdis->itemState & ODS_FOCUS) && !(pdis->itemState & ODS_COMBOBOXEDIT)) {
				hBrush = CreateSolidBrush(RGB(215, 215, 220));
			}
			else {
				hBrush = CreateSolidBrush(RGB(245, 245, 250));
			}
			
			FillRect(hdc, &rc, hBrush);
			// 绘制文本
			SetTextColor(hdc, RGB(0, 0, 0)); // 文本颜色
			SetBkMode(hdc, TRANSPARENT); // 设置背景模式为透明，防止覆盖背景色
			wchar_t text[256];
			SendMessage(pdis->hwndItem, CB_GETLBTEXT, pdis->itemID, (LPARAM)text);

			// 设置文本居中
			int textFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			DrawText(hdc, text, -1, &rc, textFormat);
		}
		return TRUE;
	}
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		HWND hStatic = (HWND)lParam;

		// 设置静态控件的文本颜色
		SetTextColor(hdcStatic, RGB(0, 0, 0)); // 黑色文字
		// 设置静态控件的背景模式和颜色
		SetBkMode(hdcStatic, TRANSPARENT);  // 背景透明
		SetBkColor(hdcStatic, SIDEBARCOLOR);  // 设置背景颜色

		return (INT_PTR)hBrushStaticBackground; // 返回背景画刷
	}
	case WM_CTLCOLORLISTBOX:
	{
		HDC hdc = (HDC)wParam;

		SetBkMode(hdc, TRANSPARENT);
		SetBkColor(hdc, SIDEBARCOLOR);

		return (INT_PTR)hBrushComboBackground;
	}
	case WM_COMMAND: {
		if (HIWORD(wParam) == CBN_SELCHANGE) { // 处理选择变化
			combo->UpdateType(); // 更新类型
			SendMessage(GetParent(hWnd), CUSTOM_TYPE_CHANGE, combo->GetType(), 0);
		}
		break;
	}
	case WM_DESTROY: {
		DeleteObject(hBrushComboBackground); // 清理资源
		DeleteObject(hBrushStaticBackground); // 清理资源
		delete combo; // 清理资源
		combo = nullptr;
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
