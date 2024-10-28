#ifndef WINDOWSIZE_H
#define WINDOWSIZE_H

#include <Windows.h>

// 画布宽度和其他宽度
#define OTHERW 200
#define HUABUWIDTH(width) (width - OTHERW)
#define OTHERWIDTH(width) OTHERW
#define STATUSBARHEIGHT 20
#define DEFAULTPADDING 6

#define DEFAULTRADIUS 0.04
#define MINRADIUS 0.0001
#define MAXRADIUS 10000
#define RADIUSCHANGESPEED 0.001
#define FITRADIUS 1.5
#define MINXPERZ 30

typedef struct windowState {
	int width;
	int height;
	int toolbarHeight;
} WindowState;

typedef struct windowPos {
	int x;
	int y;
	int width;
	int height;
} WindowsPos;

typedef struct windowRect {
	RECT mainrect;
	RECT toolbarrect;
	WindowsPos smallrect;
	WindowsPos canvasrect;
	WindowsPos sidebarrect;
	WindowsPos statusbar;
} WindowRect;

typedef enum DrawType {
	CHOOSEIMG,
	CHOOSEN,
	DRAWLINE,
	DRAWCIRCLE,
	DRAWRECTANGLE,
	DRAWCURVE,
	DRAWMULTILINE,
	MMOUSEMOVE,
	KZDRAW,				// 扩展功能
} DrawType;

typedef enum KZDrawType {
	DRAWCX,				// 画垂线
} KZDrawType;

typedef struct MyDrawState {
	DrawType type;
	DrawType lastType;
	KZDrawType kztype;
	union {
		POINT lastMMousemovePoint;
		POINT lastLButtonPoint;
	};
} MyDrawState;

bool InDraw(MyDrawState& mst) {
	return mst.type != CHOOSEIMG && mst.type != CHOOSEN && mst.type != MMOUSEMOVE;
}

bool CanRefresh(MyDrawState& mst) {
	switch (mst.type) {
	case DRAWLINE:
	case DRAWCIRCLE:
	case DRAWRECTANGLE:
	case DRAWCURVE:
	case DRAWMULTILINE:
	{
		if (mst.lastLButtonPoint.x == -1 && mst.lastLButtonPoint.y == -1) {
			return false;
		} 
		return true;
	}
	case CHOOSEN:
	case CHOOSEIMG:
	case MMOUSEMOVE:
	{
		return true;
	}
	case KZDRAW:
		switch (mst.kztype) {
		case DRAWCX:
			return true;
		}
	default:
		return true;
	}
	
}

void InitMyDrawState(MyDrawState& mst) {
	mst.type = CHOOSEIMG;
	mst.lastType = CHOOSEIMG;
	mst.lastLButtonPoint = { -1, -1 };
	mst.lastMMousemovePoint = { -1, -1 };
}

void setTypeWithLastType(MyDrawState& mst, DrawType type){
	mst.lastType = mst.type;
	mst.type = type;
}

void RestoreFormLastType(MyDrawState& mst) {
	mst.lastLButtonPoint = { -1, -1 };
	mst.type = mst.lastType;
}

DrawType getType(MyDrawState& mst) {
	return mst.type;
}

void setType(MyDrawState& mst, DrawType type) {
	mst.type = type;
	switch (type) {
	case CHOOSEIMG:
	case DRAWLINE:
	case DRAWCIRCLE:
	case DRAWRECTANGLE:
	case DRAWMULTILINE:
	case DRAWCURVE:
		mst.lastLButtonPoint = { -1, -1 };
		break;
	default:
		break;
	}
}

void setKZType(MyDrawState& mst, KZDrawType type) {
	mst.type = KZDRAW;
	mst.kztype = type;
	mst.lastLButtonPoint = { -1, -1 };
}

void EndKZType(MyDrawState& mst) {
	mst.type = CHOOSEIMG;
}

// 静态数据
WindowState wstate = { 800, 650, 45 };

void InitWindowRect(WindowRect& wr, const RECT& mainrect, const RECT& toolbarrect, const int mode) {
	int padding;
	switch (mode) {
	case 1:
	{
		padding = 60;
		
	}
	break;
	case 2:
	{
		padding = 0;
	}
	break;
	default:
		padding = 60;
		break;
	}
	wr.mainrect = mainrect;
	wr.toolbarrect = toolbarrect;
	wr.smallrect = { 0, toolbarrect.bottom - toolbarrect.top , HUABUWIDTH(mainrect.right - mainrect.left) , mainrect.bottom - toolbarrect.bottom - STATUSBARHEIGHT };
	wr.canvasrect = { padding, padding, wr.smallrect.width - 2 * padding, wr.smallrect.height - 2 * padding - DEFAULTPADDING };
	wr.statusbar = { 0, wr.smallrect.y + wr.smallrect.height, wr.mainrect.right - wr.mainrect.left, STATUSBARHEIGHT };
	wr.sidebarrect = { wr.smallrect.x + wr.smallrect.width, wr.smallrect.y, OTHERWIDTH(mainrect.right - mainrect.left), wr.smallrect.height };
}

