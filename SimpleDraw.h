#pragma once

#include "resource.h"
#include "drawinfo.h"
#include "drawAny.h"
#include "windowState.h"
#include "calculateImg.h"

#define DLLPATH		L"DLL"

using namespace Gdiplus;

ULONG_PTR gdiplusToken;  // GDI+ 初始化令牌
GdiplusStartupInput gdiplusStartupInput; // GDI+ 初始化输入

void DrawBSplineC(HDC hdc, POINT* controlPoints, int degree, int n, const DrawUnitProperty* pro);
void DrawBCurveHelp(HDC hdc, POINT* points, int degree, int n);
void ShowAllCalPoint(HDC hdc, Coordinate coor);

// 初始化 GDI+
void InitGDIPlus() {
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// 关闭 GDI+
void ShutdownGDIPlus() {
	GdiplusShutdown(gdiplusToken);
}

#define MAINCOLOR		RGB(230, 230, 230)
#define SMALLCOLOR		RGB(127, 127, 127)
#define CANVASCOLOR		RGB(255, 255, 255)
#define BLACKCOLOR		RGB(0, 0, 0)
#define CUTRECTCOLOR	RGB(0,255,0)
#define STATUSBARCOLOR	RGB(75, 75, 75)
#define SIDEBARCOLOR	RGB(215, 220, 220)

POINT getClientPos(LPARAM lParam) {
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	return point;
}

POINT getClientPos(HWND hWnd) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);
	return point;
}

#define COORDILINE		RGB(0, 0, 0)		// 默认颜色
#define COORDISLINE		RGB(230, 230, 230)	// 坐标线颜色
#define COORDIWIDTH		1		// 坐标及其他线宽
#define COORDIKWIDTH	2		// 刻度宽度

// TODO: 改进坐标线绘制
void drawCoordinate(HDC hdc, POINT center, int width, int height) {
	// 画坐标系
	HPEN hPen = CreatePen(PS_SOLID, COORDIWIDTH, COORDILINE); // 黑色画笔

	HPEN hWPen = CreatePen(PS_SOLID, COORDIKWIDTH, COORDILINE);

	HPEN hPenLight = CreatePen(PS_SOLID, COORDIWIDTH, COORDISLINE); // 浅色画笔

	LOGBRUSH lb;
	lb.lbStyle = BS_NULL;
	lb.lbColor = 0;       // 黑色
	lb.lbHatch = 0;     // 不使用图案
	HBRUSH hNullBrush = CreateBrushIndirect(&lb);

	LOGBRUSH lbb;
	lbb.lbStyle = BS_SOLID;
	lbb.lbColor = 0;
	lbb.lbHatch = 0;
	HBRUSH hBlackBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hBlackBrush);

	// 显示基本横坐标纵坐标，和浅色坐标轴

	double radius = coordinate.radius;		// 单位/像素

	int g = (int)(MINXPERZ * radius);

	g = (div(g, 2).quot + 1) * 2;
	// 转换为像素间隔
	double wpx = g / radius;

	// 纵轴线
	SelectObject(hdc, hPenLight);
	for (double i = center.x + wpx; i < width; i += wpx) {
		MoveToEx(hdc, i, 0, NULL);
		LineTo(hdc, i, height);
	}

	for (double i = center.x - wpx; i > 0; i -= wpx) {
		MoveToEx(hdc, i, 0, NULL);
		LineTo(hdc, i, height);
	}

	// 横轴线
	for (double i = center.y + wpx; i < height; i += wpx) {
		MoveToEx(hdc, 0, i, NULL);
		LineTo(hdc, width, i);
	}

	for (double i = center.y - wpx; i > 0; i -= wpx) {
		MoveToEx(hdc, 0, i, NULL);
		LineTo(hdc, width, i);
	}

	SelectObject(hdc, hWPen);
	int g1 = g;
	// 每隔5条线显示一个数字
	for (double i = center.x + wpx * 5; i < width; i += wpx * 5, g1 += g) {
		wchar_t str[10];
		swprintf_s(str, L"%d", g1 * 5);
		MoveToEx(hdc, i, center.y, NULL);
		LineTo(hdc, i, center.y - 5);
		TextOut(hdc, i, center.y + 5, str, (int)wcslen(str));
	}

	g1 = g;
	for (double i = center.x - wpx * 5; i > 0; i -= wpx * 5, g1 += g) {
		wchar_t str[10];
		swprintf_s(str, L"%d", -g1 * 5);
		MoveToEx(hdc, i, center.y, NULL);
		LineTo(hdc, i, center.y - 5);
		TextOut(hdc, i, center.y + 5, str, (int)wcslen(str));
	}

	g1 = g;
	for (double i = center.y + wpx * 5; i < height; i += wpx * 5, g1 += g) {
		wchar_t str[10];
		swprintf_s(str, L"%d", -g1 * 5);
		MoveToEx(hdc, center.x, i, NULL);
		LineTo(hdc, center.x + 5, i);
		TextOut(hdc, center.x + 5, i + 2, str, (int)wcslen(str));
	}

	g1 = g;
	for (double i = center.y - wpx * 5; i > 0; i -= wpx * 5, g1 += g) {
		wchar_t str[10];
		swprintf_s(str, L"%d", g1 * 5);
		MoveToEx(hdc, center.x, i, NULL);
		LineTo(hdc, center.x + 5, i);
		TextOut(hdc, center.x + 5, i + 2, str, (int)wcslen(str));
	}

	SelectObject(hdc, hPen);
	// 在坐标系中心点，画一个小圆点，表示坐标原点
	Ellipse(hdc, center.x - 2, center.y - 2, center.x + 2, center.y + 2);
	// 坐标原点O
	TextOut(hdc, center.x - 12, center.y, L"O", 1);

	// 画一条垂直和水平的线
	MoveToEx(hdc, 0, center.y, NULL);
	LineTo(hdc, width, center.y);
	MoveToEx(hdc, center.x, 0, NULL);
	LineTo(hdc, center.x, height);

	SelectObject(hdc, hWPen);
	// 横轴箭头
	MoveToEx(hdc, width - 5, center.y - 5, NULL);
	LineTo(hdc, width, center.y);
	MoveToEx(hdc, width - 5, center.y + 5, NULL);
	LineTo(hdc, width, center.y);
	// 纵轴箭头
	MoveToEx(hdc, center.x - 5, 5, NULL);
	LineTo(hdc, center.x, 0);
	MoveToEx(hdc, center.x + 5, 5, NULL);
	LineTo(hdc, center.x, 0);

	DeleteObject(hPenLight);
	DeleteObject(hNullBrush);
	DeleteObject(hBlackBrush);
	DeleteObject(hPen);
}

