#pragma once

#include "windowState.h"
#include "drawinfo.h"
#include "drawAny.h"

void LineFirstPoint(POINT point) {
	mst.lastLButtonPoint = point;
	MyPoint start;
	PointToCoordinate(coordinate, point, start.x, start.y);
	drawing.info.line.start = start;
}

void LineNextPoint(POINT point) {
	MyPoint end;
	PointToCoordinate(coordinate, point, end.x, end.y);
	drawing.info.line.end = end;
	drawing.info.proper = customProperty;

	// 保存线
	DrawInfo line ;
	InitDrawInfo(&drawing, &line);
	AddDrawInfoToStoreImg(&allImg, line);
	mst.lastLButtonPoint = INITPOINT;
}

void CircleFirstPoint(POINT point) {
	mst.lastLButtonPoint = point;
	MyPoint center;
	PointToCoordinate(coordinate, point, center.x, center.y);
	drawing.info.circle.center = center;
}

void CircleNextPoint(POINT point) {
	MyPoint end;
	PointToCoordinate(coordinate, point, end.x, end.y);
	double r = sqrt((drawing.info.circle.center.x - end.x) * (drawing.info.circle.center.x - end.x) + (drawing.info.circle.center.y - end.y) * (drawing.info.circle.center.y - end.y));
	drawing.info.circle.radius = r;
	drawing.info.proper = customProperty;

	DrawInfo circle;
	InitDrawInfo(&drawing, &circle);
	AddDrawInfoToStoreImg(&allImg, circle);
	mst.lastLButtonPoint = INITPOINT;
}

void RectFirstPoint(POINT point) {
	mst.lastLButtonPoint = point;
	MyPoint start;
	PointToCoordinate(coordinate, point, start.x, start.y);
	drawing.info.rectangle.start = start;
}

void RectNextPoint(POINT point) {
	MyPoint end;
	PointToCoordinate(coordinate, point, end.x, end.y);
	drawing.info.rectangle.end = end;
	drawing.info.proper = customProperty;

	// 保存线
	DrawInfo rect;
	InitDrawInfo(&drawing, &rect);
	AddDrawInfoToStoreImg(&allImg, rect);
	mst.lastLButtonPoint = INITPOINT;
}

void MultiPFirstPoint(POINT point) {
	mst.lastLButtonPoint = point;
	// 初始化 mst.
	InitMultipoint(&(drawing.info.multipoint));
	MyPoint fPoint;
	PointToCoordinate(coordinate, point, fPoint.x, fPoint.y);
	AddPointToMultipoint(&(drawing.info.multipoint), fPoint);
}

void MultiPNextPoint(POINT point) {
	mst.lastLButtonPoint = point;
	MyPoint nextPoint;
	PointToCoordinate(coordinate, point, nextPoint.x, nextPoint.y);
	AddPointToMultipoint(&(drawing.info.multipoint), nextPoint);
}

void MultiPDone() {
	drawing.info.proper = customProperty;
	DrawInfo mline;
	InitDrawInfo(&drawing, &mline);
	AddDrawInfoToStoreImg(&allImg, mline);
	ClearMultipoint(&(drawing.info.multipoint));
	mst.lastLButtonPoint = INITPOINT;
}

void BCurveFirstPoint(POINT point) {
	mst.lastLButtonPoint = point;
	// 初始化 mst.
	InitMultipoint(&(drawing.info.multipoint));
	MyPoint fPoint;
	PointToCoordinate(coordinate, point, fPoint.x, fPoint.y);
	AddPointToMultipoint(&(drawing.info.multipoint), fPoint);
}

void BCurveNextPoint(POINT point) {
	mst.lastLButtonPoint = point;
	MyPoint nextPoint;
	PointToCoordinate(coordinate, point, nextPoint.x, nextPoint.y);
	AddPointToMultipoint(&(drawing.info.multipoint), nextPoint);
}

void BCurveDone() {
	drawing.info.proper = customProperty;
	DrawInfo bcurve;
	InitDrawInfo(&drawing, &bcurve);
	AddDrawInfoToStoreImg(&allImg, bcurve);
	ClearMultipoint(&(drawing.info.multipoint));
	mst.lastLButtonPoint = INITPOINT;
}