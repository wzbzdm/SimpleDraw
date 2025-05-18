#pragma once

#include "windowState.h"
#include <iostream>
#include <vector>

#define MAXWTOCHHOSE 10
#define MAXWCONTINUECHOOSE 25

std::vector<MyPoint> CalculatePoints; // 用于存储点

// 判断点是否在线段上
bool isPointOnLineSegment(MyPoint p, MyLine line) {
    double minX = min(line.start.x, line.end.x);
    double maxX = max(line.start.x, line.end.x);
    double minY = min(line.start.y, line.end.y);
    double maxY = max(line.start.y, line.end.y);

    // 考虑浮点数精度，设置一个小的容差
    double epsilon = 1e-6;

    return (p.x >= minX - epsilon && p.x <= maxX + epsilon &&
        p.y >= minY - epsilon && p.y <= maxY + epsilon);
}

// 计算两线段的交点
bool lineSegmentIntersection(const MyLine& line1, const MyLine& line2, MyPoint& intersection) {
    double x1 = line1.start.x, y1 = line1.start.y;
    double x2 = line1.end.x, y2 = line1.end.y;
    double x3 = line2.start.x, y3 = line2.start.y;
    double x4 = line2.end.x, y4 = line2.end.y;

    double denominator = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);

    // 平行或重合
    if (fabs(denominator) < 1e-6) {
        return false;
    }

    double ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / denominator;
    double ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / denominator;

    // 检查交点是否在两条线段上
    if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
        intersection.x = x1 + ua * (x2 - x1);
        intersection.y = y1 + ua * (y2 - y1);
        return true;
    }

    return false;
}

// 计算线段和圆的交点
std::vector<MyPoint> lineSegmentCircleIntersection(const MyLine& line, const MyCircle& circle) {
    std::vector<MyPoint> intersections;

    // 线段的参数方程：P = line.start + t*(line.end - line.start), t ∈ [0,1]
    double dx = line.end.x - line.start.x;
    double dy = line.end.y - line.start.y;

    double fx = line.start.x - circle.center.x;
    double fy = line.start.y - circle.center.y;

    double a = dx * dx + dy * dy;
    double b = 2 * (fx * dx + fy * dy);
    double c = fx * fx + fy * fy - circle.radius * circle.radius;

    double discriminant = b * b - 4 * a * c;

    // 没有交点
    if (discriminant < 0) {
        return intersections;
    }

    discriminant = sqrt(discriminant);

    double t1 = (-b - discriminant) / (2 * a);
    double t2 = (-b + discriminant) / (2 * a);

    // 检查 t 是否在 [0,1] 范围内
    if (t1 >= 0 && t1 <= 1) {
        MyPoint p;
        p.x = line.start.x + t1 * dx;
        p.y = line.start.y + t1 * dy;
        intersections.push_back(p);
    }

    if (t2 >= 0 && t2 <= 1 && discriminant > 0) {
        MyPoint p;
        p.x = line.start.x + t2 * dx;
        p.y = line.start.y + t2 * dy;
        intersections.push_back(p);
    }

    return intersections;
}

// 计算两个圆的交点
std::vector<MyPoint> circleCircleIntersection(const MyCircle& circle1, const MyCircle& circle2) {
    std::vector<MyPoint> intersections;

    double dx = circle2.center.x - circle1.center.x;
    double dy = circle2.center.y - circle1.center.y;
    double d = sqrt(dx * dx + dy * dy);

    // 无交点或包含关系
    if (d > circle1.radius + circle2.radius || d < fabs(circle1.radius - circle2.radius)) {
        return intersections;
    }

    double a = (circle1.radius * circle1.radius - circle2.radius * circle2.radius + d * d) / (2 * d);
    double h = sqrt(circle1.radius * circle1.radius - a * a);

    double x2 = circle1.center.x + a * (dx) / d;
    double y2 = circle1.center.y + a * (dy) / d;

    double rx = -dy * (h / d);
    double ry = dx * (h / d);

    MyPoint p1 = { x2 + rx, y2 + ry };
    MyPoint p2 = { x2 - rx, y2 - ry };

    intersections.push_back(p1);

    // 如果 h == 0，两个圆相切，只有一个交点
    if (h > 1e-6) {
        intersections.push_back(p2);
    }

    return intersections;
}

MyPoint CalPerpendicular(MyPoint start, MyPoint end, MyPoint mp) {
    MyPoint res;

    // 计算线段的斜率 k
    double dx = end.x - start.x;
    double dy = end.y - start.y;

    // 处理特殊情况：如果线段是竖直的
    if (dx == 0) {
        // 线段垂直，直接返回与 mp 点相同的 x
        res.x = start.x; // 或者 end.x
        res.y = mp.y;    // y 坐标与 mp 点相同
        return res;
    }

    // 计算斜率 k
    double k = dy / dx;

    // 计算与 mp 点垂直的交点
    double x = (k * k * start.x + k * (mp.y - start.y) + mp.x) / (k * k + 1);
    double y = k * (x - start.x) + start.y;

    res.x = x;
    res.y = y;

    return res;
}