void DrawLineM(HDC hdc, const MyPoint& mstart, const MyPoint& mend, const DrawUnitProperty *pro) {
	// 将坐标转换为屏幕坐标
	POINT start = mapCoordinate(coordinate, mstart.x, mstart.y);
	POINT end = mapCoordinate(coordinate, mend.x, mend.y);
	// 需要进行坐标转换
	DrawLine(hdc, start, end, pro);
}

void DrawCircleM(HDC hdc, const MyPoint& center, double radius, const DrawUnitProperty* pro) {
	POINT pt = mapCoordinate(coordinate, center.x, center.y);
	int r = (int)(radius / coordinate.radius);
	DrawCircle(hdc, pt, r, pro);
}

void DrawCircleM(HDC hdc, const MyPoint& center, const MyPoint& end, const DrawUnitProperty* pro) {
	POINT pt = mapCoordinate(coordinate, center.x, center.y);
	POINT pt2 = mapCoordinate(coordinate, end.x, end.y);
	int r = (int)sqrt((pt2.x - pt.x) * (pt2.x - pt.x) + (pt2.y - pt.y) * (pt2.y - pt.y));
	DrawCircle(hdc, pt, r, pro);
}

void DrawRectangleM(HDC hdc, const MyPoint& mstart, const MyPoint& mend, const DrawUnitProperty* pro) {
	POINT start = mapCoordinate(coordinate, mstart.x, mstart.y);
	POINT end = mapCoordinate(coordinate, mend.x, mend.y);
	DrawRectangle(hdc, start, end, pro);
}

void DrawRectangleI(HDC hdc, const MyRectangle& tangle, const DrawUnitProperty* pro) {
	POINT start = mapCoordinate(coordinate, tangle.start.x, tangle.start.y);
	POINT end = mapCoordinate(coordinate, tangle.end.x, tangle.end.y);
	POINT add1 = mapCoordinate(coordinate, tangle.add1.x, tangle.add1.y);
	POINT add2 = mapCoordinate(coordinate, tangle.add2.x, tangle.add2.y);
	DrawRectangleF(hdc, start, end, add1, add2, pro);
}

void DrawMultiLineM(HDC hdc, MyPoint* mpoints, int numPoints, int endNum, const DrawUnitProperty* pro) {
	POINT* points = mapMyPoints(mpoints, coordinate, numPoints, endNum);
	DrawMultiLine(hdc, points, numPoints, pro);
	delete[] points;
}

void DrawFMultiLineM(HDC hdc, MyPoint* mpoints, int numPoints, int endNum, const DrawUnitProperty* pro) {
	POINT* points = mapMyPoints(mpoints, coordinate, numPoints, endNum);
	DrawFMultiLine(hdc, points, numPoints, pro);
	delete[] points;
}

