#pragma once

#include "windowState.h"
#include "calculateImg.h"
#include <vector>
#include <algorithm>
#include <thread>
#include <vector>

using namespace std;

void MidpointLine(HDC hdc, int x0, int y0, int x1, int y1, int color);
void BresenhamLine(HDC hdc, int x0, int y0, int x1, int y1, int color);
void MidpointCircle(HDC hdc, int xc, int yc, int r, int color);
void BresenhamCircle(HDC hdc, int xc, int yc, int r, int color);
void FillLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int width);
void FillCircle(HDC hdc, int xc, int yc, int r, int color, int width);
void ScanlineFill(HDC hdc, POINT* polygon, int n, int color);
void FenceFill(HDC hdc, POINT* points, int n, int color);

void DrawPoint(HDC hdc, int x, int y, int size, COLORREF color) {
	HBRUSH brush = CreateSolidBrush(color);  // 创建填充颜色
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	Ellipse(hdc, x - size, y - size, x + size, y + size);  // 绘制圆
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

int DrawXLine(HDC hdc, POINT start, POINT end, const DrawUnitProperty* pro) {
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = pro->color; // 线条颜色
	DWORD dashPattern[2] = { 10, 5 }; // 10个像素实线，5个像素空白
	HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, pro->width, &lb, 2, dashPattern);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);
	MoveToEx(hdc, start.x, start.y, NULL);
	LineTo(hdc, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);

	return 0;
}

int DrawXLineWithDash(HDC hdc, POINT start, POINT end, int color, int width, const DWORD dash[2]) {
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = color; // 线条颜色
	HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, width, &lb, 2, dash);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);
	MoveToEx(hdc, start.x, start.y, NULL);
	LineTo(hdc, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);

	return 0;
}

int DrawXLine(HDC hdc, POINT start, POINT end, int color, int width) {
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = color; // 线条颜色
	DWORD dashPattern[2] = { 10, 5 }; // 10个像素实线，5个像素空白
	HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, width, &lb, 2, dashPattern);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);
	MoveToEx(hdc, start.x, start.y, NULL);
	LineTo(hdc, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);

	return 0;
}

int DrawLine(HDC hdc, POINT start, POINT end, int width, int color) {
	HPEN hPen = CreatePen(PS_SOLID, width, color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);
	MoveToEx(hdc, start.x, start.y, NULL);
	LineTo(hdc, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);

	return 0;
}

