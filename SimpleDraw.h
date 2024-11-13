#pragma once

#include "resource.h"
#include "drawinfo.h"
#include "drawAny.h"
#include "windowState.h"

using namespace Gdiplus;

ULONG_PTR gdiplusToken;  // GDI+ 初始化令牌
GdiplusStartupInput gdiplusStartupInput; // GDI+ 初始化输入

void DrawBSplineC(HDC hdc, POINT* controlPoints, int degree, int n, const DrawUnitProperty* pro);
void DrawBCurveHelp(HDC hdc, POINT* points, int degree, int n, const DrawUnitProperty* pro);

// 初始化 GDI+
void InitGDIPlus() {
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// 关闭 GDI+
void ShutdownGDIPlus() {
	GdiplusShutdown(gdiplusToken);
}

#define MAINCOLOR  RGB(230, 230, 230)
#define SMALLCOLOR RGB(127, 127, 127)
#define CANVASCOLOR RGB(255, 255, 255)
#define STATUSBARCOLOR RGB(75, 75, 75)
#define SIDEBARCOLOR RGB(215, 220, 220)

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

POINT* mapMyPoints(MyPoint* mp, int length, int end) {
	POINT* points = new POINT[length];
	int count = 0;
	for (int i = 0; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
		}
	}

	return points;
}

POINT* mapLastMyPoints(MyPoint* mp, int length, int end) {
	POINT* points = new POINT[length];
	int count = 0;
	for (int i = end - length; i < end; i++) {
		MyPoint pt = mp[i];
		points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
	}

	return points;
}

POINT* mapPointsAddOne(MyPoint* mp, int length, int end, POINT add) {
	POINT* points = new POINT[length+1];
	int count = 0;
	for (int i = 0; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
		}
	}
	points[length] = add;

	return points;
}

POINT* mapLastMyPointsAddOne(MyPoint* mp, int length, int end, POINT add) {
	POINT* points = new POINT[length + 1];
	int count = 0;
	for (int i = end - length; i < end; i++) {
		MyPoint pt = mp[i];
		if (pt.x != ILLEGELMYPOINT && pt.y != ILLEGELMYPOINT && count < length) {
			points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
		}
	}
	points[length] = add;

	return points;
}

