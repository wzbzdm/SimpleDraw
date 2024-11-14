#ifndef DRAWINFO_H
#define DRAWINFO_H

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <climits>
#include <Windows.h>
#include <cassert>

#define ILLEGELMYPOINT	DBL_MAX
#define INITMYPOINT		{ILLEGELMYPOINT, ILLEGELMYPOINT}

// 最开始创建点数组时的大小，以后每次增加的大小5
#define MAX_POINT 20
#define ADD_POINT 10

// 最开始的图像数量，以后每次增加的大小8
#define MAX_IMG_NUM 30
#define ADD_IMG_NUM 15

#define IMG_HEADER "GTX"
#define FILE_VERSION "1.4"

#define FILEHEADERL 4
#define FILEVERSIONL 8

#define BSPLINE				3
			
#define DRAWSYSTEM			0x1
#define DRAWBRE				0x2
#define DRAWMID				0x3
#define PADSYSTEM			0x10
#define PADSCAN				0x20
#define PADZL				0x30
#define DRAWTYPEBIT			0xf
#define PADTYPEBIT			0xf0
#define DRAWTYPEMASK		~DRAWTYPEBIT
#define PADTYPEMASK			~PADTYPEBIT
#define DRAWTYPE(gc)		(gc & DRAWTYPEBIT)
#define PADTYPE(gc)			(gc & PADTYPEBIT)
#define ISDRAWSYSTEM(gc)	(gc & DRAWTYPEBIT == DRAWSYSTEM)
#define ISDRAWBRE(gc)		(gc & DRAWTYPEBIT == DRAWBRE)
#define ISDRAWMID(gc)		(gc & DRAWTYPEBIT == DRAWMID)
#define ISPADSYSTEM(gc)		(gc & PADTYPEBIT == PADSYSTEM)
#define ISPADSCAN(gc)		(gc & PADTYPEBIT == PADSCAN)
#define ISPADZL(gc)			(gc & PADTYPEBIT == PADZL)

	// 绘制方式，采用系统api or 自定义api
	typedef enum gctype {
		DRAWGC	= 0x1,		// 系统画线
		PADGC	= 0x10,
		SYSTEM	= 0x1,
		CUSTOM1 = 0x2,
		CUSTOM2 = 0x3,
		DEFAULTTYPE = DRAWSYSTEM | PADSYSTEM
	} gctype;

	void SetDrawTypeR(gctype *gc, int type) {
		*gc = (gctype)(*gc & DRAWTYPEMASK);
		*gc = (gctype)(*gc | type);
	}

	gctype SetDrawType(gctype gc, int type) {
		return (gctype)((gc & DRAWTYPEMASK) | type);
	}

	void SetPadTypeR(gctype *gc, int type) {
		*gc = (gctype)(*gc & PADTYPEMASK);
		*gc = (gctype)(*gc | type);
	}

	gctype SetPadType(gctype gc, int type) {
		return (gctype)((gc & PADTYPEMASK) | type);
	}

	// 图元属性
	typedef struct DrawUnitProperty {
		unsigned int color;
		unsigned int bgcolor;
		int width;
		gctype type;	// 绘制方式
	} DrawUnitProperty;

	void SetColorWithColorRef(DrawUnitProperty *pro, COLORREF r) {
		pro->color = r;
	}

	void SetBgColorWithColorRef(DrawUnitProperty* pro, COLORREF r) {
		pro->bgcolor = r;
	}

	void SetWidth(DrawUnitProperty *pro, int w) {
		pro->width = w;
	}

	void SetType(DrawUnitProperty* pro, gctype t) {
		pro->type = t;
	}

