#pragma once

#include "drawinfo.h"
#include "windowState.h"
#include <vector>

#define MENU_LINE_CX 101
#define SIDM_OPTION2 102
#define SIDM_EXIT    103

#define ARGS_NONE        0
typedef void (*MenuItemHandlerN)();

#define ARGS_HWND        1
typedef void (*MenuItemHandlerH)(HWND hwnd);

typedef void* MenuItemHandler;

typedef enum HandlerType {
	HANDLER_NONE = ARGS_NONE,
    HANDLER_HWND = ARGS_HWND,

} HandlerType;

typedef struct MenuItemData {
    MenuItemHandler handler;
    HandlerType type;
} MenuItemData, *MenuItemDataPTR;

typedef enum RigthMenuType {
    RigthMenuInit,
    RightMenuNone,
    RigthMenuLine,

} RigthMenuType;

typedef struct RightMenuManager {
    std::vector<MenuItemData> rightMenuData;
    RigthMenuType rightMenuType;
    HMENU rightPopMenu;
	HWND hwnd;
	RightMenuManager() : rightMenuType(RigthMenuInit), rightPopMenu(NULL), hwnd(NULL) {}
} RightMenuManager;

void MENUHANDLERNONE() {
    return;
}

MenuItemData MENUITEMDATANULL = { (MenuItemHandler)MENUHANDLERNONE, HANDLER_NONE };

RightMenuManager rmenuManager;

void MenuTest(HWND hwnd) {
	MessageBox(hwnd, L"测试", L"测试", MB_OK);
}

void InitRightMenuM(RightMenuManager &manager, HWND hwnd) {
    manager.rightPopMenu = CreatePopupMenu();
	manager.rightMenuType = RigthMenuInit;
    manager.hwnd = hwnd;
}

void DestroyRightMenuM(RightMenuManager& manager) {
    DestroyMenu(manager.rightPopMenu);
    manager.rightPopMenu = NULL;
    manager.rightMenuData.clear();
}

void ClearRightMenu(RightMenuManager& manager) {
    // 如果右键菜单已经存在，先清空菜单内容
    if (manager.rightPopMenu) {
        // 删除现有菜单项
        DestroyMenu(manager.rightPopMenu);
        manager.rightPopMenu = CreatePopupMenu();

        manager.rightMenuData.clear();
    }
    else {
        // 如果菜单句柄为NULL，创建新的菜单
        manager.rightPopMenu = CreatePopupMenu();
    }
}

void AddMenuItem(RightMenuManager& manager, int id, const wchar_t* text, MenuItemData data) {
    MENUITEMINFO itemInfo;
    ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    itemInfo.fMask = MIIM_DATA;
    size_t index = manager.rightMenuData.size();
    itemInfo.dwItemData = (ULONG_PTR)index;

    manager.rightMenuData.push_back(data);
    AppendMenu(manager.rightPopMenu, MF_STRING, id, text);
    int s = SetMenuItemInfo(manager.rightPopMenu, id, FALSE, &itemInfo);
}

void InitRightMenuNone(RightMenuManager& manager) {
    MenuItemData data;
    data.handler = (MenuItemHandler)&MenuTest;
    data.type = HANDLER_HWND;

    AddMenuItem(manager, SIDM_OPTION2, L"测试", data);
}

// 进入垂线的绘制
void MenuLineCX(HWND hwnd) {

}

void InitRightMenuLine(RightMenuManager& manager) {
    MenuItemData data;
	data.handler = (MenuItemHandler)&MenuLineCX;
	data.type = HANDLER_HWND;
    AddMenuItem(manager, MENU_LINE_CX, L"作垂线", data);

    AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

MenuItemData GetMenuItemData(RightMenuManager& manager, UINT item) {
    MENUITEMINFO itemInfo;
    ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    itemInfo.fMask = MIIM_DATA;
    if (GetMenuItemInfo(manager.rightPopMenu, LOWORD(item), FALSE, &itemInfo)) {
		int index = (int)itemInfo.dwItemData;
        return manager.rightMenuData[index];
    }

	return MENUITEMDATANULL;
}

void InitRightMenu(RightMenuManager& manager, RigthMenuType type) {
    ClearRightMenu(manager);
    manager.rightMenuType = type;
    switch (type) {
    case RightMenuNone:
    {
        InitRightMenuNone(manager);
        break;
    }
	case RigthMenuLine:
		InitRightMenuLine(manager);
		break;
    }
}

void ShowMenu(RightMenuManager& manager, POINT pt, RigthMenuType type) {
    ClientToScreen(manager.hwnd, &pt);
	if (manager.rightMenuType != type) {
        InitRightMenu(manager, type);
	}
    TrackPopupMenu(manager.rightPopMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, manager.hwnd, NULL);
}
