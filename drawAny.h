#pragma once

#include "drawinfo.h"

void MidpointLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int lineWidth);
void BresenhamLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int lineWidth);
void MidpointCircle(HDC hdc, int xc, int yc, int r, DrawUnitProperty *pro);
void BresenhamCircle(HDC hdc, int xc, int yc, int r, DrawUnitProperty *pro);

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
		BresenhamLine(hdc, start.x, start.y, end.x, end.y, pro->color, pro->width);
	}
	break;
	case DRAWMID:
	{
		MidpointLine(hdc, start.x, start.y, end.x, end.y, pro->color, pro->width);
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

int DrawCircle(HDC hdc, POINT center, POINT rp, DrawUnitProperty* pro) {
	HPEN hPen = CreatePen(PS_SOLID, pro->width, pro->color);
	SelectObject(hdc, hPen);
	// 创建无色画刷
	LOGBRUSH lbb;
	lbb.lbStyle = BS_NULL;
	lbb.lbColor = RGB(0, 0, 0);
	lbb.lbHatch = 0;
	HBRUSH hNullBrush = CreateBrushIndirect(&lbb);
	SelectObject(hdc, hNullBrush);

	double r = sqrt((center.x - rp.x) * (center.x - rp.x) + (center.y - rp.y) * (center.y - rp.y));
	switch (DRAWTYPE(pro->type)) {
	case DRAWSYSTEM:
	{
		Ellipse(hdc, center.x - r, center.y - r, center.x + r, center.y + r);
	}
	break;
	case DRAWBRE:
	{
		BresenhamCircle(hdc, center.x, center.y, r, pro);
	}
	break;
	case DRAWMID:
	{
		MidpointCircle(hdc, center.x, center.y, r, pro);
	}
	break;
	}

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int DrawCircle(HDC hdc, POINT center, double r, DrawUnitProperty* pro) {
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
		BresenhamCircle(hdc, center.x, center.y, r, pro);
	}
	break;
	case DRAWMID:
	{
		MidpointCircle(hdc, center.x, center.y, r, pro);
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

int DrawRectangle(HDC hdc, POINT start, POINT end, DrawUnitProperty* pro) {
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

int StoreRectangleTo(StoreImg* sti, MyPoint start, MyPoint end, DrawUnitProperty pro) {
	DrawInfo item;
	item.type = RECTANGLE;
	item.rectangle.start = start;
	item.rectangle.end = end;
	item.proper = pro;
	AddDrawInfoToStoreImg(sti, item);

	return 0;
}

int DrawCurve(HDC hdc, POINT* start, int length, DrawUnitProperty* pro) {
	return 0;
}

int DrawMultiLine(HDC hdc, POINT* start, int length, DrawUnitProperty* pro) {
	for (int i = 0; i < length - 1; i++) {
		DrawLine(hdc, start[i], start[i + 1], pro);
	}
	return 0;
}

int DrawFMultiLine(HDC hdc, POINT* start, int length, DrawUnitProperty* pro) {
	for (int i = 0; i < length; i++) {
		DrawLine(hdc, start[i], start[(i + 1) % length], pro);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////// 实验要求

void MidpointLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int lineWidth) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int d = dy - (dx / 2);
	int x = x0, y = y0;

	auto drawThickPixel = [&](int x, int y) {
		for (int i = -lineWidth / 2; i <= lineWidth / 2; i++) {
			for (int j = -lineWidth / 2; j <= lineWidth / 2; j++) {
				SetPixel(hdc, x + i, y + j, color);
			}
		}
		};

	drawThickPixel(x, y);

	while (x < x1) {
		x++;
		if (d < 0) {
			d = d + dy;
		}
		else {
			d = d + (dy - dx);
			y++;
		}
		drawThickPixel(x, y);
	}
}

void BresenhamLine(HDC hdc, int x0, int y0, int x1, int y1, int color, int lineWidth) {
	int dx = abs(x1 - x0), dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	auto drawThickPixel = [&](int x, int y) {
		for (int i = -lineWidth / 2; i <= lineWidth / 2; i++) {
			for (int j = -lineWidth / 2; j <= lineWidth / 2; j++) {
				SetPixel(hdc, x + i, y + j, color);
			}
		}
		};

	while (true) {
		drawThickPixel(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void MidpointCircle(HDC hdc, int xc, int yc, int r, DrawUnitProperty *pro) {
	int x = 0;
	int y = r;
	int d = 1 - r;
	int color = pro->color;
	int lineWidth = pro->width;

	auto drawThickCirclePoints = [&](int x, int y) {
		for (int i = -lineWidth / 2; i <= lineWidth / 2; i++) {
			for (int j = -lineWidth / 2; j <= lineWidth / 2; j++) {
				SetPixel(hdc, xc + x + i, yc + y + j, color);
				SetPixel(hdc, xc - x + i, yc + y + j, color);
				SetPixel(hdc, xc + x + i, yc - y + j, color);
				SetPixel(hdc, xc - x + i, yc - y + j, color);
				SetPixel(hdc, xc + y + i, yc + x + j, color);
				SetPixel(hdc, xc - y + i, yc + x + j, color);
				SetPixel(hdc, xc + y + i, yc - x + j, color);
				SetPixel(hdc, xc - y + i, yc - x + j, color);
			}
		}
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

void BresenhamCircle(HDC hdc, int xc, int yc, int r, DrawUnitProperty *pro) {
	int x = 0, y = r;
	int d = 3 - 2 * r;
	int lineWidth = pro->width;
	int color = pro->color;

	auto drawThickCirclePoints = [&](int x, int y) {
		for (int i = -lineWidth / 2; i <= lineWidth / 2; i++) {
			for (int j = -lineWidth / 2; j <= lineWidth / 2; j++) {
				SetPixel(hdc, xc + x + i, yc + y + j, color);
				SetPixel(hdc, xc - x + i, yc + y + j, color);
				SetPixel(hdc, xc + x + i, yc - y + j, color);
				SetPixel(hdc, xc - x + i, yc - y + j, color);
				SetPixel(hdc, xc + y + i, yc + x + j, color);
				SetPixel(hdc, xc - y + i, yc + x + j, color);
				SetPixel(hdc, xc + y + i, yc - x + j, color);
				SetPixel(hdc, xc - y + i, yc - x + j, color);
			}
		}
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