// 图形计算逻辑, 交点
void CalculateImg(StoreImg& allimg, CSDrawInfo& csdraw) {
    if (csdraw.index < 0 || csdraw.index >= allImg.endNum) return;
    CalculatePoints.clear();
    DrawInfo choose = csdraw.choose;
    switch (choose.type) {
    case CIRCLE:
        CalculatePoints.push_back(choose.circle.center);
        break;
    }
    for (int i = 0; i < allimg.endNum; i++) {
        if (csdraw.index == i) continue;
        DrawInfo item = allimg.img[i];
        if (item.type == LINE && choose.type == LINE) {
            MyPoint mp;
            if (lineSegmentIntersection(choose.line, item.line, mp)) {
                CalculatePoints.push_back(mp);
            }
        }
        else if (item.type == LINE && choose.type == CIRCLE) {
            std::vector<MyPoint> mps;
            mps = lineSegmentCircleIntersection(item.line, choose.circle);
            for (int j = 0; j < mps.size(); j++) {
                CalculatePoints.push_back(mps[j]);
            }
        }
        else if (item.type == CIRCLE && choose.type == LINE) {
            std::vector<MyPoint> mps;
            mps = lineSegmentCircleIntersection(choose.line, item.circle);
            for (int j = 0; j < mps.size(); j++) {
                CalculatePoints.push_back(mps[j]);
            }
        }
        else if (item.type == CIRCLE && choose.type == CIRCLE) {
            std::vector<MyPoint> mps;
            mps = circleCircleIntersection(choose.circle, item.circle);
            for (int j = 0; j < mps.size(); j++) {
                CalculatePoints.push_back(mps[j]);
            }
        }
        else {

        }
    }
    return;
}

// 在图形内，或者图形外一定长度，继续选中
// 图形是否继续选中
bool ContinueChooseDrawInfo(DrawInfo& choose, Coordinate coor, POINT p) {
    MyPoint mp;
    PointToCoordinate(coor, p, mp.x, mp.y);
    double d = ILLEGELMYPOINT;
    switch (choose.type) {
    case LINE:
    {
        d = DistanceToLine(mp, choose.line);
        break;
    }
    case CIRCLE:
    {
        d = GetDPointToCircle(mp, choose.circle);
        break;
    }
    case RECTANGLE:
    {
        d = GetMinDPointToRectangle(mp, choose.rectangle);
        break;
    }
    case CURVE:
    case BCURVE:
    case MULTILINE:
    {
        d = GetMinDPointToMultipoint(mp, &(choose.multipoint));
        break;
    }
    case FMULTILINE:
    {
        d = GetMinDPointToFMultipoint(mp, &(choose.multipoint));
        break;
    }
    default:
        break;
    }

    double dpx = d / coor.radius;
    if (dpx > MAXWCONTINUECHOOSE) {
        // 超出阈值，取消选中
        return false;
    }

    return true;
}

void ZoomMyPoint(MyPoint &p, const MyPoint &center, double scale) {
	p.x = center.x + (p.x - center.x) * scale;
	p.y = center.y + (p.y - center.y) * scale;
}

void ZoomPoint(POINT& p, const POINT& center, double scale) {
    p.x = center.x + (p.x - center.x) * scale;
    p.y = center.y + (p.y - center.y) * scale;
}

// TODO: 缩放图元, 根据缩放比例，缩放所有坐标
void ZoomDrawInfo(DrawInfo &info, const MyPoint &center, double scale) {
	if (scale == 1.0) return;
    if (scale == 0) return;
    // 根据中心点和相应比例改变图形坐标
    switch (info.type) {
    case LINE:
    {
		ZoomMyPoint(info.line.start, center, scale);
		ZoomMyPoint(info.line.end, center, scale);
    }
    break;
    case CIRCLE:
    {
		ZoomMyPoint(info.circle.center, center, scale);
		info.circle.radius *= scale;
    }
    break;
    case RECTANGLE:
    {
		ZoomMyPoint(info.rectangle.start, center, scale);
		ZoomMyPoint(info.rectangle.end, center, scale);
        ZoomMyPoint(info.rectangle.add1, center, scale);
        ZoomMyPoint(info.rectangle.add2, center, scale);
    }
    break;
	case CURVE:
	case BCURVE:
	case MULTILINE:
    case FMULTILINE:
    {
		for (int i = 0; i < info.multipoint.endNum; i++) {
            if (HFMyPoint(&(info.multipoint.points[i]))) {
                ZoomMyPoint(info.multipoint.points[i], center, scale);
            }
		}
    }
    break;
    }
}

