

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
	Int8				size;									// ���ʴ�С
	DataType		color;								// ��ɫ
	DataType		background;						// ����ɫ
}Paint,*pPaint;

void    setPaint(pPaint	paint);
pPaint  getPaint(void);
void    clearScreen(pArray array);
void    clearLine(pLineNode line);
void    adjectArrayLight(pArray array,float k);									// ������������
void    adjectLineLight(pLineNode line,float k);								// ������������
void    clearArea(pArray array,Int16 xStart,Int16 xEnd,Int16 yStart,Int16 yEnd);// �������
void		drawLine(pArray array,Point p0,Point p1);								// ����ֱ��
void    drawLineXY(pArray array,Int16 x0,Int16 y0,Int16 x1,Int16 y1);
void    drawTriangle(pArray array,pTriangle	triangle);					// ����������
void    drawTriangleFull(pArray array,pTriangle triangle);			// ����ʵ��������
void		drawCircle(pArray array,Point p0,Int16 R);							// ����Բ��
void    drawCircleXY(pArray array,Int16 x,Int16 y,Int16 R);
void    drawCircleFull(pArray array,Point p0,Int16 R);					// ����ʵ��Բ
void    drawCircleFullXY(pArray array,Int16 x,Int16 y,Int16 R);
void    drawPixelObject(pArray array,Point p0);									// ���Ƶ�
void    drawPixel(pArray array,Int16 x,Int16 y);								// ���Ƶ�
void    showArray(pArray array);																// �ô���ģ��
void    moveScreen(pArray array,int row,int column);						// ���ƶ�
void    moveScreenWidth(pArray array,int width);								// ����
void    moveLine(pLineNode pline,int number);										// ���������ƶ����Է����ƶ�
void    moveScreenHeigh(pArray array,int heigh);								// ���ƶ�
void    moveLineRGB(pRGB pData,Int16 len,int number);						// �ܳ��ȼ��ƶ�,���Դ���getPixelLine��setPixelLineһ��ʹ��

#endif