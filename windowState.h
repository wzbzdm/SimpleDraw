#ifndef WINDOWSIZE_H
#define WINDOWSIZE_H

#include <Windows.h>
#include <stack>
#include <vector>

using namespace std;

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
#define ANGLECHANGESPEED	0.0007		// 旋转速度
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

bool HFPoint(const POINT& pt) {
	return pt.x != ILLEGELPOINT && pt.y != ILLEGELPOINT;
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
	CUTIMG,
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

#define GETDRAWTYPE(type)  ((DrawType)type)

bool InDrawDrawType(const DrawType& type) {
	switch (type) {
	case CHOOSEIMG:
	case CHOOSEN:
	case MMOUSEMOVE:
		return false;
	case KZDRAW:
	case DRAWLINE:
	case DRAWCIRCLE:
	case DRAWRECTANGLE:
	case DRAWMULTILINE:
	case DRAWFMULTI:
	case DRAWBCURVE:
	case DRAWCURVE:
		return true;
	}
}

typedef enum KZDrawType {
	KZNONE,
	DRAWCX,				// 画垂线
} KZDrawType;

typedef struct MyDrawState MyDrawState;

typedef void typeGo(MyDrawState& ms, DrawType type);
typedef void typeBack(MyDrawState& ms);

struct MyDrawState {
	bool choose = false;			// 选择状态
	bool draw = false;				// 绘图中
	bool mmove = false;				// mmove
	bool chosen = false;			// 选中状态还是非选中
	DrawType type = DrawType::CHOOSEIMG;
	DrawType lastType = DrawType::CHOOSEIMG;
	std::stack<DrawType> preType;
	typeGo* go = nullptr;
	typeBack* back = nullptr;
	KZDrawType kztype = KZDrawType::KZNONE;
	POINT lastMouseP = INITPOINT;
	POINT lastMMouseBDown = INITPOINT;
	POINT lastMMouseBUp = INITPOINT;
	POINT lastLButtonDown = INITPOINT;
	POINT lastLButtonUp = INITPOINT;
};

void StartChoose(MyDrawState& mst) {
	mst.choose = true;
}

bool InChoose(MyDrawState& mst) {
	return mst.choose;
}

void EndChoose(MyDrawState& mst) {
	mst.choose = false;
}

POINT LButtomDP(const MyDrawState& mst) {
	return mst.lastLButtonDown;
}

void LButtonDown(MyDrawState &mst, POINT point) {
	mst.lastLButtonDown = point;
}

POINT LButtomUP(const MyDrawState& mst) {
	return mst.lastLButtonUp;
}

void LButtonUp(MyDrawState &mst, POINT point) {
	mst.lastLButtonUp = point;
}

POINT MButtomDP(const MyDrawState& mst) {
	return mst.lastMMouseBDown;
}

void MMouseDown(MyDrawState &mst, POINT point) {
	mst.lastMMouseBDown = point;
}

POINT MButtomUP(const MyDrawState& mst) {
	return mst.lastMMouseBUp;
}

void MMouseUp(MyDrawState &mst, POINT point) {
	mst.lastMMouseBUp;
}

POINT MButtomMP(const MyDrawState& mst) {
	return mst.lastMouseP;
}

// TODO:
void MouseMove(MyDrawState &mst, POINT point) {
	mst.lastMouseP = point;
}

bool TwoPointDraw(const POINT& p1, const POINT& p2) {
	return (p1.x != p2.x || p1.y != p2.y) && (p1.x != ILLEGELPOINT && p1.y != ILLEGELPOINT && p2.x != ILLEGELPOINT && p2.y != ILLEGELPOINT);
}

bool InDrawState(const MyDrawState& mst) {
	return InDrawDrawType(mst.type);
}

bool InState(const MyDrawState& mst, DrawType type) {
	return mst.type == type;
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
	else if (mst.type == CUTIMG) {
		mst.type = mst.lastType;
		mst.preType.pop();
	}
}

void InitMyDrawState(MyDrawState& mst) {
	mst.draw = false;
	mst.mmove = false;
	mst.chosen = false;
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

void ClearType(MyDrawState& mst) {
	if (mst.type == CHOOSEN) {
		mst.type = CHOOSEIMG;
	}
	else {
		setType(mst, mst.type);
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

void MoveCoordinateCenter(Coordinate& coor, int x, int y) {
	coor.center.x += x;
	coor.center.y += y;
}

void SetCoordinate(Coordinate& coor, POINT center, const double radius) {
	coor.center = center;
	coor.radius = radius;
}

// 将坐标映射到画布上
POINT mapCoordinate(const Coordinate& coor, double x, double y) {
	POINT pt;
	pt.x = (LONG)(coor.center.x + x / coor.radius);
	pt.y = (LONG)(coor.center.y - y / coor.radius);
	return pt;
}

POINT mapCoordinate(const Coordinate& coor, MyPoint mp) {
	POINT pt;
	pt.x = (LONG)(coor.center.x + mp.x / coor.radius);
	pt.y = (LONG)(coor.center.y - mp.y / coor.radius);
	return pt;
}

// 将画布上的点映射到坐标系上
void PointToCoordinate(const Coordinate& coor, const POINT& pt, double& x, double& y) {
	x = (pt.x - coor.center.x) * coor.radius;
	y = (coor.center.y - pt.y) * coor.radius;
}

POINT* mapMyPoints(MyPoint* mp, const Coordinate& coor, int length, int end) {
	POINT* points = new POINT[length];
	int count = 0;
	for (int i = 0; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coor, pt.x, pt.y);
		}
	}

	return points;
}

vector<POINT> mapMyPointsV(MyPoint* mp, const Coordinate& coor, int length, int end) {
	vector<POINT> points(length);
	int count = 0;
	for (int i = 0; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coor, pt.x, pt.y);
		}
	}

	return points;
}

