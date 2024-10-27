#pragma once

#include "resource.h"
#include "drawinfo.h"
#include "drawAny.h"
#include "windowState.h"
#include <windows.h>
#include <CommCtrl.h>
#include <string>

#define MAINCOLOR  RGB(214, 222, 220)
#define SMALLCOLOR RGB(127, 127, 127)
#define CANVASCOLOR RGB(255, 255, 255)
#define STATUSBARCOLOR RGB(75, 75, 75)
#define SIDEBARCOLOR RGB(200, 200, 200)

MyDrawState mst = { CHOOSEIMG, CHOOSEIMG }; // 默认状态
ChooseState cs = { -1 };

Coordinate coordinate; // 坐标系
StoreImg allImg; // 存储所有的图形
DrawInfo drawing; // 当前正在绘制的图形

using namespace Gdiplus;

ULONG_PTR gdiplusToken;  // GDI+ 初始化令牌

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

void drawCoordinate(HDC hdc, POINT center, int width, int height) {
	// 画坐标系
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // 黑色画笔

	HPEN hWPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	HPEN hPenLight = CreatePen(PS_SOLID, 1, RGB(230, 230, 230)); // 浅色画笔

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

	int g = MINXPERZ * radius;

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
			double r = (radius / coordinate.radius);

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
			if (item->multiline.numPoints > 0) {
				POINT* points = new POINT[item->multiline.numPoints];
				int count = 0;
				for (int i = 0; i < item->multiline.endNum; i++) {
					MyPoint pt = item->multiline.points[i];
					if (pt.x != DBL_MAX && pt.y != DBL_MAX && count < item->multiline.numPoints) {
						points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
					}
				}
				DrawMultiLine(hdc, points, count, &item->proper);
				delete[] points;
			}
			break;
		}
		case CURVE:
		{
			// 画曲线
			if (item->curve.numPoints > 0) {
				Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[item->curve.numPoints];
				int count = 0;
				for (int i = 0; i < item->curve.endNum; i++) {
					MyPoint pt = item->curve.controlPoints[i];
					if (pt.x == DBL_MAX || pt.y == DBL_MAX) continue;
					POINT p = mapCoordinate(coordinate, pt.x, pt.y);
					gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
				}
				if (count == item->curve.numPoints) {
					// 使用GDI+绘图
					Graphics graphics(hdc);
					int color = item->proper.color;
					// 提取 ARGB 组件
					int red = color & 0xFF;   // 提取 Red 分量
					int green = (color >> 8) & 0xFF;  // 提取 Green 分量
					int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

					Pen pen(Color(255, red, green, blue), item->proper.width);
					graphics.DrawCurve(&pen, gdiplusPoints, item->curve.numPoints);
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

void drawDrawing(HDC hdc, DrawInfo* drawing) {
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
	switch (drawing->type) {
	case CURVE:
	{
		// 画曲线
		if (drawing->curve.numPoints > 0) {
			Gdiplus::Point* gdiplusPoints = new Gdiplus::Point[drawing->curve.numPoints];
			int count = 0;
			for (int i = 0; i < drawing->curve.endNum; i++) {
				MyPoint pt = drawing->curve.controlPoints[i];
				if (pt.x == DBL_MAX || pt.y == DBL_MAX) continue;
				POINT p = mapCoordinate(coordinate, pt.x, pt.y);
				gdiplusPoints[count++] = Gdiplus::Point(p.x, p.y);
			}
			if (count == drawing->curve.numPoints) {
				// 使用GDI+绘图
				Graphics graphics(hdc);
				int color = drawing->proper.color;
				// 提取 ARGB 组件
				int red = color & 0xFF;   // 提取 Red 分量
				int green = (color >> 8) & 0xFF;  // 提取 Green 分量
				int blue = (color >> 16) & 0xFF;          // 提取 Blue 分量

				Pen pen(Color(255, red, green, blue), drawing->proper.width);
				graphics.DrawCurve(&pen, gdiplusPoints, drawing->curve.numPoints);
			}
			delete[]gdiplusPoints;
		}
		break;
	}
	case MULTILINE:
	{
		// 画多义线
		if (drawing->multiline.numPoints > 0) {
			POINT* points = new POINT[drawing->multiline.numPoints];
			int count = 0;
			for (int i = 0; i < drawing->multiline.endNum; i++) {
				MyPoint pt = drawing->multiline.points[i];
				if (pt.x != DBL_MAX && pt.y != DBL_MAX && count < drawing->multiline.numPoints) {
					points[count++] = mapCoordinate(coordinate, pt.x, pt.y);
				}
			}
			if (count == drawing->multiline.numPoints) {
				DrawMultiLine(hdc, points, count, &drawing->proper);
			}
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
	// 加载坐标系
	drawCoordinate(hdc, coordinate.center, rect.right, rect.bottom);

	// 加载保存的图形
	drawStoreImg(hdc, &allImg);

	// 加载正在绘制的图形
	drawDrawing(hdc, &drawing);
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