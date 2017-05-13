

#ifndef __DISPLAY__H
#define __DISPLAY__H

#include "ArtNetType.h"
#include "DMXOut.h"


typedef struct Triangle{
	Point				a;
	Point 			b;
	Point 			c;
}Triangle,*pTriangle;

typedef struct Paint{
	Int8				size;									// 画笔大小
	DataType		color;								// 颜色
	DataType		background;						// 背景色
}Paint,*pPaint;

void    setPaint(pPaint	paint);
pPaint  getPaint(void);
void    clearScreen(pArray array);
void    clearLine(pLineNode line);
void    adjectArrayLight(pArray array,float k);									// 调节整体亮度
void    adjectLineLight(pLineNode line,float k);								// 调节整体亮度
void    clearArea(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd);// 区域擦除
void		drawLine(pArray array,Point p0,Point p1);								// 绘制直线
void    drawLineXY(pArray array,Int16 x0,Int16 y0,Int16 x1,Int16 y1);
void    drawTriangle(pArray array,pTriangle	triangle);					// 绘制三角形
void    drawTriangleFull(pArray array,pTriangle triangle);			// 绘制实心三角形
void		drawCircle(pArray array,Point p0,Int16 R);							// 绘制圆形
void    drawCircleXY(pArray array,Int16 x,Int16 y,Int16 R);
void    drawCircleFull(pArray array,Point p0,Int16 R);					// 绘制实心圆
void    drawCircleFullXY(pArray array,Int16 x,Int16 y,Int16 R);
void    drawPixelObject(pArray array,Point p0);									// 绘制点
void    drawPixel(pArray array,Int16 x,Int16 y);								// 绘制点
void    showArray(pArray array);																// 用串口模拟
void    moveScreen(pArray array,int row,int column);						// 屏移动
void    moveScreenWidth(pArray array,int width);								// 屏移
void    moveLine(pLineNode pline,int number);										// 可以正向移动可以反向移动
void    moveScreenHeigh(pArray array,int heigh);								// 屏移动
void    moveLineRGB(pRGB pData,Int16 len,int number);						// 总长度及移动,可以搭配getPixelLine和setPixelLine一起使用

#endif