void DrawBCurveM(HDC hdc, MyPoint* mpoints, int numPoints, int endNum, const DrawUnitProperty* pro) {
	POINT* points = mapMyPoints(mpoints, coordinate, numPoints, endNum);
	DrawBSplineC(hdc, points, BSPLINE, numPoints, pro);
	delete[] points;
}

void DrawCurveM(HDC hdc, MyPoint* points, int numPoints, int endNum, const DrawUnitProperty* pro) {
	Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[numPoints];
	int count = 0;
	for (int i = 0; i < endNum; i++) {
		MyPoint pt = points[i];
		if (pt.x == ILLEGELMYPOINT || pt.y == ILLEGELMYPOINT) continue;
		POINT p = mapCoordinate(coordinate, pt.x, pt.y);
		if (count < numPoints) {
			gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
		}
	}
	if (count == numPoints) {
		// 使用GDI+绘图
		Graphics graphics(hdc);
		int color = pro->color;
		// 提取 ARGB 组件
		int red = color & 0xFF;   // 提取 Red 分量
		int green = (color >> 8) & 0xFF;  // 提取 Green 分量
		int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

		Pen pen(Color(255, red, green, blue), pro->width);
		graphics.DrawCurve(&pen, gdiplusPoints, numPoints);
	}
	delete[]gdiplusPoints;
}

void drawDrawInfo(HDC hdc, DrawInfo *item) {
	switch (item->type) {
		case LINE:
		{
			DrawLineM(hdc, item->line.start, item->line.end, &item->proper);
			break;
		}
		case CIRCLE:
		{
			DrawCircleM(hdc, item->circle.center, item->circle.radius, &item->proper);
			break;
		}
		case RECTANGLE:
		{
			// 画矩形
			DrawRectangleI(hdc, item->rectangle, &item->proper);
			break;
		}
		case MULTILINE:
		{
			// 画多义线
			if (item->multipoint.numPoints > 0) {
				DrawMultiLineM(hdc, item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum, &item->proper);
			}
			break;
		}
		case FMULTILINE:
		{
			// 画封闭多义线
			if (item->multipoint.numPoints > 0) {
				DrawFMultiLineM(hdc, item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum, &item->proper);
			}
			break;
		}
		case BCURVE:
		{
			if (item->multipoint.numPoints > 0) {
				DrawBCurveM(hdc, item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum, &item->proper);
			}

			break;
		}
		case CURVE:
		{
			// 画曲线
			if (item->multipoint.numPoints > 0) {
				DrawCurveM(hdc, item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum, &item->proper);
			}
			break;
		}
		default:
			break;
	}
}

void drawStoreImg(HDC hdc, StoreImg* imgs) {
	for (int i = 0; i < imgs->endNum; i++) {
		DrawInfo item = imgs->img[i];
		drawDrawInfo(hdc, &item);
	}
}

/*
若当前绘制图像已经有部分绘制到固定画布，则需要覆盖它

多个点绘制的图形，预览暂时不刷新
*/

void drawDrawing(HDC hdc, const DrawingInfo* drawing, const DrawUnitProperty* pro) {
	if (drawing->lastRem.x == ILLEGELMYPOINT && drawing->lastRem.y == ILLEGELMYPOINT) return;
	// 创建画笔
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	// 创建画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hPen);
	SelectObject(hdc, hNullBrush);
	switch (drawing->info.type) {
	case LINE:
	{
		MyPoint first = drawing->info.line.start;
		MyPoint end = drawing->lastRem;
		if (HFMyPoint(&first) && HFMyPoint(&end)) {
			DrawLineM(hdc, first, end, pro);
		}
	}
	break;
	case CIRCLE:
	{
		MyPoint center = drawing->info.circle.center;
		MyPoint end = drawing->lastRem;
		if (HFMyPoint(&center) && HFMyPoint(&end)) {
			DrawCircleM(hdc, center, end, pro);
		}
	}
	break;
	case RECTANGLE:
	{
		MyPoint start = drawing->info.rectangle.start;
		MyPoint end = drawing->lastRem;
		if (HFMyPoint(&start) && HFMyPoint(&end)) {
			DrawRectangleM(hdc, start, end, pro);
		}
	}
	break;
	case CURVE:
	{
		// 画曲线
		if (drawing->info.multipoint.numPoints > 0) {
			DrawCurveM(hdc, drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum, pro);
		}
		break;
	}
	case MULTILINE:
	{
		// 画多义线
		if (drawing->info.multipoint.numPoints > 0) {
			DrawMultiLineM(hdc, drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum, pro);
		}
		break;
	}
	case FMULTILINE:
	{
		if (drawing->info.multipoint.numPoints > 0) {
			DrawFMultiLineM(hdc, drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum, pro);
		}
		break;
	}
	case BCURVE:
	{
		if (drawing->info.multipoint.numPoints > 0) {
			DrawBCurveM(hdc, drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum, pro);
		}
		break;
	}
	default:
		break;
	}
	DeleteObject(hPen);
	DeleteObject(hNullBrush);
}