void drawDrawInfo(HDC hdc, DrawInfo *item) {
	switch (item->type) {
		case LINE:
		{
			// 将坐标转换为屏幕坐标
			POINT start = mapCoordinate(coordinate, item->line.start.x, item->line.start.y);
			POINT end = mapCoordinate(coordinate, item->line.end.x, item->line.end.y);
			// 需要进行坐标转换
			DrawLine(hdc, start, end, &item->proper);
			break;
		}
		case CIRCLE:
		{
			// 画圆
			MyPoint center = item->circle.center;
			double radius = item->circle.radius;
			// 将坐标转换为屏幕坐标
			POINT pt = mapCoordinate(coordinate, center.x, center.y);
			int r = (int)(radius / coordinate.radius);

			DrawCircle(hdc, pt, r, &item->proper);
			break;
		}
		case RECTANGLE:
		{
			// 画矩形
			POINT start = mapCoordinate(coordinate, item->rectangle.start.x, item->rectangle.start.y);
			POINT end = mapCoordinate(coordinate, item->rectangle.end.x, item->rectangle.end.y);

			DrawRectangle(hdc, start, end, &item->proper);
			break;
		}
		case MULTILINE:
		{
			// 画多义线
			if (item->multipoint.numPoints > 0) {
				POINT* points = mapMyPoints(item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum);
				DrawMultiLine(hdc, points, item->multipoint.numPoints, &item->proper);
				delete[] points;
			}
			break;
		}
		case FMULTILINE:
		{
			// 画封闭多义线
			if (item->multipoint.numPoints > 0) {
				POINT* points = mapMyPoints(item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum);
				DrawFMultiLine(hdc, points, item->multipoint.numPoints, &item->proper);
				delete[] points;
			}
			break;
		}
		case BCURVE:
		{
			if (item->multipoint.numPoints > 0) {
				POINT* points = mapMyPoints(item->multipoint.points, item->multipoint.numPoints, item->multipoint.endNum);
				DrawBSplineC(hdc, points, BSPLINE, item->multipoint.numPoints, &item->proper);
				delete[] points;
			}

			break;
		}
		case CURVE:
		{
			// 画曲线
			if (item->multipoint.numPoints > 0) {
				Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[item->multipoint.numPoints];
				int count = 0;
				for (int i = 0; i < item->multipoint.endNum; i++) {
					MyPoint pt = item->multipoint.points[i];
					if (pt.x == ILLEGELMYPOINT || pt.y == ILLEGELMYPOINT) continue;
					POINT p = mapCoordinate(coordinate, pt.x, pt.y);
					gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
				}
				if (count == item->multipoint.numPoints) {
					// 使用GDI+绘图
					Graphics graphics(hdc);
					int color = item->proper.color;
					// 提取 ARGB 组件
					int red = color & 0xFF;   // 提取 Red 分量
					int green = (color >> 8) & 0xFF;  // 提取 Green 分量
					int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

					Pen pen(Color(255, red, green, blue), item->proper.width);
					graphics.DrawCurve(&pen, gdiplusPoints, item->multipoint.numPoints);
				}
				delete[]gdiplusPoints;
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

void drawDrawing(HDC hdc, const DrawingInfo* drawing, const DrawUnitProperty* pro) {
	// 创建黑色画笔
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	// 创建无色画刷
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
		if (HFMyPoint(&(drawing->lastRem))) {

		}
	}
	break;
	case CIRCLE:
	{
		if (HFMyPoint(&(drawing->lastRem))) {

		}
	}
	break;
	case RECTANGLE:
	{
		if (HFMyPoint(&(drawing->lastRem))) {

		}
	}
	break;
	case CURVE:
	{
		// 画曲线
		if (drawing->info.multipoint.numPoints > 0) {
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing->info.multipoint.numPoints];
			int count = 0;
			for (int i = 0; i < drawing->info.multipoint.endNum; i++) {
				MyPoint pt = drawing->info.multipoint.points[i];
				if (pt.x == ILLEGELMYPOINT || pt.y == ILLEGELMYPOINT) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing->info.multipoint.numPoints) {
				// 使用GDI+绘图
				Graphics graphics(hdc);
				int color = pro->color;
				// 提取 ARGB 组件
				int red = color & 0xFF;   // 提取 Red 分量
				int green = (color >> 8) & 0xFF;  // 提取 Green 分量
				int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

				Pen pen(Color(255, red, green, blue), pro->width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing->info.multipoint.numPoints);
			}
			delete[]gdiplusPoints;
		}
		break;
	}
	case MULTILINE:
	{
		// 画多义线
		if (drawing->info.multipoint.numPoints > 0) {
			POINT* points = mapMyPoints(drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum);
			DrawMultiLine(hdc, points, drawing->info.multipoint.numPoints, pro);
			delete[] points;
		}
		break;
	}
	case FMULTILINE:
	{
		POINT* points = mapMyPoints(drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum);
		DrawFMultiLine(hdc, points, drawing->info.multipoint.numPoints, pro);
		delete[] points;
		break;
	}
	case BCURVE:
	{
		if (drawing->info.multipoint.numPoints > 0) {
			POINT* points = mapMyPoints(drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum);
			DrawBSplineC(hdc, points, BSPLINE, drawing->info.multipoint.numPoints, pro);
			delete[] points;
		}
		break;
	}
	default:
		break;
	}
	DeleteObject(hPen);
	DeleteObject(hNullBrush);
}

// 图形计算或者辅助线显示
void drawCoSDrawing(HDC hdc, DrawingInfo* drawing, const DrawUnitProperty* pro) {
	switch (drawing->info.type) {
	case BCURVE:
	{
		if (drawing->info.multipoint.numPoints > 0) {
			POINT* points = mapMyPoints(drawing->info.multipoint.points, drawing->info.multipoint.numPoints, drawing->info.multipoint.endNum);
			DrawBCurveHelp(hdc, points, BSPLINE, drawing->info.multipoint.numPoints, pro);
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

	// 重新填充为白色
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

// 中间窗口重绘
void RedrawCoSContent(HWND hCWnd, HDC hdc) {
	RECT rect;
	GetClientRect(hCWnd, &rect);

	// 重新填充为白色
	HBRUSH hBrush = CreateSolidBrush(CANVASCOLOR);
	FillRect(hdc, &rect, hBrush);

	drawCoSDrawing(hdc, &drawing, &customProperty);
}

void EnableMouseTracking(HWND hWnd) {
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE; // 启用鼠标离开事件
	tme.hwndTrack = hWnd;    // 需要跟踪的窗口句柄
	tme.dwHoverTime = 0;     // 设置为0以立即触发事件

	TrackMouseEvent(&tme);
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

void SetToolBarCheck(HWND toolbar, ChooseState &cs, int id) {
	// 取消之前的选择
	SendMessage(toolbar, TB_CHECKBUTTON, cs.choose, FALSE);
	// 设置新的选择
	SendMessage(toolbar, TB_CHECKBUTTON, id, TRUE);
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
		DrawXLine(hdc, points[i], points[i+1], 1);
	}
	
	// 画隔一个点的虚线
	for (int i = 0; i < n - 2; i++) {
		DrawXLine(hdc, points[i], points[i + 2], 1);
	}

	// 隔一个的线的中点与两个点之间的点相连
	POINT mid;
	POINT third;
	int r = degree;
	for (int i = 0; i < n - 2; i++) {
		mid.x = (points[i].x + points[i + 2].x) / 2;
		mid.y = (points[i].y + points[i + 2].y) / 2;
		DrawXLine(hdc, mid, points[i + 1], 1);

		// 将中点和其三分一处标红
		third.x = (points[i + 1].x * (r - 1) + mid.x) / r;
		third.y = (points[i + 1].y * (r - 1) + mid.y) / r;

		DrawPoint(hdc, mid.x, mid.y, 3, 0x000000ff);
		DrawPoint(hdc, third.x, third.y, 3, 0x000000ff);
	}

	DrawBSplineC(hdc, points, degree, n, pro);
}

void DrawBCurveHelp(HDC hdc, POINT* points, int degree, int n, const DrawUnitProperty* pro) {
	// 画虚线
	for (int i = 0; i < n - 1; i++) {
		DrawXLine(hdc, points[i], points[i + 1], 1);
	}

	// 画隔一个点的虚线
	for (int i = 0; i < n - 2; i++) {
		DrawXLine(hdc, points[i], points[i + 2], 1);
	}

	// 隔一个的线的中点与两个点之间的点相连
	POINT mid;
	POINT third;
	int r = degree;
	for (int i = 0; i < n - 2; i++) {
		mid.x = (points[i].x + points[i + 2].x) / 2;
		mid.y = (points[i].y + points[i + 2].y) / 2;
		DrawXLine(hdc, mid, points[i + 1], 1);

		// 将中点和其三分一处标红
		third.x = (points[i + 1].x * (r - 1) + mid.x) / r;
		third.y = (points[i + 1].y * (r - 1) + mid.y) / r;

		DrawPoint(hdc, mid.x, mid.y, 3, 0x000000ff);
		DrawPoint(hdc, third.x, third.y, 3, 0x000000ff);
	}
}

void DrawABCurveHelp(HDC hdc, POINT start, POINT middle, POINT end, int degree) {
	POINT seM, semM;
	seM.x = (start.x + end.x) / 2;
	seM.y = (start.y + end.y) / 2;
	semM.x = (middle.x * (degree - 1) + seM.x) / degree;
	semM.y = (middle.y * (degree - 1) + seM.y) / degree;

	DrawXLine(hdc, middle, end, 1);
	DrawXLine(hdc, start, end, 1);
	DrawXLine(hdc, seM, middle, 1);
	DrawPoint(hdc, seM.x, seM.y, 3, 0x000000ff);
	DrawPoint(hdc, semM.x, semM.y, 3, 0x000000ff);
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

void RefreshRadius(WPARAM wParam) {
	// 放大时,坐标系radius减小，缩小时，坐标系radius增大
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	// 缩放因子，可以调整为更适合的值
	double scaleFactor = RADIUSCHANGESPEED;

	// 使用指数缩放，确保 radius 始终大于0
	coordinate.radius *= exp(-scaleFactor * zDelta);

	// 防止缩放比例过小或过大
	if (coordinate.radius < MINRADIUS) {
		coordinate.radius = MINRADIUS; // 限制最小缩放比例
	}
	else if (coordinate.radius > MAXRADIUS) {
		coordinate.radius = MAXRADIUS; // 限制最大缩放比例
	}
	return;
}