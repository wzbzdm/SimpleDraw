#pragma once

#include "drawinfo.h"
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

void MoveMyPoint(MyPoint& p, double x, double y) {
	p.x += x;
	p.y += y;
}

// 图像平移
void MoveDrawInfo(DrawInfo &info, double x, double y) {
	if (x == 0 && y == 0) return;
    switch (info.type) {
    case LINE:
    {
        MoveMyPoint(info.line.start, x, y);
        MoveMyPoint(info.line.end, x, y);
    }
    break;
    case CIRCLE:
    {
		MoveMyPoint(info.circle.center, x, y);
    }
    break;
    case RECTANGLE:
    {
		MoveMyPoint(info.rectangle.start, x, y);
		MoveMyPoint(info.rectangle.end, x, y);
    }
    break;
    case CURVE:
    case BCURVE:
    case MULTILINE:
    case FMULTILINE:
    {
        for (int i = 0; i < info.multipoint.endNum; i++) {
            if (HFMyPoint(&(info.multipoint.points[i]))) {
                MoveMyPoint(info.multipoint.points[i], x, y);
            }
        }
    }
    break;
    }
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

// 中点分割裁剪函数
bool MidpointClipLine(RECT clipRect, POINT& p1, POINT& p2) {
    if (IsInside(clipRect, p1) && IsInside(clipRect, p2)) {
        return true; // 完全在内部，保留
    }
    if ((p1.x < clipRect.left && p2.x < clipRect.left) ||
        (p1.x > clipRect.right && p2.x > clipRect.right) ||
        (p1.y < clipRect.top && p2.y < clipRect.top) ||
        (p1.y > clipRect.bottom && p2.y > clipRect.bottom)) {
        return false; // 完全在外部，舍弃
    }

    // 递归裁剪中点
    POINT mid = { (p1.x + p2.x) / 2, (p1.y + p2.y) / 2 };
    return MidpointClipLine(clipRect, p1, mid) && MidpointClipLine(clipRect, mid, p2);
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
    POINT inter;
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

std::vector<POINT> WeilerAthertonClip(RECT clipRect, std::vector<POINT> polygon) {
    std::vector<POINT> clippedPolygon;
    std::vector<POINT> intersectionPoints;

    for (int edge = 0; edge < 4; edge++) {
        std::vector<POINT> newPolygon;
        POINT prev = polygon.back();

        for (const POINT& cur : polygon) {
            if (Inside(cur, clipRect, edge)) {
                if (!Inside(prev, clipRect, edge)) {
                    // 添加入口点
                    newPolygon.push_back(Intersect(prev, cur, clipRect, edge));
                }
                newPolygon.push_back(cur); // 添加内部点
            }
            else if (Inside(prev, clipRect, edge)) {
                // 添加出口点
                newPolygon.push_back(Intersect(prev, cur, clipRect, edge));
            }
            prev = cur;
        }
        polygon = newPolygon;
    }

    // 对结果进行排序、去重，形成最终的裁剪多边形
    clippedPolygon = polygon;
    return clippedPolygon;
}