void DrawDrawInfoRect(HDC hdc, const DrawInfoRect& rect) {
	MyPoint mp1 = { rect.minX, rect.minY };
	MyPoint mp2 = { rect.minX, rect.maxY };
	MyPoint mp3 = { rect.maxX, rect.maxY };
	MyPoint mp4 = { rect.maxX, rect.minY };
	POINT p1 = mapCoordinate(coordinate, mp1.x, mp1.y);
	POINT p2 = mapCoordinate(coordinate, mp2.x, mp2.y);
	POINT p3 = mapCoordinate(coordinate, mp3.x, mp3.y);
	POINT p4 = mapCoordinate(coordinate, mp4.x, mp4.y);
	DrawXLine(hdc, p1, p2, HELPLINECORLOR, 1);
	DrawXLine(hdc, p2, p3, HELPLINECORLOR, 1);
	DrawXLine(hdc, p3, p4, HELPLINECORLOR, 1);
	DrawXLine(hdc, p4, p1, HELPLINECORLOR, 1);
}

#define HELPSIZE	3

// 绘制选中图形的辅助线
void drawCosCSDraw(HDC hdc, CSDrawInfo* csdraw) {
	if (csdraw->index == -1) return;
	if (!csdraw->config.showFLine) return;
	DrawDrawInfoRect(hdc, csdraw->rect);
	switch (csdraw->choose.type) {
	case LINE:
		DrawLineHelp(hdc, csdraw->choose.line, HELPSIZE, HELPPOINTCOLOR);
		break;
	case RECTANGLE:
		DrawRectangleHelp(hdc, csdraw->choose.rectangle, HELPSIZE, HELPPOINTCOLOR);
		break;
	case CIRCLE:
		DrawCircleHelp(hdc, csdraw->choose.circle, HELPSIZE, HELPPOINTCOLOR);
		break;
	case MULTILINE:
	case FMULTILINE:
	case CURVE:
		DrawMultiPointHelpNoL(hdc, &(csdraw->choose.multipoint));
		break;
	case BCURVE:
		POINT* pts = mapMyPoints(csdraw->choose.multipoint.points, coordinate, csdraw->choose.multipoint.numPoints, csdraw->choose.multipoint.endNum);
		DrawBCurveHelp(hdc, pts, BSPLINE, csdraw->choose.multipoint.numPoints);
		delete[] pts;
		break;
	}
}

void drawCSDraw(HDC hdc, CSDrawInfo* csdraw, const DrawUnitProperty* pro) {
	if (csdraw->index == -1) return;
	DrawInfo choose = csdraw->choose;
	choose.proper = *pro;
	drawDrawInfo(hdc, &choose);
}

void drawCSDrawInfoRectM(HDC hdc, POINT move) {
	POINT start = mapCoordinate(coordinate, csdrect.start.x, csdrect.start.y);
	drawCSDrawRectP(hdc, move, start);
}

void drawCSDrawInfoRect(HDC hdc, const CSDrawInfoRect& rect) {
	if (rect.hasChoose) {
		POINT start = mapCoordinate(coordinate, rect.start.x, rect.start.y);
		POINT end = mapCoordinate(coordinate, rect.end.x, rect.end.y);
		drawCSDrawRectP(hdc, start, end);
	}
}

void MoveCSDrawRect(int x, int y) {
	double dx, dy;
	dx = coordinate.radius * x;
	dy = - coordinate.radius * y;
	MoveCSDrawInfoRect(csdrect, dx, dy);
}

// 图形计算或者辅助线显示
void drawCoSDrawing(HDC hdc, DrawingInfo* drawing) {
	switch (drawing->info.type) {
	case BCURVE:
	{
		if (drawing->info.multipoint.numPoints > 0) {
			POINT* points = mapMyPoints(drawing->info.multipoint.points, coordinate, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum);
			DrawBCurveHelp(hdc, points, BSPLINE, drawing->info.multipoint.numPoints);
			delete[] points;
		}
		break;
	}
	}
}

// 固定画布重绘
void RedrawFixedContent(HWND hCWnd, HDC hdc) {
	RECT rect;
	GetClientRect(hCWnd, &rect);

	// 重新填充背景色
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdc, &rect, hBrush);

	if (coordinate.radius == 0) {
		POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
		SetCoordinate(coordinate, center, DEFAULTRADIUS); // 设置坐标系参数
	}
	// 加载坐标系, 最底层
	drawCoordinate(hdc, coordinate.center, rect.right, rect.bottom);

	// 加载保存的图形，第一层
	drawStoreImg(hdc, &allImg);

	// 加载正在绘制的图形，第二层
	drawDrawing(hdc, &drawing, &customProperty);
}

