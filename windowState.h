#ifndef WINDOWSIZE_H
#define WINDOWSIZE_H

#include <Windows.h>
#include <stack>

#define HELPLINECORLOR	RGB(0, 0 , 255)
#define HELPPOINTCOLOR	RGB(255, 0, 0)

// 画布宽度和其他宽度
#define OTHERW 200
#define HUABUWIDTH(width) (width - OTHERW)
#define OTHERWIDTH(width) OTHERW
#define STATUSBARHEIGHT 20
#define DEFAULTPADDING 6

#define DEFAULTRADIUS		0.04		// 默认缩放
#define MINRADIUS			0.0001		// 最小缩放
#define MAXRADIUS			10000		// 最大缩放
#define RADIUSCHANGESPEED	0.001		// 缩放增速
#define FITRADIUS			1.5			// 适应屏幕时的倍率
#define MINXPERZ			30			// 最小像素每刻度
#define STEPSHOWNUM			4			// 多少个刻度下显示数据

#define ILLEGELPOINT		-1
#define INITPOINT			{ ILLEGELPOINT, ILLEGELPOINT}

#define CanvasMode			1		// 画布模式
#define CoordinateMode		2		// 坐标模式
#define DEFAULTSYSTEMMODE	{ CoordinateMode }

typedef struct SYSTEMMODE {
	int worktype;
} SYSTEMMODE;

bool HFPoint(const POINT* pt) {
	return pt->x != ILLEGELPOINT && pt->y != ILLEGELPOINT;
}

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
	DRAWBCURVE,
	DRAWMULTILINE,
	DRAWFMULTI,
	MMOUSEMOVE,
	KZDRAW,				// 扩展功能
} DrawType;

typedef enum KZDrawType {
	DRAWCX,				// 画垂线
} KZDrawType;

typedef struct MyDrawState MyDrawState;

typedef void typeGo(MyDrawState& ms, DrawType type);
typedef void typeBack(MyDrawState& ms);

struct MyDrawState {
	bool draw;
	DrawType type;
	DrawType lastType;
	std::stack<DrawType> preType;
	typeGo* go;
	typeBack* back;
	KZDrawType kztype;
	POINT lastMouseP;
	POINT lastMMouseBDown;
	POINT lastMMouseBUp;
	POINT lastLButtonDown;
	POINT lastLButtonUp;
};

void LButtonDown(MyDrawState &mst, POINT point) {
	mst.lastLButtonDown = point;
}

void LButtonUp(MyDrawState &mst, POINT point) {
	mst.lastLButtonUp = point;
}

void MMouseDown(MyDrawState &mst, POINT point) {
	mst.lastMMouseBDown = point;
}

void MMouseUp(MyDrawState &mst, POINT point) {
	mst.lastMMouseBUp;
}

// TODO:
void MouseMove(MyDrawState &mst, POINT point) {
	mst.lastMouseP = point;
}

bool TwoPointDraw(const POINT& p1, const POINT& p2) {
	return (p1.x != p2.x || p1.y != p2.y) && (p1.x != ILLEGELPOINT && p1.y != ILLEGELPOINT && p2.x != ILLEGELPOINT && p2.y != ILLEGELPOINT);
}

bool InDraw(const MyDrawState& mst) {
	return mst.type != CHOOSEIMG && mst.type != CHOOSEN && mst.type != MMOUSEMOVE;
}

void setTypeWithLastType(MyDrawState& mst, DrawType type) {
	mst.lastType = mst.type;
	mst.type = type;
	mst.preType.push(type);
}

DrawType getType(MyDrawState& mst) {
	return mst.type;
}

void ClearStateP(MyDrawState& mst) {
	mst.lastLButtonDown = INITPOINT;
	mst.lastLButtonUp = INITPOINT;
	mst.lastMMouseBDown = INITPOINT;
	mst.lastMMouseBUp = INITPOINT;
	mst.lastMouseP = INITPOINT;
}

void RestoreFormLastType(MyDrawState& mst) {
	if (mst.type == MMOUSEMOVE) {
		ClearStateP(mst);
		mst.type = mst.lastType;
		mst.preType.pop();
	}
}

void InitMyDrawState(MyDrawState& mst) {
	mst.type = CHOOSEIMG;
	mst.lastType = CHOOSEIMG;
	ClearStateP(mst);
	mst.go = setTypeWithLastType;   // 初始化函数指针
	mst.back = RestoreFormLastType; // 初始化函数指针
}

