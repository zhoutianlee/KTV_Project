
#include "App.h"
#include "FileApp.h"
#include "Server.h"
#include "NetworkDHCP.H"
#include "bsp_time.h"
#include "ArtNet.h"
#include "Display.h"
#include "DmxOut.h"
//static void timeUp(int id);
//static long count = 0;

void uiTest(void);

void initApp(void)
{
//	  TimeHandle*handle;

	
	
	initNetworkDHCP((void*)0);
	initServer((void*)0);
//		initFileApp((void*)0);
	initArtNet((void*)0);
	
//	uiTest();
//	  handle = createTime(1,0,getTimeTick(0,500,0),timeUp);
//	  startTime(handle);
//	  handle = createTime(1,1,getTimeTick(0,300,0),timeUp);
//	  startTime(handle);
//	  if(handle != NULL){
//			printf("handle is not NULL ,timeUP setting\r\n");
//		}
}

void uiTest(void)
{
	Triangle triangle;
	Point p0 = {0,0};
	Point p1 = {15,7};
	Point p2 = {4,20};
	triangle.a = p0;
	triangle.b = p1;
	triangle.c = p2;
	Paint paint;
	paint.size  =1;
	paint.background.rgb.red = 0x00;
	paint.background.rgb.green = 0x00;
	paint.background.rgb.blue  = 0x00;
	paint.color.rgb.red = 0xff;
	paint.color.rgb.green = 0x00;
	paint.color.rgb.blue  = 0x00;
	setPaint(&paint);
	
	pArray array = createArray(50,50,DATATYPE_RGB);
	paint.background.rgb.red = 0xff;
	clearScreen(array);
	showArray(array);
	paint.background.rgb.red = 0x00;
	printf("Çå³ý¾ØÐÎÇøÓò\r\n");
	clearArea(array,3,12,0,8);
	showArray(array);
	clearScreen(array);
	drawLine(array,p0,p1);
	showArray(array);
	printf("draw triangle\r\n");
	clearScreen(array);
	drawTriangle(array,&triangle);
	showArray(array);
	printf("draw xline\r\n");
	p1.width = 15;
	p1.heigh = 0;
	drawLine(array,p0,p1);
	showArray(array);
	printf("draw yline\r\n");
	p0.width = 15;
	p0.heigh =30;
	drawLine(array,p0,p1);
	showArray(array);
	
	printf("draw screen move width 2========================\r\n");
	moveScreenWidth(array,2);
	showArray(array);
	printf("draw screen move width -2========================\r\n");
	moveScreenWidth(array,-2);
	showArray(array);
	printf("draw screen move heigh 2========================\r\n");
	moveScreenHeigh(array,2);
	showArray(array);
	printf("draw screen move heigh -2========================\r\n");
	moveScreenHeigh(array,-2);
	showArray(array);
	
	clearScreen(array);
	drawCircleXY(array,25,25,15);
	showArray(array);
	drawCircleXY(array,25,25,20);
	showArray(array);
	
	clearScreen(array);
	drawCircleFullXY(array,15,15,8);
	showArray(array);
}
//static void timeUp(int id)
//{
//		printf("id = %d\ttime Up ,count = %d\r\n",id,count++);
//}