POINT* mapLastMyPoints(MyPoint* mp, const Coordinate& coor, int length, int end) {
	POINT* points = new POINT[length];
	int count = 0;
	for (int i = end - length; i < end; i++) {
		MyPoint pt = mp[i];
		if (count < length) {
			points[count++] = mapCoordinate(coor, pt.x, pt.y);
		}
	}

	return points;
}

POINT* mapPointsAddOne(MyPoint* mp, const Coordinate& coor, int length, int end, POINT add) {
	POINT* points = new POINT[length + 1];
	int count = 0;
	for (int i = 0; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coor, pt.x, pt.y);
		}
	}
	points[length] = add;

	return points;
}

POINT* mapLastMyPointsAddOne(MyPoint* mp, const Coordinate coor, int length, int end, POINT add) {
	POINT* points = new POINT[length + 1];
	int count = 0;
	for (int i = end - length; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coor, pt.x, pt.y);
		}
	}
	points[length] = add;

	return points;
}

void InitMultipFromV(MultPoint* mps, vector<POINT> points, const Coordinate& coor) {
	if (mps) ClearMultipoint(mps);
	for (POINT p : points) {
		MyPoint mp;
		PointToCoordinate(coor, p, mp.x, mp.y);
		AddPointToMultipoint(mps, mp);
	}
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
	di->info.type = ImgType::NONE;
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

#define HELPXLINERADIUS			1.2
#define MINHIGHORWIDTH			30			// 最窄像素

typedef enum DrawConfigMode {
	ZOOM,			// 缩放
	ROTATE			// 旋转
} DrawConfigMode;

#define CUTFUNC		0

constexpr int AUTOCONFIGLEN = 4;

typedef struct DrawConfig {
	DrawConfigMode mode;
	bool showFLine = true;
	bool inCut = false;
	char AutoConfig[AUTOCONFIGLEN];		// 四个字节
} DrawConfig;

void ClearDrawConfig(DrawConfig& drawconfig) {
	drawconfig.mode = ZOOM;
	drawconfig.showFLine = true;
	drawconfig.inCut = false;
	for (int i = 0; i < AUTOCONFIGLEN; i++) {
		drawconfig.AutoConfig[i] = 0;
	}
}

int GetCutFunc(DrawConfig& drawconfig, int code) {
	return drawconfig.AutoConfig[code];
}

void SetAutoConfig(DrawConfig& config, int code, char func) {
	if (code >= sizeof(int)) return;
	
	config.AutoConfig[code] = func;
}

void ClearAutoConfig(DrawConfig& config, char code) {
	if (code >= sizeof(int)) return;

	config.AutoConfig[code] = 0;
}

void EnterCutMode(DrawConfig& config, int code, char func) {
	SetAutoConfig(config, CUTFUNC, 1);
	config.showFLine = false;
}

void EndCutMode(DrawConfig& config) {
	config.showFLine = true;
}

typedef struct CSDrawInfo {
	int index;
	DrawConfig config;
	DrawInfo choose;
	DrawInfoRect rect;
	CSDrawInfo() : index(-1), choose(), rect(), config() {};
} CSDrawInfo;

RECT getCutRect(POINT start, POINT end) {
	RECT rect;
	rect.left = min(start.x, end.x);
	rect.right = max(start.x, end.x);
	rect.top = min(start.y, end.y);
	rect.bottom = max(start.y, end.y);

	return rect;
}

void StartCut(CSDrawInfo& csdraw) {
	csdraw.config.inCut = true;
}

bool InCut(CSDrawInfo& csdraw) {
	return csdraw.config.inCut;
}

void EndCut(CSDrawInfo& csdraw) {
	csdraw.config.inCut = false;
}

void ClearCSDrawConf(CSDrawInfo& csdraw) {
	csdraw.config.mode = DrawConfigMode::ZOOM;
}

bool HasCSDraw(const CSDrawInfo& csdraw) {
	return csdraw.index != -1;
}

void SetCSDrawMode(CSDrawInfo& csdraw, DrawConfigMode mode) {
	csdraw.config.mode = mode;
}

void InitCSDrawInfo(CSDrawInfo& csdraw) {
	csdraw.index = -1;
	ClearDrawConfig(csdraw.config);
}

void ClearCSDrawInfo(CSDrawInfo& csdraw) {
	if (csdraw.index != -1) {
		ClearDrawInfo(&(csdraw.choose));
		ClearDrawConfig(csdraw.config);
		csdraw.index = -1;
	}
}

// 右上角
MyPoint GetRTMyPoint(const CSDrawInfo & csdraw) {
	return { csdraw.rect.maxX, csdraw.rect.maxY };
}

// 右下角
MyPoint GetRBMyPoint(const CSDrawInfo& csdraw) {
	return { csdraw.rect.maxX, csdraw.rect.minY };
}

// 左上角
MyPoint GetLTMyPoint(const CSDrawInfo& csdraw) {
	return { csdraw.rect.minX, csdraw.rect.maxY };
}

// 左下角 
MyPoint GetLBMyPoint(const CSDrawInfo& csdraw) {
	return { csdraw.rect.minX, csdraw.rect.minY };
}

void FixMinWoH(CSDrawInfo& csdraw, const Coordinate &coor) {
	if (csdraw.rect.maxX - csdraw.rect.minX < MINHIGHORWIDTH * coor.radius) {
		double kz = (MINHIGHORWIDTH * coor.radius - (csdraw.rect.maxX - csdraw.rect.minX)) / 2;
		csdraw.rect.maxX += kz;
		csdraw.rect.minX -= kz;
	}

	if (csdraw.rect.maxY - csdraw.rect.minY < MINHIGHORWIDTH * coor.radius) {
		double kz = (MINHIGHORWIDTH * coor.radius - (csdraw.rect.maxY - csdraw.rect.minY)) / 2;
		csdraw.rect.maxY += kz;
		csdraw.rect.minY -= kz;
	}
}

void CalcCSDrawRect(CSDrawInfo& csdraw, const Coordinate& coor) {
	if (csdraw.index == -1) return;
	csdraw.rect = INITDRAWINFORECT;
	GetDrawInfoRect(&(csdraw.choose), &(csdraw.rect));
	FixMinWoH(csdraw, coor);
	MapDrawInfoRect(&(csdraw.rect), HELPXLINERADIUS);
}

void PopStoreImgToCSDraw(StoreImg& imgs, CSDrawInfo& csdraw, const Coordinate& coor) {
	CopyDrawInfoFromImg(&imgs, &(csdraw.choose), csdraw.index);
	RemoveDrawInfoFromStoreImg(&imgs, csdraw.index);
	CalcCSDrawRect(csdraw, coor);
}

void RestoreCSDraw(StoreImg& imgs, CSDrawInfo& csdraw) {
	SetDrawInfoToStoreImg(&imgs, &(csdraw.choose), csdraw.index);
	ClearCSDrawInfo(csdraw);
}

void RefreshCSDrawPro(CSDrawInfo& csdraw, const DrawUnitProperty& dup) {
	csdraw.choose.proper = dup;
}

typedef struct CSDrawInfoRect {
	MyPoint start;		// 选择矩形起点
	MyPoint end;		// 选择矩形终点
	bool hasChoose;		// 是否是被选择状态
	bool inrect;		// 光标是否在区域内
	int select;			// 选择的图元
} CSDrawInfoRect;

void MoveInRect(CSDrawInfoRect& csdrect) {
	csdrect.inrect = true;
}

void MoveOutRect(CSDrawInfoRect& csdrect) {
	csdrect.inrect = false;
}

bool InRect(CSDrawInfoRect& csdrect) {
	return csdrect.inrect;
}

void StartCSDrawRect(CSDrawInfoRect& csdrect) {
	csdrect.hasChoose = true;
}

void EndCSDrawRect(CSDrawInfoRect& csdrect) {
	csdrect.hasChoose = false;
	csdrect.inrect = false;
}

bool InCSDrawRect(const CSDrawInfoRect& csdrect) {
	return csdrect.hasChoose;
}

void SetCSRectStart(CSDrawInfoRect& csdrect, const MyPoint& point) {
	if (InRect(csdrect)) return;
	csdrect.start.x = point.x;
	csdrect.start.y = point.y;
}

void SetCSRectEnd(CSDrawInfoRect& csdrect, const MyPoint& point) {
	if (InRect(csdrect)) return;
	csdrect.end.x = point.x;
	csdrect.end.y = point.y;
}

bool MyPointInCSDrawInfoRect(const CSDrawInfoRect& csdrect, const MyPoint& point) {
	if (!csdrect.hasChoose) return false;

	double minX, minY, maxX, maxY;
	minX = min(csdrect.start.x, csdrect.end.x);
	maxX = max(csdrect.start.x, csdrect.end.x);
	minY = min(csdrect.start.y, csdrect.end.y);
	maxY = max(csdrect.start.y, csdrect.end.y);

	return point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY;
}

void SetInRect(CSDrawInfoRect& csdrect, const MyPoint& point) {
	if (MyPointInCSDrawInfoRect(csdrect, point)) {
		MoveInRect(csdrect);
	}
	else {
		MoveOutRect(csdrect);
	}
}

void MoveCSDrawInfoRect(CSDrawInfoRect& csdrect, double x, double y) {
	csdrect.start.x += x;
	csdrect.start.y += y;
	csdrect.end.x += x;
	csdrect.end.y += y;
}

void ClearCSDrawRect(CSDrawInfoRect& csdrect) {
	csdrect.start = INITMYPOINT;
	csdrect.end = INITMYPOINT;
	csdrect.hasChoose = false;
	csdrect.inrect = false;
}

// TODO: 工作区?
// 静态数据
SYSTEMMODE systemode = DEFAULTSYSTEMMODE;
WindowState wstate = { 800, 650, 45 };
MyDrawState mst;								// 默认状态
Coordinate coordinate;							// 坐标系
StoreImg allImg;								// 存储所有的图形
DrawingInfo drawing;							// 当前正在绘制的图形
DrawUnitProperty customProperty;				// 自定义绘图
WindowRect wrect;								// 各个组件的位置
ChooseState cs;									// 工具栏状态维护
CSDrawInfo csdraw;								// 被选中的图元
CSDrawInfoRect	csdrect;						// 被选中区域

HDC hdcMemFixed;			// 固定图像内存DC
HDC hdcMemPreview;			// 预览图像内存DC
HDC hdcMemCoS;				// 计算或选中图像内存DC
HBITMAP hbmMemFixed;		// 固定图像位图
HBITMAP hbmMemPreview;		// 预览图像位图
HBITMAP hbmOldFixed;		// 原固定位图
HBITMAP hbmOldPreview;		// 原预览位图
HBITMAP hbmmemCoS;			// 计算或选中图像位图
HBITMAP hbmOldCoS;			// 原计算或选中位图
#endif // WINDOWSIZE_H