int DrawLineS(HDC hdc, POINT start, POINT end, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	MoveToEx(hdc, start.x, start.y, NULL);
	LineTo(hdc, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int DrawLine(HDC hdc, POINT start, POINT end, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	switch (DRAWTYPE(pro->type)) {
	case DRAWSYSTEM:
	{
		MoveToEx(hdc, start.x, start.y, NULL);
		LineTo(hdc, end.x, end.y);
	}
	break;
	case DRAWBRE:
	{
		if (pro->width == 1) {
			BresenhamLine(hdc, start.x, start.y, end.x, end.y, pro->color);
		}
		else {
			MoveToEx(hdc, start.x, start.y, NULL);
			LineTo(hdc, end.x, end.y);
			//FillLine(hdc, start.x, start.y, end.x, end.y, pro->color, pro->width);
		}
	}
	break;
	case DRAWMID:
	{
		if (pro->width == 1) {
			MidpointLine(hdc, start.x, start.y, end.x, end.y, pro->color);
		}
		else {
			MoveToEx(hdc, start.x, start.y, NULL);
			LineTo(hdc, end.x, end.y);
			//FillLine(hdc, start.x, start.y, end.x, end.y, pro->color, pro->width);
		}
	}
	break;
	}

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int StoreLineTo(StoreImg* sti, MyPoint start, MyPoint end, DrawUnitProperty pro) {
	DrawInfo item;
	item.type = LINE;
	item.line.start = start;
	item.line.end = end;
	item.proper = pro;
	AddDrawInfoToStoreImg(sti, item);

	return 0;
}

int DrawCircle(HDC hdc, POINT center, POINT rp, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	int r = (int)sqrt((center.x - rp.x) * (center.x - rp.x) + (center.y - rp.y) * (center.y - rp.y));
	switch (DRAWTYPE(pro->type)) {
	case DRAWSYSTEM:
	{
		Ellipse(hdc, center.x - r, center.y - r, center.x + r, center.y + r);
	}
	break;
	case DRAWBRE:
	{
		if (pro->width == 1) {
			BresenhamCircle(hdc, center.x, center.y, r, pro->color);
		}
		else {
			FillCircle(hdc, center.x, center.y, r, pro->color, pro->width);
		}
	}
	break;
	case DRAWMID:
	{
		if (pro->width == 1) {
			MidpointCircle(hdc, center.x, center.y, r, pro->color);
		}
		else {
			FillCircle(hdc, center.x, center.y, r, pro->color, pro->width);
		}
	}
	break;
	}

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int DrawCircle(HDC hdc, POINT center, int r, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	switch (DRAWTYPE(pro->type)) {
	case DRAWSYSTEM:
	{
		Ellipse(hdc, center.x - r, center.y - r, center.x + r, center.y + r);
	}
	break;
	case DRAWBRE:
	{
		if (pro->width == 1) {
			BresenhamCircle(hdc, center.x, center.y, r, pro->color);
		}
		else {
			FillCircle(hdc, center.x, center.y, r, pro->color, pro->width);
		}
	}
	break;
	case DRAWMID:
	{
		if (pro->width == 1) {
			MidpointCircle(hdc, center.x, center.y, r, pro->color);
		}
		else {
			FillCircle(hdc, center.x, center.y, r, pro->color, pro->width);
		}
	}
	break;
	}

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int StoreCircleTo(StoreImg* sti, MyPoint center, MyPoint rp, DrawUnitProperty pro) {
	double r = sqrt((center.x - rp.x) * (center.x - rp.x) + (center.y - rp.y) * (center.y - rp.y));
	DrawInfo item;
	item.type = CIRCLE;
	item.circle.center = center;
	item.circle.radius = r;
	item.proper = pro;
	AddDrawInfoToStoreImg(sti, item);

	return 0;
}

int DrawRectangle(HDC hdc, POINT start, POINT end, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	Rectangle(hdc, start.x, start.y, end.x, end.y);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

// 四个点画矩形
int DrawRectangleF(HDC hdc, POINT start, POINT end, POINT add1, POINT add2, const DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	// 画四条线
	DrawLineS(hdc, start, add1, pro);
	DrawLineS(hdc, add1, end, pro);
	DrawLineS(hdc, end, add2, pro);
	DrawLineS(hdc, add2, start, pro);

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int StoreRectangleTo(StoreImg* sti, MyPoint start, MyPoint end, DrawUnitProperty pro) {
	DrawInfo item;
	item.type = RECTANGLE;
	item.rectangle.start = start;
	item.rectangle.end = end;
	item.proper = pro;
	AddDrawInfoToStoreImg(sti, item);

	return 0;
}

int DrawMultiLine(HDC hdc, POINT* start, int length, const DrawUnitProperty* pro) {
	for (int i = 0; i < length - 1; i++) {
		DrawLine(hdc, start[i], start[i + 1], pro);
	}
	return 0;
}

// TODO: 系统颜色填充
int PadColor(HDC hdc, POINT* point, int length, int color, int type) {
	switch (PADTYPE(type)) {
	case PADSYSTEM:
		break;
	case PADSCAN:
		ScanlineFill(hdc, point, length, color);
		break;
	case PADZL:
		FenceFill(hdc, point, length, color);
		break;
	}

	return 0;
}

int DrawFMultiLine(HDC hdc, POINT* start, int length, const DrawUnitProperty* pro) {
	// 先填充颜色
	PadColor(hdc, start, length, pro->bgcolor, pro->type);
	for (int i = 0; i < length; i++) {
		DrawLine(hdc, start[i], start[(i + 1) % length], pro);
	}

	return 0;
}

void DrawMyPoint(HDC hdc, const MyPoint& mp, int size, COLORREF color) {
	POINT p = mapCoordinate(coordinate, mp.x, mp.y);
	DrawPoint(hdc, p.x, p.y, size, color);
}

void DrawLineHelp(HDC hdc, const MyLine& line, int size, COLORREF color) {
	DrawMyPoint(hdc, line.start, size, color);
	DrawMyPoint(hdc, line.end, size, color);
}

void DrawCircleHelp(HDC hdc, const MyCircle& circle, int size, COLORREF color) {
	DrawMyPoint(hdc, circle.center, size, color);
}

void DrawRectangleHelp(HDC hdc, const MyRectangle& rectangle, int size, COLORREF color) {
	DrawMyPoint(hdc, rectangle.start, size, color);
	DrawMyPoint(hdc, rectangle.end, size, color);
	DrawMyPoint(hdc, rectangle.add1, size, color);
	DrawMyPoint(hdc, rectangle.add2, size, color);

}

// 没有虚线版本
void DrawMultiPointHelpNoL(HDC hdc, const MyMultiPoint* points) {
	// 所有点
	for (int i = 0; i < points->endNum; i++) {
		MyPoint mp = points->points[i];
		if (HFMyPoint(&mp)) {
			POINT p = mapCoordinate(coordinate, mp.x, mp.y);
			DrawPoint(hdc, p.x, p.y, 3, HELPPOINTCOLOR);
		}
	}
}

// 画所有虚线
void DrawMultiPointHelpXline(HDC hdc, const MyMultiPoint* points, bool f) {
	POINT firstp;
	POINT endp;
	POINT lastp;
	int lasti = 0;
	for (int i = 0; i < points->endNum; i++) {
		lasti = i;
		if (HFMyPoint(&(points->points[i]))) break;
	}
	lastp = mapCoordinate(coordinate, points->points[lasti].x, points->points[lasti].y);
	firstp = lastp;

	for (int i = lasti + 1; i < points->endNum - 1; i++) {
		MyPoint mp = points->points[i];
		if (HFMyPoint(&mp)) {
			POINT p = mapCoordinate(coordinate, mp.x, mp.y);
			DrawXLine(hdc, lastp, p, HELPLINECORLOR, 1);
			lastp = p;
			endp = p;
		}
	}

	if (f) {
		DrawXLine(hdc, firstp, lastp, HELPLINECORLOR, 1);
	}
}

// 有虚线版本
void DrawMultiPointHelp(HDC hdc, const MyMultiPoint* points, bool f) {
	DrawMultiPointHelpNoL(hdc, points);
	// 画虚线
	DrawMultiPointHelpXline(hdc, points, f);
}

void DrawBCurveHelp(HDC hdc, POINT* points, int degree, int n) {
	// 画虚线
	for (int i = 0; i < n - 1; i++) {
		DrawXLine(hdc, points[i], points[i + 1], HELPLINECORLOR, 1);
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
}

///////////////////////////////////////////////////////////////////////////// 实验要求

void MidpointLine(HDC hdc, int x0, int y0, int x1, int y1, int color) {
	int dx = abs(x1 - x0);  // 计算横向差值
	int dy = abs(y1 - y0);  // 计算纵向差值
	int sx = (x0 < x1) ? 1 : -1;  // 设置x的步长
	int sy = (y0 < y1) ? 1 : -1;  // 设置y的步长
	int d = dy - (dx / 2);  // 初始误差

	// 根据斜率判断是否水平或垂直的特殊情况
	if (dx > dy) { // 斜率小于1
		while (x0 != x1) {
			int e2 = 2 * d;  // 计算误差的临时变量

			if (e2 > -dy) { // 误差超过x的范围时，移动x
				d -= dy;
				x0 += sx;
			}

			if (e2 < dx) { // 误差超过y的范围时，移动y
				d += dx;
				y0 += sy;
			}

			// 绘制当前像素
			SetPixel(hdc, x0, y0, color);
		}
	}
	else { // 斜率大于等于1
		while (y0 != y1) {
			int e2 = 2 * d;  // 计算误差的临时变量

			if (e2 > -dx) { // 误差超过y的范围时，移动y
				d -= dx;
				y0 += sy;
			}

			if (e2 < dy) { // 误差超过x的范围时，移动x
				d += dy;
				x0 += sx;
			}

			// 绘制当前像素
			SetPixel(hdc, x0, y0, color);
		}
	}
}

// TODO: 改正逻辑
void FillLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int width) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int d = dy - (dx / 2);
	int x = x0, y = y0;

	int offsetX = width / 2;
	int offsetY = width / 2;

	// 确定直线的四个边界点
	POINT polygon[4] = {
		{ x0 + (int)offsetX, y0 + (int)offsetY },
		{ x0 - (int)offsetX, y0 - (int)offsetY },
		{ x1 - (int)offsetX, y1 - (int)offsetY },
		{ x1 + (int)offsetX, y1 + (int)offsetY }
	};

	// 使用扫描线填充算法对带状区域进行填充
	ScanlineFill(hdc, polygon, 4, color);
}

void BresenhamLine(HDC hdc, int x0, int y0, int x1, int y1, int color) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int stepX = dx >= 0 ? 1 : -1;
	int stepY = dy >= 0 ? 1 : -1;
	dx = abs(dx);
	dy = abs(dy);

	if (dx > dy) { // |m| < 1
		int p = 2 * dy - dx;
		int y = y0;
		for (int x = x0; x != x1; x += stepX) {
			SetPixel(hdc, x, y, color);
			if (p > 0) {
				y += stepY;
				p -= 2 * dx;
			}
			p += 2 * dy;
		}
		SetPixel(hdc, x1, y1, color); // 绘制终点
	}
	else { // |m| >= 1
		int p = 2 * dx - dy;
		int x = x0;
		for (int y = y0; y != y1; y += stepY) {
			SetPixel(hdc, x, y, color);
			if (p > 0) {
				x += stepX;
				p -= 2 * dy;
			}
			p += 2 * dx;
		}
		SetPixel(hdc, x1, y1, color); // 绘制终点
	}
}


void MidpointCircle(HDC hdc, int xc, int yc, int r, int color) {
	int x = 0;
	int y = r;
	int d = 1 - r;

	auto drawThickCirclePoints = [&](int x, int y) {
		SetPixel(hdc, xc + x, yc + y, color);
		SetPixel(hdc, xc - x, yc + y, color);
		SetPixel(hdc, xc + x, yc - y, color);
		SetPixel(hdc, xc - x, yc - y, color);
		SetPixel(hdc, xc + y, yc + x, color);
		SetPixel(hdc, xc - y, yc + x, color);
		SetPixel(hdc, xc + y, yc - x, color);
		SetPixel(hdc, xc - y, yc - x, color);
		};

	drawThickCirclePoints(x, y);

	while (x < y) {
		if (d < 0) {
			d += 2 * x + 3;
		}
		else {
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		drawThickCirclePoints(x, y);
	}
}

vector<POINT> MidpointCircle(int xc, int yc, int r) {
	vector<POINT> points;
	int x = 0;
	int y = r;
	int d = 1 - r;

	auto pushCirclePoints = [&](int x, int y) {
			points.push_back({ xc + x, yc + y });
			points.push_back({ xc - x, yc + y });
			points.push_back({ xc + x, yc - y });
			points.push_back({ xc - x, yc - y });
			points.push_back({ xc + y, yc + x });
			points.push_back({ xc - y, yc + x });
			points.push_back({ xc + y, yc - x });
			points.push_back({ xc - y, yc - x });
		};

	pushCirclePoints(x, y);

	while (x < y) {
		if (d < 0) {
			d += 2 * x + 3;
		}
		else {
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		pushCirclePoints(x, y);
	}

	return points;
}

void FillCircle(HDC hdc, int xc, int yc, int r, int color, int width) {
	// 外圆和内圆的半径
	int r_outer = r + ((width + 1) / 2);
	int r_inner = r - (width / 2);

	// 使用扫描线填充法绘制圆环区域
	for (int y = 0; y < r_outer; y++) {
		// 计算外圆和内圆在当前 y 位置上的 x 范围
		int x_outer = (int)sqrt(r_outer * r_outer - y * y);
		int x_inner = (r_inner > y) ? (int)sqrt(r_inner * r_inner - y * y) : 0;

		// 填充 [x_inner, x_outer] 之间的像素
		for (int x = x_inner; x < x_outer; x++) {
			// 绘制左右两侧的像素点
			SetPixel(hdc, xc + x, yc + y, color);
			SetPixel(hdc, xc - x, yc + y, color);

			// 下方左右两次像素
			if (y != 0) {
				SetPixel(hdc, xc + x, yc - y, color);
				SetPixel(hdc, xc - x, yc - y, color);
			}
		}
	}
}

void BresenhamCircle(HDC hdc, int xc, int yc, int r, int color) {
	int x = 0, y = r;
	int d = 3 - 2 * r;

	auto drawThickCirclePoints = [&](int x, int y) {
		SetPixel(hdc, xc + x, yc + y, color);
		SetPixel(hdc, xc - x, yc + y, color);
		SetPixel(hdc, xc + x, yc - y, color);
		SetPixel(hdc, xc - x, yc - y, color);
		SetPixel(hdc, xc + y, yc + x, color);
		SetPixel(hdc, xc - y, yc + x, color);
		SetPixel(hdc, xc + y, yc - x, color);
		SetPixel(hdc, xc - y, yc - x, color);
		};
	
	drawThickCirclePoints(x, y);

	while (x <= y) {
		x++;
		if (d > 0) {
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else {
			d = d + 4 * x + 6;
		}
		drawThickCirclePoints(x, y);
	}
}

// 绘制像素点的函数
void SetPixelPoint(HDC hdc, int x, int y, int color) {
	SetPixel(hdc, x, y, color);
}

// 扫描线填充函数
void ScanlineFill(HDC hdc, POINT* polygon, int n, int color) {
	// 找到多边形的最小和最大 Y 值
	int minY = polygon[0].y, maxY = polygon[0].y;
	for (int i = 1; i < n; i++) {
		if (polygon[i].y < minY) minY = polygon[i].y;
		if (polygon[i].y > maxY) maxY = polygon[i].y;
	}

	// 对每条扫描线进行填充
	for (int y = minY; y <= maxY; y++) {
		vector<int> intersections;

		// 查找扫描线与每条边的交点
		for (int i = 0; i < n; i++) {
			POINT p1 = polygon[i];
			POINT p2 = polygon[(i + 1) % n];

			// 确保 p1.y <= p2.y
			if (p1.y > p2.y) swap(p1, p2);

			// 检查扫描线是否与边相交
			if (y >= p1.y && y < p2.y) {
				int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
				intersections.push_back(x);
			}
		}

		// 对交点进行排序
		sort(intersections.begin(), intersections.end());

		// 成对填充交点之间的区域
		for (size_t i = 0; i < intersections.size(); i += 2) {
			for (int x = intersections[i]; x < intersections[i + 1]; x++) {
				SetPixelPoint(hdc, x, y, color);
			}
		}
	}
}

// 用于存储边界交点的结构体
struct Edge {
	int yMin;       // 边的最小 y 坐标
	int yMax;       // 边的最大 y 坐标
	double x;       // 边的初始 x 坐标
	double dx;      // 1 像素步长对应的 x 增量
};

// 计算多边形的所有边，并将其存入边表
void BuildEdgeTable(POINT* points, int n, vector<Edge>& edgeTable) {
	for (int i = 0; i < n; ++i) {
		POINT p1 = points[i];
		POINT p2 = points[(i + 1) % n]; // 下一个顶点（环形）

		// 确保 p1.y <= p2.y
		if (p1.y > p2.y) swap(p1, p2);

		// 跳过水平边
		if (p1.y == p2.y) continue;

		// 创建边，并计算 dx
		Edge edge;
		edge.yMin = p1.y;
		edge.yMax = p2.y;
		edge.x = p1.x;
		edge.dx = (double)(p2.x - p1.x) / (p2.y - p1.y);
		edgeTable.push_back(edge);
	}
}

// 栅栏填充算法
void FenceFill(HDC hdc, POINT* points, int n, int color) {
	vector<Edge> edgeTable;
	BuildEdgeTable(points, n, edgeTable);

	// 找到最小和最大的 y 坐标
	int yMin = INT_MAX, yMax = INT_MIN;
	for (int i = 0; i < n; ++i) {
		yMin = min(yMin, points[i].y);
		yMax = max(yMax, points[i].y);
	}

	// 活动边表
	vector<Edge> activeEdgeTable;

	// 扫描线填充
	for (int y = yMin; y <= yMax; ++y) {
		// 更新活动边表，移除 yMax == y 的边
		activeEdgeTable.erase(
			remove_if(activeEdgeTable.begin(), activeEdgeTable.end(),
				[y](Edge& edge) { return edge.yMax == y; }),
			activeEdgeTable.end());

		// 添加新边到活动边表
		for (auto& edge : edgeTable) {
			if (edge.yMin == y) {
				activeEdgeTable.push_back(edge);
			}
		}

		// 更新 x 坐标
		for (auto& edge : activeEdgeTable) {
			edge.x += edge.dx;
		}

		// 按 x 坐标排序
		sort(activeEdgeTable.begin(), activeEdgeTable.end(),
			[](Edge& e1, Edge& e2) { return e1.x < e2.x; });

		// 成对填充像素
		for (size_t i = 0; i < activeEdgeTable.size(); i += 2) {
			if (i + 1 >= activeEdgeTable.size()) break;
			int xStart = (int)ceil(activeEdgeTable[i].x);
			int xEnd = (int)floor(activeEdgeTable[i + 1].x);

			// 填充当前扫描线上的像素
			for (int x = xStart; x <= xEnd; ++x) {
				SetPixelPoint(hdc, x, y, color);
			}
		}
	}
}

// 任意阶均匀B样条计算
void DrawBCurveDeBoor(HDC hdc, POINT* controlPoints, int degree, int n, const DrawUnitProperty* pro) {
	// 将指针转化为 vector
	std::vector<POINT> points(controlPoints, controlPoints + n);

	// 计算 B 样条曲线上的点, 默认 100 个点
	std::vector<POINT> curvePoints = CalcDeBoor(points, degree);

	if (curvePoints.size() == 0) return;

	if (curvePoints.size() == 1) {
		DrawPoint(hdc, curvePoints[0].x, curvePoints[0].y, 3, pro->color);
		return;
	}

	// 绘制 B 样条曲线
	for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
		DrawLine(hdc, curvePoints[i], curvePoints[i + 1], pro);
	}
}

#define CSDRAWRECTCOLOR		RGB(64, 64, 64)

// 绘制选择矩形
void drawCSDrawRectP(HDC hdc, POINT start, POINT end) {
	DWORD dash[2] = { 10, 10 };
	// 四边偏白色虚线
	DrawXLineWithDash(hdc, start, { end.x, start.y }, CSDRAWRECTCOLOR, 2, dash);
	DrawXLineWithDash(hdc, { end.x, start.y }, end, CSDRAWRECTCOLOR, 2, dash);
	DrawXLineWithDash(hdc, end, { start.x, end.y }, CSDRAWRECTCOLOR, 2, dash);
	DrawXLineWithDash(hdc, { start.x, end.y }, start, CSDRAWRECTCOLOR, 2, dash);
}