void ZoomCoordinate(Coordinate& coor, const POINT& pt, double scale) {
    ZoomPoint(coor.center, pt, scale);
}

void RotateMyPoint(MyPoint& p, const MyPoint center, double angle) {
	double x = p.x - center.x;
	double y = p.y - center.y;
	double x1 = x * cos(angle) - y * sin(angle);
	double y1 = x * sin(angle) + y * cos(angle);
	p.x = x1 + center.x;
	p.y = y1 + center.y;
}

// 图像旋转
void RotateDrawInfo(DrawInfo& info, const MyPoint& center, double angle) {
	if (angle == 0) return;
	// 根据中心点和相应角度改变图形坐标
	switch (info.type) {
	case LINE:
	{
		RotateMyPoint(info.line.start, center, angle);
		RotateMyPoint(info.line.end, center, angle);
	}
	break;
	case CIRCLE:
	{
		RotateMyPoint(info.circle.center, center, angle);
	}
	break;
	case RECTANGLE:
	{
		RotateMyPoint(info.rectangle.start, center, angle);
		RotateMyPoint(info.rectangle.end, center, angle);
        RotateMyPoint(info.rectangle.add1, center, angle);
        RotateMyPoint(info.rectangle.add2, center, angle);
	}
	break;
	case CURVE:
	case BCURVE:
	case MULTILINE:
	case FMULTILINE:
	{
		for (int i = 0; i < info.multipoint.endNum; i++) {
			if (HFMyPoint(&(info.multipoint.points[i]))) {
				RotateMyPoint(info.multipoint.points[i], center, angle);
			}
		}
	}
	break;
	}
}

double GetDrawInfoDistance(DrawInfo& item, const MyPoint& mp) {
    double d = DBL_MAX;
    switch (item.type) {
    case LINE:
    {
        d = DistanceToLine(mp, item.line);
        break;
    }
    case CIRCLE:
    {
        d = GetDPointToCircle(mp, item.circle);
        break;
    }
    case RECTANGLE:
    {
        d = GetMinDPointToRectangle(mp, item.rectangle);
        break;
    }
    case CURVE:
    case BCURVE:
    case MULTILINE:
    {
        d = GetMinDPointToMultipoint(mp, &(item.multipoint));
        break;
    }
    case FMULTILINE:
    {
        d = GetMinDPointToFMultipoint(mp, &(item.multipoint));
        break;
    }
    default:
        break;
    }

    return d;
}

// 图形选择逻辑
int ChooseImg(const StoreImg& store, const Coordinate& coor, POINT p) {
    int count = -1;
    double minDistance = MAXWTOCHHOSE * coor.radius;
    MyPoint mp;
    PointToCoordinate(coor, p, mp.x, mp.y);
    for (int i = 0; i < store.endNum; i++) {
        DrawInfo item = store.img[i];
        double d = GetDrawInfoDistance(item, mp);

        if (minDistance > d) {
            minDistance = d;
            count = i;
        }
    }

    return count;
}

// 判断点是否在选中区域内
/*
线，圆，矩形的选中区域为矩形
多义线等选中区域为每条线周围所有矩形区域的和
*/
// TODO: 
bool MyPointInCSDrawInfo(const DrawInfoRect& rect, const MyPoint& mp) {
    return mp.x < rect.maxX && mp.x > rect.minX && mp.y < rect.maxY && mp.y > rect.minY;
}

bool ChooseCSdraw(const CSDrawInfo& csdraw, const Coordinate& coor, POINT p) {
    if (csdraw.index == -1) return false;
    MyPoint mp;
    PointToCoordinate(coor, p, mp.x, mp.y);
    return MyPointInCSDrawInfo(csdraw.rect, mp);
}

