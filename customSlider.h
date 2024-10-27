#pragma once

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#pragma comment(lib, "Comctl32.lib")

LRESULT CALLBACK TrackbarSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);

class CustomSlider {
public:
    HWND hWnd; // 自定义滑块窗口句柄
    HWND hName; // 名称标签
    HWND hEdit; // 输入框
    HWND hTrackbar; // 滑块控件
    int value; // 当前滑块值

    CustomSlider(HWND parent, LPCWSTR name, int x, int y, int width) {
        // 创建滑块窗口
        hWnd = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
            x, y, width, 60, parent, NULL, NULL, NULL);

        // 创建名称标签
        hName = CreateWindow(L"STATIC", name, WS_CHILD | WS_VISIBLE,
            x+5, y+5, 50, 20, hWnd, NULL, NULL, NULL);

        // 创建输入框
        hEdit = CreateWindow(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | ES_READONLY | ES_CENTER,
            x + width - 50, y+5, 50, 20, hWnd, NULL, NULL, NULL);

        // 创建滑块控件
        hTrackbar = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_TOOLTIPS,
            x, y + 35, width, 20, hWnd, NULL, NULL, NULL);

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

    void UpdateValue(int newValue) {
        value = newValue;
        WCHAR buffer[10];
        swprintf_s(buffer, 10, L"%d", value);
        SetWindowText(hEdit, buffer);
        InvalidateRect(hWnd, NULL, TRUE); // 触发重绘
    }
};

// 子类化过程函数，拦截 WM_SETFOCUS 消息
LRESULT CALLBACK TrackbarSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
	if (message == WM_SETFOCUS) {
		// 将焦点设置到父窗口，避免滑块显示虚线框
		SetFocus(GetParent(hWnd));
		return 0;
	}
	return DefSubclassProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK SliderWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static CustomSlider* slider = nullptr; // 保存自定义滑块指针

	switch (message) {
	case WM_CREATE: {
		// 初始化自定义滑块
		slider = new CustomSlider(hWnd, L"宽度", 0, 0, 150);
		break;
	}
	case WM_HSCROLL: {
		if ((HWND)lParam == slider->hTrackbar) {
			int pos = SendMessage(slider->hTrackbar, TBM_GETPOS, 0, 0);
			slider->UpdateValue(pos); // 更新值并重绘
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 绘制滑块本体
		RECT rect;
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		// 绘制灰色线条
		HBRUSH hBrushGray = CreateSolidBrush(RGB(200, 200, 200));
		FillRect(hdc, &rect, hBrushGray);

		// 获取滑块当前位置
		int pos = SendMessage(slider->hTrackbar, TBM_GETPOS, 0, 0);
		int lineLength = (pos - 1) * (width - 20) / 99; // 计算颜色变化的长度

		// 绘制浅蓝色已通过的线条
		HBRUSH hBrushLightBlue = CreateSolidBrush(RGB(173, 216, 230)); // 浅蓝色
		RECT lineRect = { rect.left, height - 40, rect.left + lineLength, height - 30 };
		FillRect(hdc, &lineRect, hBrushLightBlue);

		// 绘制蓝色指针
		RECT pointerRect = { rect.left + lineLength - 5, height - 30, rect.left + lineLength + 5, height - 10 };
		HBRUSH hBrushBlue = CreateSolidBrush(RGB(0, 0, 255)); // 蓝色
		FillRect(hdc, &pointerRect, hBrushBlue);

		// 清理资源
		DeleteObject(hBrushGray);
		DeleteObject(hBrushLightBlue);
		DeleteObject(hBrushBlue);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		delete slider; // 清理内存
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}