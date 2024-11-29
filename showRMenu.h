#pragma once

#include "drawinfo.h"
#include "windowState.h"
#include <math.h>
#include <vector>

#define POPMENUINTERVAL     5
#define IDBASE      100

#define ARGS_NONE           0
typedef void (*MenuItemHandlerN)();

#define ARGS_HWND           1
typedef void (*MenuItemHandlerH)(HWND hwnd);

#define ARGS_CSDRAW	        2
typedef void (*MenuItemHandlerC)(CSDrawInfo* csdraw);

#define ARGS_CUT            3
typedef void (*MenuItemHandlerCut)(MyDrawState& mst, CSDrawInfo& csdraw);

#define ARGS_MST            4
typedef void (*MenuItemHandlerMST)(MyDrawState& mst);

#define ARGS_KZDRAW         5
typedef void (*MenuItemHandlerKZD)(MyDrawState& mst, CSDrawInfo& csdraw, KZDrawInfo& kzdraw);

#define ARGS_CJSF           10
typedef void (*MenuItemHandlerCJSF)(StoreImg& imgs, const CSDrawInfoRect& rect, Coordinate& coor);

typedef void* MenuItemHandler;

typedef enum HandlerType {
	HANDLER_NONE    =   ARGS_NONE,
    HANDLER_HWND    =   ARGS_HWND,
	HANDLER_CSDRAW  =   ARGS_CSDRAW,
	HANDLER_CJSF    =   ARGS_CJSF,
    HANDLER_CUT     =   ARGS_CUT,
    HANDLER_MST     =   ARGS_MST,
    HANDLER_KZD     =   ARGS_KZDRAW,
} HandlerType;

typedef struct MenuItemData {
    MenuItemHandler handler;
    HandlerType type;
} MenuItemData, *MenuItemDataPTR;

typedef enum RigthMenuType {
    RigthMenuInit,
    RightMenuNone,
    RightMenuChoose,
    RigthMenuLine,
    RightMenuCircle,
	RightMenuRectangle,
	RightMenuCurve,
	RightMenuBCurve,
	RightMenuMultiLine,
	RightMenuFMultiLine,
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

void InitRightMenuChoose(RightMenuManager& manager) {
    InitMenuStyle(manager.style, 0);

    MenuItemData data;
}

// 进入垂线的绘制
void MenuLineCX(MyDrawState& mst, CSDrawInfo& csdraw, KZDrawInfo &kzdraw) {
    if (csdraw.choose.type != LINE) return;
    setTypeWithLastType(mst, KZDRAW);
    setKZType(kzdraw, DRAWCX);
    MyPoint start = csdraw.choose.line.start;
    MyPoint end = csdraw.choose.line.end;
    // 求角度
    kzdraw.cx.first = true;
    // 不显示辅助线
    ChangeShowLineState(csdraw.config, false);
}

// 线的裁减算法
void MenuLineCut(MyDrawState& mst, CSDrawInfo& csdraw) {
    // 进入 CUTIMG 模式
    setTypeWithLastType(mst, CUTIMG);
    // 进入 CUT 模式
    EnterCutMode(csdraw.config, CUTFUNC, 1);
}

void InitRightMenuLine(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

    InitBaseMenu(manager);

    MenuItemData data;
	data.handler = (MenuItemHandler)&MenuLineCX;
	data.type = HANDLER_KZD;
    AddMenuItem(manager, L"作垂线", data);

    data.handler = (MenuItemHandler)&MenuLineCut;
    data.type = HANDLER_CUT;
    AddMenuItem(manager, L"裁剪", data);

    AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuCircle(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

	AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuRectangle(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

	AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuCurve(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

	AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuBCurve(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

	AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

void InitRightMenuMultiLine(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

	AppendMenu(manager.rightPopMenu, MF_SEPARATOR, 0, NULL);
}

// 多边形的裁减算法
void MenuFMultiCutSH(MyDrawState& mst, CSDrawInfo& csdraw) {
    // 进入 CUTIMG 模式
    setTypeWithLastType(mst, CUTIMG);
    // 设置 CUT 函数
    EnterCutMode(csdraw.config, CUTFUNC, 1);
}

// 多边形的裁减算法
void MenuFMultiCutWA(MyDrawState& mst, CSDrawInfo& csdraw) {
    // 进入 CUTIMG 模式
    setTypeWithLastType(mst, CUTIMG);
    // 设置 CUT 函数
    EnterCutMode(csdraw.config, CUTFUNC, 2);
}

void InitRightMenuFMultiLine(RightMenuManager& manager) {
	InitMenuStyle(manager.style, POPMENUINTERVAL);

	InitBaseMenu(manager);

    MenuItemData data;
    data.handler = (MenuItemHandler)&MenuFMultiCutSH;
    data.type = HANDLER_CUT;
    AddMenuItem(manager, L"Sutherland-Hodgman 裁减", data);

    data.handler = (MenuItemHandler)&MenuFMultiCutWA;
    data.type = HANDLER_CUT;
    AddMenuItem(manager, L"Weiler-AthertonWeiler 裁剪", data);

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
    case RightMenuChoose:
    {
        InitRightMenuChoose(manager);
        break;
    }
    case RigthMenuLine:
        InitRightMenuLine(manager);
        break;
    case RightMenuCircle:
    {
        InitRightMenuCircle(manager);
        break;
    }
    case RightMenuRectangle:
    {
        InitRightMenuRectangle(manager);
        break;
    }
    case RightMenuCurve:
    {
        InitRightMenuCurve(manager);
        break;
    }
    case RightMenuBCurve:
    {
        InitRightMenuBCurve(manager);
        break;
    }
    case RightMenuMultiLine:
    {
        InitRightMenuMultiLine(manager);
        break;
    }
    case RightMenuFMultiLine:
    {
        InitRightMenuFMultiLine(manager);
        break;
    }
    default:
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

void ShowMenuType(RightMenuManager& manager, POINT pt, ImgType type) {
    switch (type) {
    case LINE:
    {
        ShowMenu(rmenuManager, pt, RigthMenuLine);
        break;
    }
    case CIRCLE:
    {
		ShowMenu(rmenuManager, pt, RightMenuCircle);
        break;
    }
    case RECTANGLE:
    {
        ShowMenu(rmenuManager, pt, RightMenuRectangle);
        break;
    }
    case CURVE:
    {
        ShowMenu(rmenuManager, pt, RightMenuCurve);
        break;
    }
    case BCURVE:
    {
        ShowMenu(rmenuManager, pt, RightMenuBCurve);
        break;
    }
    case MULTILINE:
    {
        ShowMenu(rmenuManager, pt, RightMenuMultiLine);
        break;
    }
	case FMULTILINE:
	{
        ShowMenu(rmenuManager, pt, RightMenuFMultiLine);
		break;
	}
    default:
        break;
    }
    
}