void drawCosCalcPoint(HDC hdc) {
	if (csdraw.index == -1) return;
	if (!csdraw.config.showFLine) return;
	CalculateImg(allImg, csdraw);
	ShowAllCalPoint(hdc, coordinate);
}

// 中间窗口重绘
void RedrawCoSContent(HWND hCWnd, HDC hdc) {
	RECT rect;
	GetClientRect(hCWnd, &rect);

	// 重新填充背景色
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdc, &rect, hBrush);

	drawCoSDrawing(hdc, &drawing);
	drawCosCSDraw(hdc, &csdraw);
	drawCosCalcPoint(hdc);
	drawCSDrawInfoRect(hdc, csdrect);
}

void EnableMouseTracking(HWND hWnd) {
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE; // 启用鼠标离开事件
	tme.hwndTrack = hWnd;    // 需要跟踪的窗口句柄
	tme.dwHoverTime = 0;     // 设置为0以立即触发事件

	TrackMouseEvent(&tme);
}

void ShowCutWindow(HDC hdc, POINT point) {
	POINT lastp = mst.lastLButtonDown;

	DrawLine(hdc, {lastp.x, point.y}, {lastp.x, lastp.y}, 2, CUTRECTCOLOR);
	DrawLine(hdc, {lastp.x, lastp.y}, {point.x, lastp.y}, 2, CUTRECTCOLOR);
	DrawLine(hdc, {point.x, lastp.y}, {point.x, point.y}, 2, CUTRECTCOLOR);
	DrawLine(hdc, { point.x, point.y }, { lastp.x, point.y }, 2, CUTRECTCOLOR);
}

void ShowPointInWindow(HDC hdc, MyPoint mp) {
	POINT pt = mapCoordinate(coordinate, mp.x, mp.y);
	// 在预览窗口显示当前点的坐标
	std::wstring str = L"(" + std::to_wstring(mp.x) + L", " + std::to_wstring(mp.y) + L")";
	// 创建一个字体，字体高度为24，使用默认字体
	HFONT hFont = CreateFont(
		14,              // 字体高度
		0,               // 字符宽度（为0表示使用默认宽度）
		0,               // 文本倾斜角度
		0,               // 基线倾斜角度
		FW_NORMAL,       // 字体粗细
		FALSE,           // 斜体
		FALSE,           // 下划线
		FALSE,           // 删除线
		DEFAULT_CHARSET, // 字符集
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, // 字体族和间距
		L"Arial");       // 字体名称

	// 将字体选择到内存 DC 中
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	TextOut(hdc, pt.x + 3, pt.y, str.c_str(), str.length());
	// 恢复原始字体
	SelectObject(hdc, hOldFont);

	// 删除自定义字体对象
	DeleteObject(hFont);
}

void ShowPointInWindow(HDC hdc, POINT p) {
	// 在预览窗口显示当前点的坐标
	MyPoint mp;
	PointToCoordinate(coordinate, p, mp.x, mp.y);
	std::wstring str = L"(" + std::to_wstring(mp.x) + L", " + std::to_wstring(mp.y) + L")";
	// 创建一个字体，字体高度为24，使用默认字体
	HFONT hFont = CreateFont(
		14,              // 字体高度
		0,               // 字符宽度（为0表示使用默认宽度）
		0,               // 文本倾斜角度
		0,               // 基线倾斜角度
		FW_NORMAL,       // 字体粗细
		FALSE,           // 斜体
		FALSE,           // 下划线
		FALSE,           // 删除线
		DEFAULT_CHARSET, // 字符集
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, // 字体族和间距
		L"Arial");       // 字体名称

	// 将字体选择到内存 DC 中
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	TextOut(hdc, p.x + 3, p.y, str.c_str(), str.length());
	// 恢复原始字体
	SelectObject(hdc, hOldFont);

	// 删除自定义字体对象
	DeleteObject(hFont);
}

void ShowAllCalPoint(HDC hdc, Coordinate coor) {
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBlackBrush);
	// 在预览窗口显示所有计算点
	for (int i = 0; i < CalculatePoints.size(); i++) {
		MyPoint mp = CalculatePoints[i];
		POINT pt = mapCoordinate(coor, mp.x, mp.y);
		// 画圆心
		SelectObject(hdc, hPen);
		SelectObject(hdc, hBlackBrush);
		Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
		// 显示坐标
		ShowPointInWindow(hdc, mp);
	}
	DeleteObject(hPen);
	DeleteObject(hBlackBrush);
}