// 使当前坐标系适应画布上的图像，方便观察
void FitCoordinate(Coordinate& coor, StoreImg& img, RECT canvasRect) {
    // 计算图像的最大最小坐标
    DrawInfoRect rect = INITDRAWINFORECT;
    // 若没有图像
    if (img.endNum == 0) {
        POINT center = { (canvasRect.right - canvasRect.left) / 2, (canvasRect.bottom - canvasRect.top) / 2 };
        SetCoordinate(coordinate, center, DEFAULTRADIUS); // 设置坐标系参数
        return;
    }
    for (int i = 0; i < img.endNum; i++) {
        DrawInfo item = img.img[i];
        DrawInfoRect nowrect = INITDRAWINFORECT;
        GetDrawInfoRect(&item, &nowrect);
		LargestRect(&rect, &nowrect);
    }

    // 处理无效坐标情况
    if (rect.minX == DBL_MAX || rect.minY == DBL_MAX || rect.maxX == -DBL_MAX || rect.maxY == -DBL_MAX) {
        // 没有有效的图形，设置默认坐标
        POINT center = { (canvasRect.right - canvasRect.left) / 2, (canvasRect.bottom - canvasRect.top) / 2 };
        SetCoordinate(coor, center, DEFAULTRADIUS); // 设置坐标系参数
        return;
    }

    // 计算坐标系的中心点
    double centerX = (rect.minX + rect.maxX) / 2;
    double centerY = (rect.minY + rect.maxY) / 2;

    int canvasWidth = canvasRect.right - canvasRect.left;
    int canvasHeight = canvasRect.bottom - canvasRect.top;

    // 计算坐标系的半径
    double radiusX = rect.maxX - centerX;
    double radiusY = rect.maxY - centerY;

    double radius1 = radiusX / (canvasWidth / 2.0);
    double radius2 = radiusY / (canvasHeight / 2.0);

    // 选择较大的半径作为坐标系的半径
    double radius = radius1 > radius2 ? radius1 : radius2;

    // 先确定缩放比例
    coor.radius = radius * FITRADIUS;

    // 计算该点在画布的位置
    POINT pt = mapCoordinate(coor, centerX, centerY);

    // 计算移动到画布中点的距离
    int x = canvasWidth / 2 - pt.x;
    int y = canvasHeight / 2 - pt.y;

    // 将坐标系中心点移动
    coor.center.x += x;
    coor.center.y += y;

    // 当前应该在中点
    pt = mapCoordinate(coor, centerX, centerY);
}

// DeBoor 递归计算
double DeBoor(int i, int k, double u, std::vector<double>& ui) {
    if (k == 0) {
        return (u >= ui[i] && u < ui[i + 1]) ? 1.0 : 0.0;
    }

    double ai = (ui[i + k] != ui[i]) ? (u - ui[i]) / (ui[i + k] - ui[i]) : 0;
    double bi = (ui[i + k + 1] != ui[i + 1]) ? (ui[i + k + 1] - u) / (ui[i + k + 1] - ui[i + 1]) : 0;
    double uik1 = DeBoor(i, k - 1, u, ui);
    double uik2 = DeBoor(i + 1, k - 1, u, ui);

    return ai * uik1 + bi * uik2;
}

#define BCURVECALCPOINT     100

// 均匀B样条计算
std::vector<POINT> CalcDeBoor(std::vector<POINT> points, int degree, int n) {
    // 计算曲线上的点
    std::vector<POINT> res(n);

    if (points.size() < degree)
        return res;

	// 计算 ui, 范围为 [0, 1]
    int sizep = points.size();
    double num = sizep + degree;
	std::vector<double> ui(num + 1);

    // 初始化 ui
	for (int i = 0; i <= num ; i++) {
		ui[i] = i / num;
	}

    for (int i = 0; i < n; i++) {
        double u = (double)i / (n - 1) * (ui[sizep] - ui[degree]) + ui[degree]; // 映射 u 的范围v
        POINT p = { 0, 0 };

        for (int j = 0; j < sizep; j++) {
            double nij = DeBoor(j, degree, u, ui);
            p.x += nij * points[j].x;
            p.y += nij * points[j].y;
        }

        res[i] = p;
    }

	return res;
}

std::vector<POINT> CalcDeBoor(std::vector<POINT> points, int degree) {
	// 默认计算 100 个点
	return CalcDeBoor(points, degree, BCURVECALCPOINT);
}

// 判断点是否在矩形内
bool IsInside(RECT rect, POINT pt) {
    return pt.x >= rect.left && pt.x <= rect.right &&
        pt.y >= rect.top && pt.y <= rect.bottom;
}

#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8

bool MidpointClipLine(const RECT& clipRect, POINT& p1, POINT& p2) {
    POINT p3 = p1;
    POINT p4 = p2;

    // 定义编码
    auto encode = [](const RECT& rect, POINT pt) -> int {
        int code = 0;
        if (pt.x < rect.left) code |= LEFT;
        if (pt.x > rect.right) code |= RIGHT;
        if (pt.y < rect.top) code |= TOP;
        if (pt.y > rect.bottom) code |= BOTTOM;
        return code;
        };

    int code1 = encode(clipRect, p1);
    int code2 = encode(clipRect, p2);
    int code3 = code2;

    // 如果完全在裁剪窗口外
    if ((code1 & code2) != 0) {
        return false;
    }

    // 如果完全在裁剪窗口内
    if (code1 == 0 && code2 == 0) {
        return true; // 不需要裁剪
    }

    // 递归裁剪函数
    auto midpointClip = [&](POINT& p1, int& code1, POINT& p2, int& code2) {
        while (true) {
            POINT mid = { (p1.x + p2.x) / 2, (p1.y + p2.y) / 2 };
            int midCode = encode(clipRect, mid);

            if (abs(mid.x - p1.x) <= 1 && abs(mid.y - p1.y) <= 1) {
                if (code1 == 0) break;
                p1 = mid; // 找到交点
                code1 = midCode;
                break;
            }

            // 判断中点与 p1 是否完全在同一区域外
            if ((code1 & midCode) != 0) {
                p1 = mid; // 更新起点到中点
                code1 = midCode;
            }
            else {
                p2 = mid; // 更新终点到中点
                code2 = midCode;
            }
        }
        };

    // 找到离 p1 最近的交点
    midpointClip(p1, code1, p2, code2);

    // 保存交点P3
    p3 = p1;
    // 恢复P2
    p2 = p4;
    code2 = code3;

    if (code1 == 0 && code2 == 0) return true;

    // 离 p2 最近的交点
    midpointClip(p2, code2, p1, code1);

    p1 = p3;

    return true;
}


