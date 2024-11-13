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
void CalculateImg(StoreImg& allimg, int count) {
    CalculatePoints.clear();
    DrawInfo choose = allimg.img[count];
    for (int i = 0; i < allimg.endNum; i++) {
        if (i == count) continue;
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

// 图形选择逻辑
int ChooseImg(StoreImg& store, Coordinate coor, POINT p) {
    int count = -1;
    double minDistance = MAXWTOCHHOSE * coor.radius;
    MyPoint mp;
    PointToCoordinate(coor, p, mp.x, mp.y);
    for (int i = 0; i < store.endNum; i++) {
        DrawInfo item = store.img[i];
        double d;
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

        if (minDistance > d) {
            minDistance = d;
            count = i;
        }
    }

    return count;
}

// 使当前坐标系适应画布上的图像，方便观察
void FitCoordinate(Coordinate& coor, StoreImg& img, RECT canvasRect) {
    // 计算图像的最大最小坐标
    double minX = ILLEGELMYPOINT, minY = ILLEGELMYPOINT, maxX = DBL_MIN, maxY = DBL_MIN;

    // 若没有图像
    if (img.endNum == 0) {
        POINT center = { (canvasRect.right - canvasRect.left) / 2, (canvasRect.bottom - canvasRect.top) / 2 };
        SetCoordinate(coordinate, center, DEFAULTRADIUS); // 设置坐标系参数
        return;
    }
    for (int i = 0; i < img.endNum; i++) {
        DrawInfo item = img.img[i];
        switch (item.type) {
        // MyLine和MyRectangle结构相同
        case RECTANGLE:
        case LINE:
        {
            if (item.line.start.x < item.line.end.x) {
                if (item.line.start.x < minX) minX = item.line.start.x;
                if (item.line.end.x > maxX) maxX = item.line.end.x;
            }
            else {
                if (item.line.end.x < minX) minX = item.line.end.x;
                if (item.line.start.x > maxX) maxX = item.line.start.x;
            }

            if (item.line.start.y < item.line.end.y) {
                if (item.line.start.y < minY) minY = item.line.start.y;
                if (item.line.end.y > maxY) maxY = item.line.end.y;
            }
            else {
                if (item.line.end.y < minY) minY = item.line.end.y;
                if (item.line.start.y > maxY) maxY = item.line.start.y;
            }
            
            break;
        }
        case CIRCLE:
        {
            if (item.circle.center.x - item.circle.radius < minX) minX = item.circle.center.x - item.circle.radius;
            if (item.circle.center.y - item.circle.radius < minY) minY = item.circle.center.y - item.circle.radius;
            if (item.circle.center.x + item.circle.radius > maxX) maxX = item.circle.center.x + item.circle.radius;
            if (item.circle.center.y + item.circle.radius > maxY) maxY = item.circle.center.y + item.circle.radius;
            break;
        }
        case CURVE:
        case BCURVE:
        case FMULTILINE:
        case MULTILINE:
        {
            for (int j = 0; j < item.multipoint.endNum; j++) {
                if (item.multipoint.points[j].x == ILLEGELMYPOINT || item.multipoint.points[j].y == ILLEGELMYPOINT) continue;
                if (item.multipoint.points[j].x < minX) minX = item.multipoint.points[j].x;
                if (item.multipoint.points[j].y < minY) minY = item.multipoint.points[j].y;
                if (item.multipoint.points[j].x > maxX) maxX = item.multipoint.points[j].x;
                if (item.multipoint.points[j].y > maxY) maxY = item.multipoint.points[j].y;
            }
            break;
        }
        default:
            break;
        }
    }

    // 处理无效坐标情况
    if (minX == ILLEGELMYPOINT || minY == ILLEGELMYPOINT || maxX == ILLEGELMYPOINT || maxY == ILLEGELMYPOINT) {
        // 没有有效的图形，设置默认坐标
        POINT center = { (canvasRect.right - canvasRect.left) / 2, (canvasRect.bottom - canvasRect.top) / 2 };
        SetCoordinate(coor, center, DEFAULTRADIUS); // 设置坐标系参数
        return;
    }

    // 计算坐标系的中心点
    double centerX = (minX + maxX) / 2;
    double centerY = (minY + maxY) / 2;

    int canvasWidth = canvasRect.right - canvasRect.left;
    int canvasHeight = canvasRect.bottom - canvasRect.top;

    // 计算坐标系的半径
    double radiusX = maxX - centerX;
    double radiusY = maxY - centerY;

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