void setType(MyDrawState& mst, DrawType type) {
	mst.type = type;
	switch (type) {
	case CHOOSEIMG:
	case DRAWLINE:
	case DRAWCIRCLE:
	case DRAWRECTANGLE:
	case DRAWMULTILINE:
	case DRAWFMULTI:
	case DRAWCURVE:
	case DRAWBCURVE:
		ClearStateP(mst);
		break;
	default:
		break;
	}
}

void setKZType(MyDrawState& mst, KZDrawType type) {
	mst.type = KZDRAW;
	mst.kztype = type;
	ClearStateP(mst);
}

void EndKZType(MyDrawState& mst) {
	mst.type = CHOOSEIMG;
}

void InitWindowRect(WindowRect& wr, const RECT& mainrect, const RECT& toolbarrect, const int mode) {
	int padding;
	switch (mode) {
	case CanvasMode:
	{
		padding = 60;
	}
	break;
	case CoordinateMode:
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
	case CanvasMode:
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
	case CoordinateMode:
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
void PointToCoordinate(const Coordinate& coor, POINT& pt, double& x, double& y) {
	x = (pt.x - coor.center.x) * coor.radius;
	y = (coor.center.y - pt.y) * coor.radius;
}

typedef struct ChooseState {
	int choose;	// id
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

void SetActiveID(ChooseState &cs, int id) {
	cs.choose = id;
}

// 更改 drawing 的定义添加其他数据使重绘时更完善
typedef struct DrawingInfo {
	DrawInfo info;
	MyPoint lastRem;	// 鼠标移除时的坐标
} DrawingInfo;

void InitDrawing(DrawingInfo *di) {
	di->info.type = NONE;
	di->lastRem = { ILLEGELMYPOINT, ILLEGELMYPOINT };
}

void ClearDrawing(DrawingInfo* di) {
	di->lastRem = INITPOINT;
	switch (di->info.type) {
	case LINE:
		di->info.line.start = INITMYPOINT;
		break;
	case CIRCLE:
		di->info.circle.center = INITMYPOINT;
		break;
	case RECTANGLE:
		di->info.rectangle.start = INITMYPOINT;
		break;
	case CURVE:
	case BCURVE:
	case MULTILINE:
	case FMULTILINE:
		// 清理空间
		ClearMultipoint(&(di->info.multipoint));
		break;
	}
}

void InitDrawInfo(DrawingInfo* di, DrawInfo *info) {
	if (&(di->info) == info) return;
	info->type = di->info.type;
	info->proper = di->info.proper;
	switch (di->info.type) {
	case LINE:
	{
		info->line = di->info.line;
	}
	break;
	case CIRCLE:
	{
		info->circle = di->info.circle;
	}
	break;
	case RECTANGLE:
	{
		info->rectangle = di->info.rectangle;
	}
	break;
	case CURVE:
	case BCURVE:
	case MULTILINE:
	case FMULTILINE:
	{
		InitFromMultipoint(&(info->multipoint), &(di->info.multipoint));
	}
	break;
	default:
		break;
	}
}

typedef struct CSDrawInfo {
	int index;
	DrawInfo choose;
} CSDrawInfo;

void InitCSDrawInfo(CSDrawInfo& csdraw) {
	csdraw.index = -1;
}

void PopStoreImgToCSDraw(StoreImg& imgs, CSDrawInfo& csdraw) {
	CopyDrawInfoFromImg(&imgs, &(csdraw.choose), csdraw.index);
	RemoveDrawInfoFromStoreImg(&imgs, csdraw.index);
}

void RestoreCSDraw(StoreImg& imgs, CSDrawInfo& csdraw) {
	SetDrawInfoToStoreImg(&imgs, &(csdraw.choose), csdraw.index);
}

// TODO: 工作区?
// 静态数据
SYSTEMMODE systemode = DEFAULTSYSTEMMODE;
WindowState wstate = { 800, 650, 45 };
MyDrawState mst = { CHOOSEIMG, CHOOSEIMG };		// 默认状态
Coordinate coordinate;							// 坐标系
StoreImg allImg;								// 存储所有的图形
DrawingInfo drawing;							// 当前正在绘制的图形
DrawUnitProperty customProperty;				// 自定义绘图
WindowRect wrect;								// 各个组件的位置
ChooseState cs;									// 工具栏状态维护
CSDrawInfo csdraw;								// 被选中的图元
#endif // WINDOWSIZE_H