void RefreshWindowRect(WindowRect& wr, const int width, const int height, const int mode) {
	switch (mode) {
	case 1:
	{
		// 工具栏保持高度不变，宽度适应主窗口
		wr.mainrect = { 0, 0, width, height };
		int lastBottom = wr.toolbarrect.bottom;
		wr.toolbarrect = { 0, 0, width, lastBottom };

		// 小窗口宽度为主窗口宽度的 2/3, 高度为主窗口高度减去工具栏的高度
		int smallWidth = HUABUWIDTH(width);
		int smallHeight = wr.mainrect.bottom - wr.toolbarrect.bottom - STATUSBARHEIGHT;

		// 更新小窗口的位置和大小
		wr.smallrect = { 0, wr.toolbarrect.bottom, smallWidth, smallHeight };

		// 侧边栏区域
		wr.sidebarrect = { wr.smallrect.x + wr.smallrect.width, wr.smallrect.y, OTHERWIDTH(width), wr.smallrect.height };

		// 画布保持原来的宽度和高度，居中小窗口
		int canvasWidth = wr.canvasrect.width;
		int canvasHeight = wr.canvasrect.height;

		// 小窗口内居中画布
		int canvasX = (smallWidth - canvasWidth) / 2;
		int canvasY = (smallHeight - canvasHeight) / 2;

		// 更新画布的矩形（画布相对于小窗口的位置）
		wr.canvasrect = { canvasX, canvasY, canvasWidth, canvasHeight };

		// 状态栏保持原来的宽度和高度，位置在小窗口的下方
		wr.statusbar = { 0, wr.smallrect.y + wr.smallrect.height, width, STATUSBARHEIGHT };
	}
	break;
	case 2:
	{
		// 工具栏保持高度不变，宽度适应主窗口
		wr.mainrect = { 0, 0, width, height };
		int lastBottom = wr.toolbarrect.bottom;
		wr.toolbarrect = { 0, 0, width, lastBottom };

		// 小窗口宽度为主窗口宽度的 2/3, 高度为主窗口高度减去工具栏的高度
		int smallWidth = HUABUWIDTH(width);
		int smallHeight = wr.mainrect.bottom - wr.toolbarrect.bottom - STATUSBARHEIGHT;

		// 更新小窗口的位置和大小
		wr.smallrect = { 0, wr.toolbarrect.bottom, smallWidth, smallHeight };

		// 侧边栏区域
		wr.sidebarrect = { wr.smallrect.x + wr.smallrect.width, wr.smallrect.y, OTHERWIDTH(width), wr.smallrect.height };

		// 画布与小窗口大小相同
		wr.canvasrect = {0, 0, smallWidth, smallHeight - DEFAULTPADDING };

		// 状态栏保持原来的宽度和高度，位置在小窗口的下方
		wr.statusbar = { 0, wr.smallrect.y + wr.smallrect.height, width, STATUSBARHEIGHT };
	}
	break;
	}
}

RECT getSmallRect(const WindowRect& wr)
{
	RECT smallRect;
	smallRect.left = wr.smallrect.x;  // 矩形左边界
	smallRect.top = wr.smallrect.y;   // 矩形上边界
	smallRect.right = wr.smallrect.x + wr.smallrect.width;   // 矩形右边界
	smallRect.bottom = wr.smallrect.y + wr.smallrect.height; // 矩形下边界
	return smallRect;
}

RECT getCanvasRect(const WindowRect& wr)
{
	RECT canvasRect;
	canvasRect.left = wr.canvasrect.x;  // 矩形左边界
	canvasRect.top = wr.canvasrect.y;   // 矩形上边界
	canvasRect.right = wr.canvasrect.width + wr.canvasrect.x;   // 矩形右边界
	canvasRect.bottom = wr.canvasrect.height + wr.canvasrect.y; // 矩形下边界
	return canvasRect;
}

typedef struct Coordinate {
	// 坐标系中心在画布上的位置
	POINT center;
	// 像素与坐标的比例
	// 单位/像素
	double radius;
} Coordinate;

void SetCoordinate(Coordinate& coor, POINT center, const double radius) {
	coor.center = center;
	coor.radius = radius;
}

// 将坐标映射到画布上
POINT mapCoordinate(Coordinate& coor, double x, double y) {
	POINT pt;
	pt.x = (LONG)(coor.center.x + x / coor.radius);
	pt.y = (LONG)(coor.center.y - y / coor.radius);
	return pt;
}

// 将画布上的点映射到坐标系上
void PointToCoordinate(Coordinate& coor, POINT& pt, double& x, double& y) {
	x = (pt.x - coor.center.x) * coor.radius;
	y = (coor.center.y - pt.y) * coor.radius;
}

typedef struct ChooseState {
	int choose;
	int count;
	int* ids;
} ChooseState;

void InitState(ChooseState& cs, int count, int dc) {
	cs.choose = dc;
	cs.count = 0;
	cs.ids = (int*)malloc(sizeof(int) * count);
	assert(cs.ids != NULL);
	for (int i = 0; i < count; i++) {
		cs.ids[i] = 0;
	}
}

void AddIdToState(ChooseState& cs, int id) {
	cs.ids[cs.count++] = id;
}

#endif // WINDOWSIZE_H