void UpdateStatusBarRadius(HWND hStatusBar, double r) {
	wchar_t textR[100];

	swprintf_s(textR, L"R: %f", r);
	SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)textR);

	return;
}

void UpdateStatusBarText(HWND hStatusBar, const wchar_t* text) {
	SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)text);
	return;
}

// 更新状态栏中的坐标
void UpdateStatusBarCoordinates(HWND hStatusBar, double x, double y) {
	wchar_t textX[100], textY[100];

	// 更新 X 坐标文本
	swprintf_s(textX, L"X: %.2f", x);
	SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)textX);

	// 更新 Y 坐标文本
	swprintf_s(textY, L"Y: %.2f", y);
	SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)textY);

	return;
}

void SetToolBarCheck(HWND toolbar, ChooseState &cs, int id) {
	if (cs.choose != -1) {
		// 取消之前的选择
		SendMessage(toolbar, TB_CHECKBUTTON, cs.choose, FALSE);
	}
	
	// 设置新的选择
	if (id != -1) {
		SendMessage(toolbar, TB_CHECKBUTTON, id, TRUE);
	}
	
	SetActiveID(cs, id);
}

void DrawBSplineC(HDC hdc, POINT* controlPoints, int degree, int n, const DrawUnitProperty* pro) {
	if (degree + 1 > n) return;
	Graphics graphics(hdc);
	Pen pen(Color(255, GetRValue(pro->color), GetGValue(pro->color), GetBValue(pro->color)), pro->width);

	// 二阶基函数矩阵
	double M2[3][3] = {
		{0.5, -1, 0.5},
		{0.5, 1, -1},
		{0, 0, 0.5}
	};

	// 三阶基函数矩阵
	double M3[4][4] = {
		{1.0 / 6, -0.5, 0.5, -1.0 / 6},
		{4.0 / 6, 0, -1, 0.5},
		{1.0 / 6, 0.5, 0.5, -0.5},
		{0 ,0 , 0 , 1.0 / 6}
	};

	// 控制点的数量
	int numPoints = degree + 1;
	const int numSegments = 20;
	double* T = new double[numPoints];
	double lastX = 0, lastY = 0;
	double pointX = 0, pointY = 0;
	for (int i = 0; i < n - degree; i++) {

		// 0 << t << 1
		for (int k = 0; k <= numSegments; k++) {
			pointX = 0;
			pointY = 0;

			double t = k / (double)numSegments;

			// 为 degree = 2 和 degree = 3 分别计算基函数	{ 1, t, t * t.... }
			T[0] = 1;
			for (int m = 1; m < numPoints; m++) {
				T[m] = T[m - 1] * t;
			}

			// 计算曲线点
			for (int row = 0; row < numPoints; row++) {
				double basis = 0;
				for (int col = 0; col < numPoints; col++) {
					if (degree == 2) {
						basis += M2[row][col] * T[col];
					}
					else if(degree == 3) {
						basis += M3[row][col] * T[col];
					}
				}
				pointX += basis * controlPoints[i + row].x;
				pointY += basis * controlPoints[i + row].y;
			}

			// 绘制曲线段
			if (lastX != 0 && lastY != 0) {
				graphics.DrawLine(&pen, static_cast<REAL>(lastX), static_cast<REAL>(lastY), static_cast<REAL>(pointX), static_cast<REAL>(pointY));
			}

			// 更新 lastX 和 lastY
			lastX = pointX;
			lastY = pointY;
		}
	}

	delete[] T;
}

// 绘制 B 样条曲线，使用 MyPoint* 作为控制点数组
void DrawBCurve(HDC hdc, POINT* points, int degree, int n, const DrawUnitProperty* pro) {
	// 画虚线
	for (int i = 0; i < n - 1; i++) {
		DrawXLine(hdc, points[i], points[i+1], HELPLINECORLOR, 1);
	}
	
	// 画隔一个点的虚线
	for (int i = 0; i < n - 2; i++) {
		DrawXLine(hdc, points[i], points[i + 2], HELPLINECORLOR, 1);
	}

	// 隔一个的线的中点与两个点之间的点相连
	POINT mid;
	POINT third;
	int r = degree;
	for (int i = 0; i < n - 2; i++) {
		mid.x = (points[i].x + points[i + 2].x) / 2;
		mid.y = (points[i].y + points[i + 2].y) / 2;
		DrawXLine(hdc, mid, points[i + 1], HELPLINECORLOR, 1);

		// 将中点和其三分一处标红
		third.x = (points[i + 1].x * (r - 1) + mid.x) / r;
		third.y = (points[i + 1].y * (r - 1) + mid.y) / r;

		DrawPoint(hdc, mid.x, mid.y, 3, HELPPOINTCOLOR);
		DrawPoint(hdc, third.x, third.y, 3, HELPPOINTCOLOR);
	}

	DrawBSplineC(hdc, points, degree, n, pro);
}


