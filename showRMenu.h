#pragma once

#include "drawinfo.h"
#include "windowState.h"
#include <vector>

#define POPMENUINTERVAL     5
#define IDBASE      100

#define ARGS_NONE        0
typedef void (*MenuItemHandlerN)();

#define ARGS_HWND        1
typedef void (*MenuItemHandlerH)(HWND hwnd);

#define ARGS_CSDRAW	  2
typedef void (*MenuItemHandlerC)(CSDrawInfo* csdraw);

typedef void* MenuItemHandler;

typedef enum HandlerType {
	HANDLER_NONE = ARGS_NONE,
    HANDLER_HWND = ARGS_HWND,
	HANDLER_CSDRAW = ARGS_CSDRAW,
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

typedef struct RightMenuStyle {
    int xinterval;
} RightMenuStyle;

void InitMenuStyle(RightMenuStyle& style, int xinterval) {
	style.xinterval = xinterval;
}

typedef struct RightMenuManager {
    int id;
    std::vector<MenuItemData> rightMenuData;
    RigthMenuType rightMenuType;
    RightMenuStyle style;
    HMENU rightPopMenu;
	HWND hwnd;
	RightMenuManager() : id(IDBASE), rightMenuType(RigthMenuInit), style({0}), rightPopMenu(NULL), hwnd(NULL) {}
} RightMenuManager;

void MENUHANDLERNONE() {
    return;
}

MenuItemData MENUITEMDATANULL = { (MenuItemHandler)MENUHANDLERNONE, HANDLER_NONE };

RightMenuManager rmenuManager;

void SetCSDrawZoom(CSDrawInfo& csdraw) {
	SetCSDrawMode(csdraw, ZOOM);
}

void SetCSDrawRotate(CSDrawInfo& csdraw) {
	SetCSDrawMode(csdraw, ROTATE);
}

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
    manager.id = IDBASE;
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

void AddMenuItem(RightMenuManager& manager, const wchar_t* text, MenuItemData data) {
    MENUITEMINFO itemInfo;
    ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    itemInfo.fMask = MIIM_DATA;
    size_t index = manager.rightMenuData.size();
    itemInfo.dwItemData = (ULONG_PTR)index;

    manager.rightMenuData.push_back(data);
    AppendMenu(manager.rightPopMenu, MF_STRING, manager.id, text);
    int s = SetMenuItemInfo(manager.rightPopMenu, manager.id++, FALSE, &itemInfo);
}

void InitBaseMenu(RightMenuManager& manager) {
	MenuItemData data;
	data.handler = (MenuItemHandler)SetCSDrawZoom;
	data.type = HANDLER_CSDRAW;

	AddMenuItem(manager, L"缩放", data);

	data.handler = (MenuItemHandler)SetCSDrawRotate;
	AddMenuItem(manager, L"旋转", data);

    AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuNone(RightMenuManager& manager) {
	InitMenuStyle(manager.style, 0);

    MenuItemData data;
    data.handler = (MenuItemHandler)&MenuTest;
    data.type = HANDLER_HWND;

    AddMenuItem(manager, L"测试", data);
}

// 进入垂线的绘制
void MenuLineCX(HWND hwnd) {

}

void InitRightMenuLine(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

    InitBaseMenu(manager);

    MenuItemData data;
	data.handler = (MenuItemHandler)&MenuLineCX;
	data.type = HANDLER_HWND;
    AddMenuItem(manager, L"作垂线", data);

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

void CustomTrackPopupMenu(RightMenuManager& manager, POINT pt) {
	TrackPopupMenu(manager.rightPopMenu, TPM_RIGHTBUTTON, pt.x + manager.style.xinterval, pt.y, 0, manager.hwnd, NULL);
}

void ShowMenu(RightMenuManager& manager, POINT pt, RigthMenuType type) {
    ClientToScreen(manager.hwnd, &pt);
	if (manager.rightMenuType != type) {
        InitRightMenu(manager, type);
	}
	CustomTrackPopupMenu(manager, pt);
}