// 绘图方式为系统API
#define DEFAULTLINEWID		2				// 线宽为2
#define DEFAULTLINECOR		0				// 黑色
#define DEFAULTPADCOR		0x000000ff		// 默认填充颜色
#define DEFAULTDRAWPROPERTY {DEFAULTLINECOR, DEFAULTPADCOR, DEFAULTLINEWID, DEFAULTTYPE}

	// 图元类型
	typedef enum ImgType {
		NONE,   // 删除或者无效的图像
		LINE,
		CIRCLE,
		RECTANGLE,
		CURVE,
		MULTILINE,
		FMULTILINE,	// 结构与MULTILINE相同
		BCURVE		// B样条曲线
	} ImgType;

	typedef struct MyPoint {
		double x;
		double y;
	} MyPoint;

	bool HFMyPoint(const MyPoint* mp) {
		return mp->x != ILLEGELMYPOINT && mp->y != ILLEGELMYPOINT;
	}

	double Distance(MyPoint p1, MyPoint p2) {
		return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
	}

	typedef struct Line {
		MyPoint start;
		MyPoint end;
	} MyLine;

	double DistanceToLine(MyPoint p, MyLine line) {
		double lineLength = Distance(line.start, line.end);
		if (lineLength == 0.0) return Distance(p, line.start); // 线段退化为一个点
		double t = ((p.x - line.start.x) * (line.end.x - line.start.x) +
			(p.y - line.start.y) * (line.end.y - line.start.y)) / pow(lineLength, 2);
		t = fmax(0, fmin(1, t));
		MyPoint projection = { (line.start.x + t * (line.end.x - line.start.x)),
							   (line.start.y + t * (line.end.y - line.start.y)) };
		return Distance(p, projection);
	}

	typedef struct Circle {
		MyPoint center;
		double radius;
	} MyCircle;

	double GetDPointToCircle(MyPoint p, MyCircle circle) {
		return fabs(Distance(p, circle.center) - circle.radius);
	}

	typedef struct Rectangle {
		MyPoint start;
		MyPoint end;
	} MyRectangle;

	double GetMinDPointToRectangle(MyPoint p, MyRectangle rect) {
		double dis = ILLEGELMYPOINT;
		double left = min(rect.start.x, rect.end.x);
		double right = max(rect.start.x, rect.end.x);
		double top = max(rect.start.y, rect.end.y);
		double bottom = min(rect.start.y, rect.end.y);

		if (p.x < left && p.y > top) {
			dis = Distance(p, { left, top });
		}
		else if (p.x < left && p.y < bottom) {
			dis = Distance(p, { left, bottom });
		}
		else if (p.x > right && p.y > top) {
			dis = Distance(p, { right, top });
		}
		else if (p.x > right && p.y < bottom) {
			dis = Distance(p, { right, bottom });
		}
		else if (p.x < left) {
			dis = left - p.x;
		}
		else if (p.x > right) {
			dis = p.x - right;
		}
		else if (p.y > top) {
			dis = p.y - top;
		}
		else if (p.y < bottom) {
			dis = bottom - p.y;
		}
		else {
			dis = min(p.x - left, right - p.x);
			dis = min(dis, top - p.y);
			dis = min(dis, p.y - bottom);
		}

		return dis;
	}

	typedef struct MultPoint {
		MyPoint* points; // 多义线的顶点数组
		int numPoints; // 顶点的数量
		int endNum;    // 当前存储的最后一个数值在数组中的位置
		int maxNum;			 // 当前数组的最大容量
	} MyMultiPoint;

	void ScanMultipoint(MyMultiPoint* multipoint) {
		if (multipoint->endNum == multipoint->numPoints) return;
		int lastNull = 0;
		for (int i = 0; i < multipoint->endNum; i++) {
			if (!HFMyPoint(&(multipoint->points[i]))) continue;
			multipoint->points[lastNull++] = multipoint->points[i];
		}

		multipoint->endNum = lastNull;
		multipoint->numPoints = lastNull;
	}

	double GetMinDPointToMultipoint(MyPoint p, MyMultiPoint* multipoint) {
		double dis = ILLEGELMYPOINT;
		ScanMultipoint(multipoint);
		if (multipoint->endNum != multipoint->numPoints) {
			return -1;
		}
		for (int i = 0; i < multipoint->endNum; ++i) {
			MyLine segment = { multipoint->points[i], multipoint->points[i + 1] };
			dis = min(dis, DistanceToLine(p, segment));
		}
		return dis;
	}

	double GetMinDPointToFMultipoint(MyPoint p, MyMultiPoint* multipoint) {
		double dis = GetMinDPointToMultipoint(p, multipoint);
		MyLine end = { multipoint->points[0], multipoint->points[multipoint->numPoints - 1] };
		dis = min(dis, DistanceToLine(p, end));

		return dis;
	}

	void InitFromMultipoint(MyMultiPoint* multipoint, MyMultiPoint* another) {
		if (multipoint == another) return;
		multipoint->points = (MyPoint*)malloc(another->maxNum * sizeof(MyPoint));
		memcpy(multipoint->points, another->points, another->maxNum * sizeof(MyPoint));
		multipoint->numPoints = another->numPoints;
		multipoint->endNum = another->endNum;
		multipoint->maxNum = another->maxNum;
	}

	void InitMultipoint(MyMultiPoint* multipoint) {
		multipoint->points = (MyPoint*)malloc(MAX_POINT * sizeof(MyPoint));
		multipoint->numPoints = 0;
		multipoint->endNum = 0;
		multipoint->maxNum = MAX_POINT;
	}

	void AddPointToMultipoint(MyMultiPoint* multipoint, MyPoint point) {
		if (!multipoint->points) {
			InitMultipoint(multipoint);
		}
		if (multipoint->endNum == multipoint->maxNum && multipoint->numPoints != multipoint->endNum) {
			// 说明数组中间有空位，需要将后面的数据往前移动
			ScanMultipoint(multipoint);
		}

		if (multipoint->numPoints == multipoint->maxNum) {
			// 扩容
			multipoint->points = (MyPoint*)realloc(multipoint->points, (multipoint->maxNum + ADD_POINT) * sizeof(MyPoint));
			multipoint->maxNum += ADD_POINT;
		}
		multipoint->points[multipoint->endNum++] = point;
		multipoint->numPoints++;
	}

	void RemovePointFromMultipoint(MyMultiPoint* multipoint, MyPoint point) {
		// 从后往前查找第一个匹配的点
		for (int i = multipoint->endNum - 1; i >= 0; i--) {
			if (multipoint->points[i].x == point.x && multipoint->points[i].y == point.y) {
				multipoint->points[i] = INITMYPOINT;
				multipoint->numPoints--; // 更新点的数量
				break;
			}
		}
	}

	void RemoveLastPointM(MyMultiPoint* multipoint) {
		// 默认最后一个点是有效的
		if (multipoint->endNum > 0) {
			multipoint->numPoints--;
			multipoint->endNum--;
		}
	}

	void MovePointToM(MyMultiPoint* multipoint, MyPoint form, MyPoint to) {
		// 将点from移动到点to
		for (int i = multipoint->endNum - 1; i >= 0; i--) {
			if (multipoint->points[i].x == form.x && multipoint->points[i].y == form.y) {
				multipoint->points[i].x = to.x;
				multipoint->points[i].y = to.y;
				break;
			}
		}
	}

	size_t GetMultipointSize(MyMultiPoint* ml) {
		return sizeof(int) * 3 + sizeof(MyPoint) * ml->endNum;
	}

	void ClearMultipoint(MyMultiPoint* multipoint) {
		if (!multipoint) return;
		free(multipoint->points);
		multipoint->points = NULL;
		multipoint->numPoints = 0;
		multipoint->endNum = 0;
		multipoint->maxNum = MAX_POINT;
	}

	typedef struct DrawInfo {
		ImgType type;				// 图元类型
		DrawUnitProperty proper;	// 属性
		union {
			MyLine line;
			MyCircle circle;
			MyRectangle rectangle;
			MyMultiPoint multipoint;
		};
	} DrawInfo;

	void setDrawInfoType(DrawInfo* di, ImgType it) {
		di->type = it;
		switch (it) {
		case LINE:
		{
			di->line.start = INITMYPOINT;
		}
		break;
		case CIRCLE:
		{
			di->circle.center = INITMYPOINT;
		}
		break;
		case RECTANGLE:
		{
			di->rectangle.start = INITMYPOINT;
		}
		break;
		}
	}

	void MoveInfoBy(DrawInfo* draw, double x, double y) {
		switch (draw->type) {
		case LINE:
		{
			draw->line.start.x += x;
			draw->line.start.y += y;
			draw->line.end.x += x;
			draw->line.end.y += y;
			break;
		}
		case CIRCLE:
		{
			draw->circle.center.x += x;
			draw->circle.center.y += y;
			break;
		}
		case RECTANGLE:
		{
			draw->rectangle.start.x += x;
			draw->rectangle.start.y += y;
			draw->rectangle.end.x += x;
			draw->rectangle.end.y += y;
			break;
		}
		case CURVE:
		case BCURVE:
		case MULTILINE:
		case FMULTILINE:
		{
			for (int i = 0; i < draw->multipoint.endNum; i++) {
				if (!HFMyPoint(&(draw->multipoint.points[i]))) continue;
				draw->multipoint.points[i].x += x;
				draw->multipoint.points[i].y += y;
			}
			break;
		}
		default:
			break;
		}
	}

	void ClearDrawInfo(DrawInfo* drawing) {
		switch (drawing->type) {
		case CURVE:
		case BCURVE:
		case MULTILINE:
		case FMULTILINE:
		{
			if (drawing->multipoint.points) {
				ClearMultipoint(&(drawing->multipoint));
			}
			break;
		}
		default:
			break;
		}
	}

	typedef struct StoreImg {
		DrawInfo* img;
		int num;		// 图像的数量
		int endNum;             // 当前存储的最后一个数值在数组中的位置
		int maxNum;			 // 当前数组的最大容量
	} StoreImg;

	void InitStoreImg(StoreImg* store) {
		if (!store->img) {
			store->img = (DrawInfo*)malloc(MAX_IMG_NUM * sizeof(DrawInfo));
			store->num = 0;
			store->endNum = 0;
			store->maxNum = MAX_IMG_NUM;
		}

	}

	void ScanStoreImg(StoreImg* store) {
		// 已经无无效点
		if (store->endNum == store->num) return;
		int lastValid = 0;
		for (int i = 0; i < store->endNum; i++) {
			if (store->img[i].type == NONE) continue;
			store->img[lastValid++] = store->img[i]; // 复制有效项
	
		}
		store->endNum = lastValid; // 更新 endNum
		store->num = lastValid;    // 更新数量
	}

	void AddDrawInfoToStoreImg(StoreImg* store, DrawInfo draw) {
		if (!store->img) {
			InitStoreImg(store);
		}
		if (store->endNum == store->maxNum && store->num != store->endNum) {
			// 说明数组中间有空位，需要将后面的数据往前移动
			ScanStoreImg(store);
		}

		if (store->num == store->maxNum) {
			// 扩容
			store->img = (DrawInfo*)realloc(store->img, (store->maxNum + ADD_IMG_NUM) * sizeof(DrawInfo));
			store->maxNum += ADD_IMG_NUM;
		}
		store->img[store->endNum++] = draw;
		store->num++;
	}

	void RemoveDrawInfoFromStoreImg(StoreImg* store, int index) {
		if (store->img->type != NONE) {
			store->img->type = NONE;
		}
	}

	void MoveDrawInfoTo(StoreImg* store, DrawInfo form, DrawInfo to) {

	}

	void ClearStoreImg(StoreImg* store) {
		for (int i = 0; i < store->endNum; i++) {
			ClearDrawInfo(&(store->img[i]));
		}
		if (store->img) {
			free(store->img);
		}
		store->img = NULL;
		store->num = 0;
		store->endNum = 0;
		store->maxNum = MAX_IMG_NUM;
	}

	typedef unsigned char Byte;

	void IntToByte(int value, Byte** buffer, int* size) {
		memcpy(*buffer + *size, &value, sizeof(int));
		*size += sizeof(int);
	}

	void DoubleToByte(double value, Byte** buffer, int* size) {
		memcpy(*buffer + *size, &value, sizeof(double));
		*size += sizeof(double);
	}

	double byteToDouble(Byte* buffer, int* index) {
		double value;
		memcpy(&value, buffer + *index, sizeof(double));
		*index += sizeof(double);
		return value;
	}

	int byteToInt(Byte* buffer, int* index) {
		int value;
		memcpy(&value, buffer + *index, sizeof(int));
		*index += sizeof(int);
		return value;
	}

	void PointToBytes(MyPoint point, Byte** buffer, int* size) {
		DoubleToByte(point.x, buffer, size); // Convert x
		DoubleToByte(point.y, buffer, size); // Convert y
	}

	MyPoint BytesToPoint(Byte* buffer, int* index) {
		MyPoint point;
		point.x = byteToDouble(buffer, index);
		point.y = byteToDouble(buffer, index);
		return point;
	}

	void PropertyToByte(const DrawUnitProperty* pro, Byte** buffer, int* index) {
		IntToByte(pro->color, buffer, index);
		IntToByte(pro->bgcolor, buffer, index);
		IntToByte(pro->width, buffer, index);
		IntToByte(pro->type, buffer, index);
	}

	DrawUnitProperty ByteToProperty(Byte* buffer, int* size) {
		DrawUnitProperty dup;
		dup.color = byteToInt(buffer, size);
		dup.bgcolor = byteToInt(buffer, size);
		dup.width = byteToInt(buffer, size);
		dup.type = (gctype)byteToInt(buffer, size);

		return dup;
	}

	// 获取绘制信息的大小
	size_t GetDrawInfoSize(DrawInfo* di) {
		size_t size = 0;
		// 标识符长度
		size += sizeof(ImgType);
		size += sizeof(DrawUnitProperty);
		switch (di->type) {
		case LINE:
			size += sizeof(MyLine);
			break;
		case CIRCLE:
			size += sizeof(MyCircle);
			break;
		case RECTANGLE:
			size += sizeof(MyRectangle);
			break;
		case CURVE:
		case BCURVE:
		case MULTILINE:
		case FMULTILINE:
			ScanMultipoint(&di->multipoint);
			size += GetMultipointSize(&(di->multipoint));
			break;
		default:
			break;
		}
		return size;
	}

	void InitHeader(ImgType type, Byte** buffer, int* size) {
		(*buffer)[*size] = type;
		*size += sizeof(ImgType);
		return;
	}
	
	ImgType ByteToImgType(Byte* buffer, int* index) {
		ImgType it =  (ImgType)buffer[(*index)]; // 读取图像类型标识
		(*index) += sizeof(ImgType);
		return it;
	}

	Byte* DrawInfoToBytes(DrawInfo draw, Byte* buffer, int* byteSize) {
		InitHeader(draw.type, &buffer, byteSize);
		PropertyToByte(&(draw.proper), &buffer, byteSize);

		switch (draw.type) {
		case LINE:
			PointToBytes(draw.line.start, &buffer, byteSize); // Add start point
			PointToBytes(draw.line.end, &buffer, byteSize);   // Add end point
			break;

		case CIRCLE:
			PointToBytes(draw.circle.center, &buffer, byteSize); // Add center point
			DoubleToByte(draw.circle.radius, &buffer, byteSize);   // Add radius
			break;

		case RECTANGLE:
			PointToBytes(draw.rectangle.start, &buffer, byteSize); // Add start point
			PointToBytes(draw.rectangle.end, &buffer, byteSize);   // Add end point
			break;

		case CURVE:
		case BCURVE:
		case MULTILINE:
		case FMULTILINE:
			IntToByte(draw.multipoint.numPoints, &buffer, byteSize); // Add number of points
			IntToByte(draw.multipoint.endNum, &buffer, byteSize); // Add endNum
			IntToByte(draw.multipoint.maxNum, &buffer, byteSize); // Add maxNum
			for (int i = 0; i < draw.multipoint.endNum; i++) {
				PointToBytes(draw.multipoint.points[i], &buffer, byteSize); // Add each point
			}
			break;

		default:
			break;
		}

		return buffer; // Return the Byte array
	}

	DrawInfo BytesToDrawInfo(Byte* buffer, int* index) {
		DrawInfo draw;
		draw.type = ByteToImgType(buffer, index); // 读取图像类型标识
		draw.proper = ByteToProperty(buffer, index); // 读取属性

		switch (draw.type) {
		case LINE:
			draw.line.start = BytesToPoint(buffer, index);
			draw.line.end = BytesToPoint(buffer, index);
			break;

		case CIRCLE:
			draw.circle.center = BytesToPoint(buffer, index);
			draw.circle.radius = byteToDouble(buffer, index);
			break;

		case RECTANGLE:
			draw.rectangle.start = BytesToPoint(buffer, index);
			draw.rectangle.end = BytesToPoint(buffer, index);
			break;

		case CURVE:
		case BCURVE:
		case MULTILINE:
		case FMULTILINE:
			draw.multipoint.numPoints = byteToInt(buffer, index);
			draw.multipoint.endNum = byteToInt(buffer, index);
			draw.multipoint.maxNum = byteToInt(buffer, index);
			draw.multipoint.points = (MyPoint*)malloc(draw.multipoint.numPoints * sizeof(MyPoint));
			for (int i = 0; i < draw.multipoint.endNum; i++) {
				draw.multipoint.points[i] = BytesToPoint(buffer, index);
			}
			break;
		default:
			break;
		}

		return draw;
	}

	typedef enum FileOpenAndSave {
		DIALOGOPENFAILE,
		FILEOPENFAILE,
		FILEVERSIONINVALID,
		FILEHEADERINVALID,
		MEMORRYALLOCFAIL,
		OPENOK,
		SAVEOK
	} FileOpenAndSave;

	FileOpenAndSave StoreImgToFile(StoreImg* store, const wchar_t* filename) {
		FILE* file;
		if (_wfopen_s(&file, filename, L"wb") != 0) {
			return FILEOPENFAILE;
		}

		// 写入GTX文件头
		fwrite(IMG_HEADER, sizeof(char), FILEHEADERL - 1, file);
		fputc(0, file);

		// 写入文件版本
		fwrite(FILE_VERSION, sizeof(char), FILEVERSIONL, file);

		// 写入图像数量
		fwrite(&store->num, sizeof(int), 1, file);

		// 写入maxNum
		fwrite(&store->maxNum, sizeof(int), 1, file);

		// 写入endNum
		fwrite(&store->endNum, sizeof(int), 1, file);

		// 写入每个图像
		for (int i = 0; i < store->num; i++) {
			int byteSize = 0;
			size_t needSize = GetDrawInfoSize(&(store->img[i]));
			Byte* data = (Byte* )malloc(needSize);
			if (!data) return MEMORRYALLOCFAIL;
			Byte* temp = data;
			DrawInfoToBytes(store->img[i], temp, &byteSize);
			fwrite(data, sizeof(Byte), byteSize, file);
			assert(needSize == byteSize);
			free(data); // 释放字节数组
		}

		fclose(file);
		return SAVEOK;
	}

	FileOpenAndSave FileToStoreImg(StoreImg* store, const wchar_t* filename) {
		FILE* file;
		if (_wfopen_s(&file, filename, L"rb") != 0) {
			return FILEOPENFAILE;
		}

		// 读取GTX文件头
		char header[FILEHEADERL] = { 0 };
		fread(header, sizeof(char), FILEHEADERL, file);
		if (strcmp(header, IMG_HEADER) != 0) {
			fclose(file);
			return FILEHEADERINVALID;
		}

		// 读取文件版本
		char version[FILEVERSIONL+1] = { 0 };
		fread(version, sizeof(char), FILEVERSIONL, file);
		if (strcmp(version, FILE_VERSION) != 0) {
			fclose(file);
			return FILEVERSIONINVALID;
		}

		// 不为空则清空
		if (store->endNum > 0) {
			ClearStoreImg(store);
		}

		// 读取num
		fread(&store->num, sizeof(int), 1, file);
		// 读取maxNum
		fread(&store->maxNum, sizeof(int), 1, file);
		// 读取endNum
		fread(&store->endNum, sizeof(int), 1, file);

		store->img = (DrawInfo*)malloc(store->maxNum * sizeof(DrawInfo));

		if (!store->img) {
			fclose(file);
			return MEMORRYALLOCFAIL;
		}
		// 获取文件大小
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		long fileHeaderLength = FILEHEADERL + FILEVERSIONL + sizeof(int) * 3;
		fseek(file, fileHeaderLength, SEEK_SET);
		long imgSize = fileSize - ftell(file);
		Byte* buffer = (Byte*)malloc(imgSize * sizeof(Byte));
		int index = 0;
		// 读取剩余数据
		fread(buffer, imgSize, 1, file);
		// 读取每个图像
		for (int i = 0; i < store->endNum; i++) {
			store->img[i] = BytesToDrawInfo(buffer, &index);
		}
		free(buffer);
		fclose(file);
		return OPENOK;
	}

#ifdef __cplusplus
}
#endif

#endif // !DRAWINFO_H