// 判断点是否在某条裁剪边内
bool Inside(POINT p, RECT clipRect, int edge) {
    switch (edge) {
    case 0: return p.y >= clipRect.top;    // 上边
    case 1: return p.x <= clipRect.right; // 右边
    case 2: return p.y <= clipRect.bottom;// 下边
    case 3: return p.x >= clipRect.left;  // 左边
    }
    return false;
}

// 求交点
POINT Intersect(POINT p1, POINT p2, RECT clipRect, int edge) {
    POINT inter = {};
    if (edge == 0) { // 上边
        inter.x = p1.x + (p2.x - p1.x) * (clipRect.top - p1.y) / (p2.y - p1.y);
        inter.y = clipRect.top;
    }
    else if (edge == 1) { // 右边
        inter.y = p1.y + (p2.y - p1.y) * (clipRect.right - p1.x) / (p2.x - p1.x);
        inter.x = clipRect.right;
    }
    else if (edge == 2) { // 下边
        inter.x = p1.x + (p2.x - p1.x) * (clipRect.bottom - p1.y) / (p2.y - p1.y);
        inter.y = clipRect.bottom;
    }
    else if (edge == 3) { // 左边
        inter.y = p1.y + (p2.y - p1.y) * (clipRect.left - p1.x) / (p2.x - p1.x);
        inter.x = clipRect.left;
    }
    return inter;
}

// Sutherland-Hodgman多边形裁剪
std::vector<POINT> SutherlandHodgman(RECT clipRect, std::vector<POINT> polygon) {
    for (int edge = 0; edge < 4; edge++) {
        std::vector<POINT> newPolygon;
        if (polygon.size() == 0) return polygon;
        POINT prev = polygon.back();

        for (const auto& cur : polygon) {
            if (Inside(cur, clipRect, edge)) {
                if (!Inside(prev, clipRect, edge)) {
                    newPolygon.push_back(Intersect(prev, cur, clipRect, edge));
                }
                newPolygon.push_back(cur);
            }
            else if (Inside(prev, clipRect, edge)) {
                newPolygon.push_back(Intersect(prev, cur, clipRect, edge));
            }
            prev = cur;
        }
        polygon = newPolygon;
    }
    return polygon;
}

typedef struct Node {
    POINT point;
    Node* next;
    Node* rect;
    Node* last;                     // 对于裁剪多边形当前插入位置的末尾

    bool isrect;                    // 是否为裁剪矩形的点
    bool isIntersection = false;    // 是否为交点
    bool isEntry = false;           // 是否为进入点
    bool visited = false;           // 是否已访问
} Node;