void DrawABCurveHelp(HDC hdc, POINT start, POINT middle, POINT end, int degree) {
	POINT seM, semM;
	seM.x = (start.x + end.x) / 2;
	seM.y = (start.y + end.y) / 2;
	semM.x = (middle.x * (degree - 1) + seM.x) / degree;
	semM.y = (middle.y * (degree - 1) + seM.y) / degree;

	DrawXLine(hdc, middle, end, HELPLINECORLOR, 1);
	DrawXLine(hdc, start, end, HELPLINECORLOR, 1);
	DrawXLine(hdc, seM, middle, HELPLINECORLOR, 1);
	DrawPoint(hdc, seM.x, seM.y, 3, HELPPOINTCOLOR);
	DrawPoint(hdc, semM.x, semM.y, 3, HELPPOINTCOLOR);
}

void DrawFBCurve(HDC hdc, POINT* points, int degree, const DrawUnitProperty* pro) {
	DrawBSplineC(hdc, points, degree, degree + 1, pro);
}

// TODO: 贝塞尔曲线
void DrawBezier(HDC hdc, POINT* points, int degree, int n, const DrawUnitProperty* pro) {
	if (degree + 1 > n) return;
	Graphics graphics(hdc);
	Pen pen(Color(255, GetRValue(pro->color), GetGValue(pro->color), GetBValue(pro->color)), pro->width);

	// 二阶基函数矩阵
	double M2[3][3] = {
		{1, -2, 1},
		{0, 2, -2},
		{0, 0, 1}
	};

	// 三阶基函数矩阵
	double M3[4][4] = {
		{1, -3, 3, -1},
		{0, 3, -6, 3},
		{0, 0, 3, -3},
		{0, 0, 0, 1}
	};

	// 控制点的数量
	int numPoints = degree + 1;
	const int numSegments = 20;
	double* T = new double[numPoints];
	double lastX = 0, lastY = 0;
	double pointX = 0, pointY = 0;
	for (int i = 0; i < n - degree; i++) {

		// 0 << t << 1
		for (int k = 0; k <= numSegments; k++) {
			pointX = 0;
			pointY = 0;

			double t = k / (double)numSegments;

			// 为 degree = 2 和 degree = 3 分别计算基函数	{ 1, t, t * t.... }
			T[0] = 1;
			for (int m = 1; m < numPoints; m++) {
				T[m] = T[m - 1] * t;
			}

			// 计算曲线点
			for (int row = 0; row < numPoints; row++) {
				double basis = 0;
				for (int col = 0; col < numPoints; col++) {
					if (degree == 2) {
						basis += M2[row][col] * T[col];
					}
					else if (degree == 3) {
						basis += M3[row][col] * T[col];
					}
				}
				pointX += basis * points[i + row].x;
				pointY += basis * points[i + row].y;
			}

			// 绘制曲线段
			if (lastX != 0 && lastY != 0) {
				graphics.DrawLine(&pen, static_cast<REAL>(lastX), static_cast<REAL>(lastY), static_cast<REAL>(pointX), static_cast<REAL>(pointY));
			}

			// 更新 lastX 和 lastY
			lastX = pointX;
			lastY = pointY;
		}
	}

	delete[] T;
}

FileOpenAndSave SaveGTXFile(HWND hWnd) {
	// 创建一个 OPENFILENAME 结构体
	OPENFILENAME ofn;
	wchar_t szFile[260];       // 文件路径缓冲区

	// 初始化 OPENFILENAME 结构体
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd; // 父窗口句柄
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0'; // 初始化文件名
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Images (*.gtx)\0*.gtx\0All Files (*.*)\0*.*\0"; // 文件类型过滤器
	ofn.lpstrTitle = L"Save Image File"; // 对话框标题
	ofn.Flags = OFN_OVERWRITEPROMPT; // 如果文件已存在，提示覆盖

	// 填充默认后缀
	wcscpy_s(szFile, L"yourImgName.gtx");

	// 弹出文件保存对话框
	if (GetSaveFileName(&ofn)) {
		return StoreImgToFile(&allImg, ofn.lpstrFile); // 保存到文件
	}
	else {
		return DIALOGOPENFAILE;
	}
}

