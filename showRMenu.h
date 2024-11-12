#pragma once

#include "drawinfo.h"

#define SIDM_OPTION1 101
#define SIDM_OPTION2 102
#define SIDM_EXIT    103

typedef void (*MenuItemHandler)(HWND hwnd);

// 显示右键菜单的函数
void ShowContextMenu(HWND hwnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        AppendMenu(hMenu, MF_STRING, SIDM_OPTION1, L"选项 1");
        AppendMenu(hMenu, MF_STRING, SIDM_OPTION2, L"选项 2");
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, SIDM_EXIT, L"退出");

        // 显示弹出菜单
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
}

// 线的右键菜单处理函数
void ShowContextMenuLine(HWND hwnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        MENUITEMINFO itemInfo;
        ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
        itemInfo.cbSize = sizeof(MENUITEMINFO);
        itemInfo.fMask = MIIM_ID | MIIM_DATA;

        AppendMenu(hMenu, MF_STRING, SIDM_OPTION1, L"选项 1");
        AppendMenu(hMenu, MF_STRING, SIDM_OPTION2, L"选项 2");
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, IDM_EXIT, L"退出");

        // 显示弹出菜单
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
}
