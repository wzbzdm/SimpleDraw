#pragma once

#include "drawinfo.h"

#define IDM_OPTION1 101
#define IDM_OPTION2 102
#define IDM_EXIT    103

// 显示右键菜单的函数
void ShowContextMenu(HWND hwnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        AppendMenu(hMenu, MF_STRING, IDM_OPTION1, L"选项 1");
        AppendMenu(hMenu, MF_STRING, IDM_OPTION2, L"选项 2");
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, IDM_EXIT, L"退出");

        // 显示弹出菜单
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
}