FileOpenAndSave OpenGTXFile(HWND hWnd) {
	// 创建一个 OPENFILENAME 结构体
	OPENFILENAME ofn;
	wchar_t szFile[260];       // 文件路径缓冲区

	// 初始化 OPENFILENAME 结构体
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd; // 父窗口句柄
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0'; // 初始化文件名
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Images (*.gtx)\0*.gtx\0All Files (*.*)\0*.*\0"; // 文件类型过滤器
	ofn.lpstrTitle = L"Open Image File"; // 对话框标题
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; // 文件必须存在且路径必须有效

	// 弹出文件打开对话框
	if (GetOpenFileName(&ofn)) {
		return FileToStoreImg(&allImg, ofn.lpstrFile); // 加载文件
	}
	else {
		return DIALOGOPENFAILE;
	}
}

double GetRadiusFromWParam(WPARAM wParam) {
	// 放大时,坐标系radius减小，缩小时，坐标系radius增大
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	// 缩放因子，可以调整为更适合的值
	double scaleFactor = RADIUSCHANGESPEED;

	// 使用指数缩放，确保 radius 始终大于0
	return exp(scaleFactor * zDelta);
}

double GetAngleFromWParam(WPARAM wParam) {
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	// 角度比例因子
	double angle = zDelta * ANGLECHANGESPEED;
	return angle;
}

void RefreshRadius(double radius) {
	// 使用指数缩放，确保 radius 始终大于0
	coordinate.radius /= radius;

	// 防止缩放比例过小或过大
	//if (coordinate.radius < MINRADIUS) {
	//	coordinate.radius = MINRADIUS; // 限制最小缩放比例
	//}
	//else if (coordinate.radius > MAXRADIUS) {
	//	coordinate.radius = MAXRADIUS; // 限制最大缩放比例
	//}
	return;
}

bool LButtonClick(const POINT &point) {
	return mst.lastLButtonDown.x == point.x && mst.lastLButtonDown.y == point.y;
}

void StartDraw() {
	mst.draw = true;
}

bool InDraw() {
	return mst.draw;
}

void StartMMove() {
	mst.mmove = true;
}

bool InMMove() {
	return mst.mmove;
}

void EndMMove() {
	mst.mmove = false;
}

void SetChosen(bool p) {
	mst.chosen = p;
}

bool IsChosen() {
	return mst.chosen;
}

bool OnlyOnePoint() {
	return drawing.info.multipoint.numPoints == 1;
}

bool BCurvePointCannotDraw() {
	return drawing.info.multipoint.numPoints <= BSPLINE;
}

void EndDraw() {
	mst.draw = false;
	ClearStateP(mst);	// TODO: 是否可以不清除
}

bool PointInCSDraw(const POINT& point) {
	MyPoint mp;
	PointToCoordinate(coordinate, point, mp.x, mp.y);
	return MyPointInCSDrawInfo(csdraw.rect, mp);
}

bool MyPointCSDraw(const MyPoint& mp) {
	if (csdraw.index == -1) return false;
	return MyPointInCSDrawInfo(csdraw.rect, mp);
}

void MoveCSDrawInPoint(int x, int y) {
	double dx = x * coordinate.radius;
	double dy = - y * coordinate.radius;
	MoveInfoBy(&(csdraw.choose), dx, dy);
	CalcCSDrawRect(csdraw, coordinate);
}

void ZoomCSDrawMyPoint(const MyPoint& center, double scale) {
	ZoomDrawInfo(csdraw.choose, center, scale);
	CalcCSDrawRect(csdraw, coordinate);
}

void RotateCSDraw(CSDrawInfo& csdraw, const MyPoint& mp, double angle) {
	RotateDrawInfo(csdraw.choose, mp, angle);
	CalcCSDrawRect(csdraw, coordinate);
}

void ZoomWindowCoordinate(const POINT& pt, double scale) {
	ZoomCoordinate(coordinate, pt, scale);
}

void MouseWheel(HWND hCWnd, POINT point, WPARAM wParam) {
	MyPoint mp;
	PointToCoordinate(coordinate, point, mp.x, mp.y);

	// 有被选中的图元
	if (HasCSDraw(csdraw)) {
		switch (csdraw.config.mode) {
		case ZOOM:
		{
			double radius = GetRadiusFromWParam(wParam);
			ZoomCSDrawMyPoint(mp, radius);
			break;
		}
		case ROTATE:
		{
			double angle = GetAngleFromWParam(wParam);
			// TODO: point
			RotateCSDraw(csdraw, mp, angle);
			break;
		}
		}
	}
	else {
		// scale
		double scale = GetRadiusFromWParam(wParam);

		// 放大时,坐标系radius减小，缩小时，坐标系radius增大
		RefreshRadius(scale);

		// 更新 center
		ZoomWindowCoordinate(point, scale);

		RedrawFixedContent(hCWnd, hdcMemFixed); // 重绘固定内容

		// 触发鼠标移动事件
		LPARAM l = MAKELPARAM(point.x, point.y);
		WPARAM w = 0;
		PostMessage(hCWnd, WM_MOUSEMOVE, w, l);
	}
}