

#include "Model_Normal_Soft.h"
#include "stdio.h"
#define RED_DLT							17
#define BLUE_DLT						29
#define GREEN_DLT						43
#define WHITE_DLT						7
#define RGBARRAY_SIZE 			sizeof(rgbArray1)/sizeof(RGB)
	
// 颜色位纯白流星雨
typedef struct TimeClock{

	Int16   x0;
	Int16   y0;
	Int8    count;
	Int8    status;
}TimeClock,*pTimeClock;

static RGB rgbArray[]={{255,255,255},{200,200,200},{150,150,150},{100,100,100},{50,50,50},
												{25,25,25},{12,12,12},{6,6,6},{0,0,0},{0,0,0},
												{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
static RGB rgbArray1[]={{255,255,255},{200,200,200},{150,150,150},{100,100,100},{50,50,50},
												{25,25,25},{12,12,12},{6,6,6},{0,0,0},{0,0,0},
												{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
//{{125,0,0},{125,0,0},{125,0,0},{125,0,0},{0,125,0},
//												{0,125,0},{0,125,0},{0,125,0},{0,125,0},{0,125,0},
//												{0,0,125},{0,0,125},{0,0,125},{0,0,125},{0,0,125}};
	
static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void calculateScreen(pArray array,pTimeClock clock);
static void calculatePoint(pArray p);
static void destroy(void);
static Paint paint;
static Paint paint1;
static TimeClock timeClock[5];
static char arrayCount=0;
static TimeClock array[3];
static unsigned char Dimmer;
static unsigned char Color;
												
extern RGB   RGBArray[80];
	
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
};

pModel getModelNormal_Soft(void)
{
	return (pModel)&model;
}

static void init (void)
{
	paint.background.rgbw.red   = 0;
	paint.background.rgbw.green = 0;
	paint.background.rgbw.blue  = 0;
	paint.background.rgbw.white = 0;
	
	paint.color.rgbw.red   = 0xff;
	paint.color.rgbw.blue  = 0;
	paint.color.rgbw.green = 0;
	paint.color.rgbw.white = 0;
	
	paint1.background.rgbw.red   = 0;
	paint1.background.rgbw.green = 0;
	paint1.background.rgbw.blue  = 0;
	paint1.background.rgbw.white = 0;
	
	paint1.color.rgbw.red   = 0xff;
	paint1.color.rgbw.blue  = 0;
	paint1.color.rgbw.green = 0;
	paint1.color.rgbw.white = 0;
	
	timeClock[0].x0 = 0;
	timeClock[0].y0 = 0;
	timeClock[0].count = 0;
	timeClock[0].status = 0;
	
	timeClock[1].x0 = 0;
	timeClock[1].y0 = 0;
	timeClock[1].count = ARTNET_FREQFLASH>>8;
	timeClock[1].status = 0;
	
	timeClock[2].x0 = 0;
	timeClock[2].y0 = 0;
	timeClock[2].count = ARTNET_FREQFLASH>>4;
	timeClock[2].status = 0;
	
	timeClock[3].x0 = 0;
	timeClock[3].y0 = 0;
	timeClock[3].count = ARTNET_FREQFLASH>>2;
	timeClock[3].status = 0;
	
	timeClock[4].x0 = 0;
	timeClock[4].y0 = 0;
	timeClock[4].count = ARTNET_FREQFLASH>>2;
	timeClock[4].status = 0;
	arrayCount = 0;
	array[0].x0 = 0;
	array[0].y0 = 0;
	array[0].count =0;
	array[0].status = 0;
	
	array[1].x0 = 80;
	array[1].y0 = 0;
	array[1].count =0;
	array[1].status = 1;
	
	array[2].x0 = 0;
	array[2].y0 = 60;
	array[2].count =0;
	array[2].status = 4;
	
	
}

static void setting(int mode,int data)
{
	if(mode == MODELDATA_COLOR){
		Color  = data;
	}else if(mode == MODELDATA_DIMMER){
		Dimmer = 4-data;
		printf("Dimmer = %d\r\n",Dimmer);
	}
}

static void setUp(void)
{
	setPaint(&paint);
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	Dimmer = 0;
	
}

static void run  (pNodeList pnode,void*paramter)
{
	
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	Paint      paint2;
	Int16      len;
	static    int count =0;
	static    char flag = 0;
	int        i;
	pArray 		array;
	pPixelHead floorHead,diamondHead;
	*((Int32*)&paint.background )+= 3;
	if(*((Int32*)&paint.background)>= 0xffffff){
		*((Int32*)&paint.background) -= 0xffffff;
	}

//	setPaint(&paint1);
//	array	= getArrayIndex(0);clearScreen(array);calculateScreen(array,&timeClock[0]);return;drawLineXY(array,0,40,80,40);
	if(flag == 0){
	setPaint(&paint1);
	array	= getArrayIndex(0);clearScreen(array);calculateScreen(array,&timeClock[0]);//drawLineXY(array,0,40,80,40);//drawLineXY(array,0,60,80,0);//calculateScreen(array,&timeClock[0]);///drawLineXY(array,0,0,6,6);
	array	= getArrayIndex(1);clearScreen(array);calculateScreen(array,&timeClock[1]);//drawLineXY(array,0,6,6,0);
//	array	= getArrayIndex(2);clearScreen(array);calculateScreen(array,&timeClock[2]);//drawLineXY(array,0,0,6,6);
//	array	= getArrayIndex(3);clearScreen(array);calculateScreen(array,&timeClock[3]);//drawLineXY(array,0,6,6,0);
//	
//	array = getArrayIndex(4);clearScreen(array);calculateScreen(array,&timeClock[4]);//drawCircleXY(array,30,30,15);  // 绘制圆
//  array	= getArrayIndex(0);clearScreen(array);
//	calculatePoint(array);
	  drawCircleXY(array,30,30,15);  // 绘制圆
		flag = 1;return;
	}
	flag = 0;

	setPaint(&paint);
	
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	
	getPixelLine(floorHead,0,RGBArray);
	moveLineRGB(RGBArray,floorHead->width,1);
	RGBArray[0].red = rgbArray1[arrayCount].red>>Dimmer;
	RGBArray[0].green = rgbArray1[arrayCount].green>>Dimmer;
	RGBArray[0].blue = rgbArray1[arrayCount].blue>>Dimmer;
	for(i = 0;i < floorHead->heigh;i++){
		setPixelLine(floorHead,i,RGBArray);
	}
	getPixelLine(diamondHead,0,RGBArray);
	moveLineRGB(RGBArray,diamondHead->width,3);
	RGBArray[0].red = rgbArray1[arrayCount].red>>Dimmer;
	RGBArray[0].green = rgbArray1[arrayCount].green>>Dimmer;
	RGBArray[0].blue = rgbArray1[arrayCount].blue>>Dimmer;
	for(i = 0;i < diamondHead->heigh;i++){
//		RGBArray[0] = rgbArray1[arrayCount];
		setPixelLine(diamondHead,i,RGBArray);
	}
	
	paint2.background.rgbw = (RGBW){0,0,0,0};
	setPaint(&paint2);
	while(plist){
		
		punion = plist->pHead;
		if(plist->nodeID == floorHead->nodeID||plist->nodeID==diamondHead->nodeID){plist= plist->pNext;continue;}
		if(plist->mode == ARTNETMODE_ARRAY){
			punion = (pUnionNode)punion ->arrayNode.pNext;
//			while(punion){
//				if(punion->lineNode.msg.isExit == 0){punion = (pUnionNode)punion ->arrayNode.pNext;continue;}
//				clearScreen(plist->parray);								// 清除屏幕
//				punion = (pUnionNode)punion ->arrayNode.pNext;
//			}
			
		}else{
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				// 
				// 此处用于改变颜色
//				clearLine(&punion->lineNode);
				moveLine(&punion->lineNode,1);
				punion->lineNode.Data->rgb.red = rgbArray[arrayCount].red>>Dimmer;
				punion->lineNode.Data->rgb.green = rgbArray[arrayCount].green>>Dimmer;
				punion->lineNode.Data->rgb.blue = rgbArray[arrayCount].blue>>Dimmer;
				punion = (pUnionNode)punion->arrayNode.pNext;
			}
		}
		plist = plist->pNext;
	}
	if(++arrayCount >= RGBARRAY_SIZE){
		arrayCount =0;
	}
	
}

static char isEnd(pNodeList pnode,void*paramter)
{
	return 0;
}

static void end  (pNodeList pnode,void*paramter)
{
	
}

static void destroy(void)
{

}


static void calculateScreen(pArray array,pTimeClock clock)
{
	Int16 x1,y1;
	x1 = array->width -clock->x0;
	y1 = array->heigh -clock->y0;
	drawLineXY(array,clock->x0,clock->y0,x1,y1);											// 绘制直线
	if(++clock->count >=1){
		clock->count = 0;
		switch(clock->status){
			case 0:if(++clock->x0>= array->width)clock->status++;break;
			case 1:if(++clock->y0>= array->heigh)clock->status++;break;
			case 2:if(--clock->x0==0)clock->status++;break;
			case 3:if(--clock->y0==0)clock->status=0;break;
		}
	}else{
		
	}
	
}

static void calculatePoint(pArray p)
{
	
	Triangle triangle;
	int i;
	triangle.a.width = array[0].x0;
	triangle.a.heigh = array[0].y0;
	
	triangle.b.width = array[1].x0;
	triangle.b.heigh = array[1].y0;
	
	triangle.c.width = array[2].x0;
	triangle.c.heigh = array[2].y0;
	
	drawTriangle(p,&triangle);
	
	for(i=0;i<3;i++){
		switch(array[i].status){
			case 0:if(++array[i].x0>=p->width)array[i].status++;break;
			case 1:if(++array[i].y0>=p->heigh)array[i].status++;break;
			case 2:if(--array[i].x0==0)array[i].status++;break;
			case 3:if(--array[i].y0==0)array[i].status=0;break;
		}
	}
}