bool EqulP(const POINT& p1, const POINT& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

// 判断是否有交点
pair<POINT, bool> GetIntersection(const POINT& p1, const POINT& p2, const POINT& c1, const POINT& c2) {
    POINT inter;
    float a1 = p2.y - p1.y;
    float b1 = p1.x - p2.x;
    float c1_eq = a1 * p1.x + b1 * p1.y;

    float a2 = c2.y - c1.y;
    float b2 = c1.x - c2.x;
    float c2_eq = a2 * c1.x + b2 * c1.y;

    // 斜率相同
    float det = a1 * b2 - a2 * b1;
    if (std::abs(det) < 1e-5) return { inter, false };

    inter.x = static_cast<LONG>((b2 * c1_eq - b1 * c2_eq) / det);
    inter.y = static_cast<LONG>((a1 * c2_eq - a2 * c1_eq) / det);

    // 检查交点是否在两线段上包含端点
    auto isBetween = [](LONG val, LONG minVal, LONG maxVal) {
        return val >= min(minVal, maxVal) && val <= max(minVal, maxVal);
        };

    // 如果交点等于四个端点之一
    if (EqulP(inter, p1) || EqulP(inter, p2) || EqulP(inter, c1) || EqulP(inter, c2)) return { inter, false };

    if (isBetween(inter.x, p1.x, p2.x) &&
        isBetween(inter.y, p1.y, p2.y) &&
        isBetween(inter.x, c1.x, c2.x) &&
        isBetween(inter.y, c1.y, c2.y)) {
        return { inter, true };
    }

    return { inter, false };
}

bool IsEntryPoint(const POINT& polyPoint, const POINT& clipStart, const POINT& clipEnd) {
    LONG crossProduct = (polyPoint.x - clipStart.x) * (clipEnd.y - clipStart.y) -
        (polyPoint.y - clipStart.y) * (clipEnd.x - clipStart.x);
    return crossProduct > 0; // >0 表示进入，<0 表示离开
}

Node* InsertNode(Node*& head, Node* position, const POINT& intersection, bool isEntry, bool isIntersection, bool isrect) {
    Node* newNode = new Node{ intersection };
    newNode->isIntersection = isIntersection;
    newNode->isEntry = isEntry;
    newNode->isrect = isrect;

    if (!head) {
        head = newNode;
        head->next = head; // 自环
        if (isrect) {
            head->rect = head;
            head->last = newNode;
        }
        return head;
    }

    if (isrect) {
        newNode->rect = position->rect;
        position->rect = newNode;
        newNode->last = newNode;
    }
    newNode->next = position->next;
    position->next = newNode;

    return newNode;
}

// 辅助函数：在裁剪多边形中找到对应的交点
Node* FindNodeInCP(Node* CP, Node* intersection) {
    Node* temp = CP;
    do {
        if (temp->point.x == intersection->point.x &&
            temp->point.y == intersection->point.y &&
            temp->isIntersection) {
            return temp;
        }
        temp = temp->next;
    } while (temp != CP);
    return nullptr;
}

// 辅助函数：在输入多边形中找到对应的交点
Node* FindNodeInSP(Node* SP, Node* intersection) {
    Node* temp = SP;
    do {
        if (temp->point.x == intersection->point.x &&
            temp->point.y == intersection->point.y &&
            temp->isIntersection) {
            return temp;
        }
        temp = temp->next;
    } while (temp != SP);
    return nullptr;
}

int InRECT(POINT p, RECT rect) {
    if (p.x > rect.left && p.x < rect.right && p.y > rect.top && p.y < rect.bottom) {
        // 矩形内
        return 1;
    }
    else {
        // 矩形外
        if (p.x < rect.left || p.x > rect.right || p.y < rect.top || p.y > rect.bottom) return -1;
        // 矩形上
        return 0;
    }
}

int InMulti(POINT p, const std::vector<POINT>& list) {
    int count = 0;
    int n = list.size();

    for (int i = 0; i < n; ++i) {
        const POINT& p1 = list[i];
        const POINT& p2 = list[(i + 1) % n];

        // 检查点是否在边上
        if ((p.y - p1.y) * (p2.x - p1.x) == (p.x - p1.x) * (p2.y - p1.y)) { // 共线判断
            if (p.x >= min(p1.x, p2.x) && p.x <= max(p1.x, p2.x) &&
                p.y >= min(p1.y, p2.y) && p.y <= max(p1.y, p2.y)) {
                return 0; // 点在边上
            }
        }

        // 射线法判断
        if ((p1.y > p.y) != (p2.y > p.y)) {
            double xIntersect = (double)(p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
            if (p.x < xIntersect) {
                count++;
            }
        }
    }

    // 如果交点数为奇数，点在多边形内部；否则在外部
    return count % 2 == 1 ? 1 : -1;
}

bool CheckFMulti(RECT clipRect, std::vector<POINT> polygon) {
    bool ssz = true;
    double area = 0.0;
    int n = polygon.size();
    for (int i = 0; i < n; i++) {
        const POINT& p1 = polygon[i];
        const POINT& p2 = polygon[(i + 1) % n];
        area += p1.x * p2.y - p1.y * p2.x;
    }
    if (area < 0) ssz = false;
    return ssz;
}

// Weiler-Atherton多边形裁剪
std::vector<std::vector<POINT>> WeilerAthertonClip(RECT clipRect, std::vector<POINT> polygon) {
    // 存储结果的容器
    std::vector<std::vector<POINT>> result;
    // 检查多边形
    bool check = CheckFMulti(clipRect, polygon);

    // 逆时针，反向
    if (!check) {
        reverse(polygon.begin(), polygon.end());
    }

    // 初始化输入多边形（SP）和裁剪多边形（CP）的环形链表
    Node* SP = nullptr;
    Node* CP = nullptr;

    // 初始化裁剪多边形（矩形顶点）
    POINT topLeft = { clipRect.left, clipRect.top };
    POINT topRight = { clipRect.right, clipRect.top };
    POINT bottomRight = { clipRect.right, clipRect.bottom };
    POINT bottomLeft = { clipRect.left, clipRect.bottom };

    std::vector<POINT> clipPolygon = { topLeft, topRight, bottomRight, bottomLeft, topLeft };
    
    Node* CPi = CP;
    for (size_t i = 0; i < clipPolygon.size(); ++i) {
        CPi = InsertNode(CPi, CPi, clipPolygon[i], false, false, true);
    }
    CP = CPi->next;

    // 初始化输入多边形
    Node* SPi = SP;
    for (const auto& p : polygon) {
        SPi = InsertNode(SPi, SPi, p, false, false, false);
    }
    SPi = InsertNode(SPi, SPi, polygon[0], false, false, false);
    SP = SPi->next;

    bool hasInter = false;
    // 计算交点并插入
    Node* spCurrent = SP;
    do {
        Node* cpCurrent = CP;
        // 当前SP边与所有rect边的交点
        vector<pair<POINT, bool>> intersection;
        // 第一次遍历获取交点
        do {
            pair<POINT, bool> data = GetIntersection(spCurrent->point, spCurrent->next->point,
            cpCurrent->point, cpCurrent->rect->point);
            if (data.second) {
                hasInter = true;
                bool isEntry = IsEntryPoint(spCurrent->point, cpCurrent->point, cpCurrent->rect->point);
                data.second = isEntry;
                intersection.push_back(data);
                // 交叉多边形
                // 将交点插入, 插入当前rect到下一个rect之间的最后一个next末尾
                //Node* last = InsertNode(CP, cpCurrent->last, data.first, data.second, true, false);
                //cpCurrent->last = last;

                // 将交点插入
                Node* rectC = cpCurrent;
                do {
                    POINT p1 = rectC->point;
                    POINT p2 = rectC->next->point;
                    if (p1.x == p2.x) {
                        if ((p1.y < data.first.y && data.first.y < p2.y) || (p1.y > data.first.y && data.first.y > p2.y)) {
                            // 两个rect之间
                            InsertNode(CP, rectC, data.first, data.second, true, false);
                            break;
                        }
                    }
                    else if (p1.y == p2.y) {
                        if ((p1.x < data.first.x && data.first.x < p2.x) || (p1.x > data.first.x && data.first.x > p2.x)) {
                            // 两个rect之间
                            InsertNode(CP, rectC, data.first, data.second, true, false);
                            break;
                        }
                    }

                    rectC = rectC->next;
                } while (rectC->isrect != true && rectC != cpCurrent);
            }

            cpCurrent = cpCurrent->rect;
        } while (cpCurrent != CP);

        // 将交点插入SP列表
        if (intersection.size() == 1) {
            InsertNode(SP, spCurrent, intersection[0].first, intersection[0].second, true, false);
            spCurrent = spCurrent->next;
        }
        else if (intersection.size() == 2) {
            int d1, d2;
            d1 = pow(intersection[0].first.x - spCurrent->point.x, 2) + pow(intersection[0].first.y - spCurrent->point.y, 2);
            d2 = pow(intersection[1].first.x - spCurrent->point.x, 2) + pow(intersection[1].first.y - spCurrent->point.y, 2);
            // 离 spCurrent->point 近的先插入
            if (d1 < d2) {
                InsertNode(SP, spCurrent, intersection[0].first, intersection[0].second, true, false);
                spCurrent = spCurrent->next;
                InsertNode(SP, spCurrent, intersection[1].first, intersection[1].second, true, false);
                spCurrent = spCurrent->next;
            }
            else {
                InsertNode(SP, spCurrent, intersection[1].first, intersection[1].second, true, false);
                spCurrent = spCurrent->next;
                InsertNode(SP, spCurrent, intersection[0].first, intersection[0].second, true, false);
                spCurrent = spCurrent->next;
            }
        }

        spCurrent = spCurrent->next;
    } while (spCurrent != SP);

    // 调整CP循环队列
    // 交叉多边形
    //Node* cpCurrent = CP;
    //do {
    //    // 第一个节点为入点
    //    if (cpCurrent->next->isEntry) {
    //        // 找到指向最后一个节点的 Node
    //        Node* p = cpCurrent;
    //        
    //        p->last->next = p->next;
    //        p->next = p->next->next;
    //        p->last->next->next = p->rect;
    //        p->last = p->last->next;
    //    }
    //    cpCurrent = cpCurrent->rect;
    //} while (cpCurrent != CP);

    // 如果没有交点
    if (!hasInter) {
        bool allInRect = true;     // 全在矩形上
        for (int i = 0; i < polygon.size(); i++) {
            int code = InRECT(polygon[i], clipRect);
            // 多边形点全在矩形内部
            if (code == 1) {
                std::vector<POINT> rect;
                for (POINT p : polygon) {
                    rect.push_back(p);
                }
                result.push_back(rect);
                return result;
            }
            // 多边形点都在矩形外
            else if(code == -1) {
                bool allInMulti = true;
                allInRect = false;
                // 矩形在多边形内部
                for (int j = 0; j < clipPolygon.size(); j++) {
                    int inMul = InMulti(clipPolygon[j], polygon);
                    // 在多边形内部
                    if (inMul == 1) {
                        std::vector<POINT> rect;
                        rect.push_back({ clipRect.left, clipRect.top });
                        rect.push_back({ clipRect.left, clipRect.bottom });
                        rect.push_back({ clipRect.right, clipRect.bottom });
                        rect.push_back({ clipRect.right, clipRect.top });
                        result.push_back(rect);
                        return result;
                    }
                    // 在多边形外部，退出
                    else if (inMul == -1) {
                        allInMulti = false;
                        break;
                    }
                }

                // 矩形的点全在多边形上
                if (allInMulti) {
                    std::vector<POINT> rect;
                    rect.push_back({ clipRect.left, clipRect.top });
                    rect.push_back({ clipRect.left, clipRect.bottom });
                    rect.push_back({ clipRect.right, clipRect.bottom });
                    rect.push_back({ clipRect.right, clipRect.top });
                    result.push_back(rect);
                    return result;
                }
            }
        }

        // 多边形的点全在矩形上
        if (allInRect) {
            std::vector<POINT> rect;
            for (POINT p : polygon) {
                rect.push_back(p);
            }
            result.push_back(rect);
            return result;
        }
    }

    // 遍历多边形，生成结果
    Node* current = SP;
    bool inSp = true;
    do {
        // 从入点开始
        Node* temp = current;
        inSp = true;
        if (temp->isEntry && !temp->visited) {
            std::vector<POINT> path;
            Node* start = temp;

            do {
                temp->visited = true;
                // 添加当前点到路径
                path.push_back(temp->point);

                // 如果是交点，切换到另一个多边形
                if (temp->isIntersection && temp->isEntry != inSp) {
                    // 若是入点，继续在SP，出点则前往CP
                    temp = temp->isEntry ? FindNodeInSP(SP, temp) : FindNodeInCP(CP, temp);
                    temp->visited = true;
                    inSp = !inSp;
                }

                temp = temp->next;
            } while (temp->point.x != start->point.x || temp->point.y != start->point.y);

            // 保存完整路径
            result.push_back(path);
        }

        current = current->next;
    } while (current != SP);

    return result;
}

pair<bool, bool> CutCSDraw(StoreImg& imgs, CSDrawInfo& csdraw, const RECT& cutrect, Coordinate coordinate) {
    DrawInfo* choose = &(csdraw.choose);
    bool drawstate = true;
    bool redrawFix = false;
    switch (choose->type) {
    case LINE:
    {
        switch (GetCutFunc(csdraw.config, CUTFUNC)) {
        case 1:
            POINT start = mapCoordinate(coordinate, choose->line.start);
            POINT end = mapCoordinate(coordinate, choose->line.end);
            if (MidpointClipLine(cutrect, start, end)) {
                MyPoint* mps = &(choose->line.start);
                MyPoint* mpe = &(choose->line.end);
                PointToCoordinate(coordinate, start, mps->x, mps->y);
                PointToCoordinate(coordinate, end, mpe->x, mpe->y);
            }
            else {
                csdraw.index = -1;
                drawstate = false;
            }
            
            break;
        }
    }
    break;
    case FMULTILINE:
    {
        vector<POINT> points = mapMyPointsV(choose->multipoint.points, coordinate, choose->multipoint.numPoints, choose->multipoint.endNum);
        vector<POINT> npoints;
        vector<vector<POINT>> results;
        switch (GetCutFunc(csdraw.config, CUTFUNC)) {
        case 1:
            // 裁剪
            npoints = SutherlandHodgman(cutrect, points);
            if (npoints.size() == 0) {
                ClearMultipoint(&(choose->multipoint));
                csdraw.index = -1;
                drawstate = false;
            }
            else {
                InitMultipFromV(&(choose->multipoint), npoints, coordinate);
            }
            break;
        case 2:
            results = WeilerAthertonClip(cutrect, points);
            if (results.size() == 0) {
                ClearMultipoint(&(choose->multipoint));
                csdraw.index = -1;
                drawstate = false;
            }
            else {
                InitMultipFromV(&(choose->multipoint), results[0], coordinate);
                for (int i = 1; i < results.size(); i++) {
                    DrawInfo multip;
                    multip.type = FMULTILINE;
                    multip.proper = choose->proper;
                    InitMultipoint(&(multip.multipoint));
                    InitMultipFromV(&(multip.multipoint), results[i], coordinate);
                    AddDrawInfoToStoreImg(&imgs, multip);
                }
                redrawFix = true;
            }
            break;
        }

    }
    break;
    }

    CalcCSDrawRect(csdraw, coordinate);

    return { drawstate ,redrawFix };
}