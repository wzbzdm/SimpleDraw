#pragma once

#include "drawinfo.h"

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

	if (pro->type == SYSTEM) {
		MoveToEx(hdc, start.x, start.y, NULL);
		LineTo(hdc, end.x, end.y);
	}
	

	DeleteObject(hPen);
	DeleteObject(hNullBrush);
	return 0;
}

int StoreLineTo(StoreImg *sti, MyPoint start, MyPoint end, DrawUnitProperty pro) {
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

	if (pro->type == SYSTEM) {
		Ellipse(hdc, center.x - r, center.y - r, center.x + r, center.y + r);
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

	if (pro->type == SYSTEM) {
		Ellipse(hdc, center.x - r, center.y - r, center.x + r, center.y + r);
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

	if (pro->type == SYSTEM) {
		Rectangle(hdc, start.x, start.y, end.x, end.y);
	}

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

int DrawCurve(HDC hdc, POINT *start, int length, DrawUnitProperty* pro) {
	return 0;
}

int DrawMultiLine(HDC hdc, POINT *start, int length, DrawUnitProperty* pro) {
	if (pro->type == SYSTEM) {
		for (int i = 0; i < length - 1; i++) {
			DrawLine(hdc, start[i], start[i + 1], pro);
		}
	}
	return 0;
}
