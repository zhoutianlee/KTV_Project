

#include "Model_Techo_DubStep.h"
#include "bsp.h"
#include "bsp_sound.h"
#include "stdio.h"
#include "arm_math.h"
#include "stm32_dsp.h"
#include "Mathbase.h"
#include "sinTable.h"

typedef struct TimeClock{

	Int16   x0;
	Int16   y0;
	Int8    count;
	Int8    status;
}TimeClock,*pTimeClock;

#define DROWN_F			0.20F
#define T 					512
#define T1					(T*3/2)
#define T2					(T/2)
#define T3		 			T
#define T4 		 			T1
#define WATERSIZE 	sizeof(waterLine)/sizeof(Int8)
	
const static Int8   waterLine[] = {250,200,170,130,100,50,25,10,5,0,0,0,0,0,0};

static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static void runSoundCB(int16_t*pdata,int16_t len);
static void calculateColor(void);
static void calculateScreen(pArray array,pTimeClock clock);

static Int16 maxDimmer;										//	最大亮度
static float Dimmer;											// 亮度系数
static Int16 ColorCount;																		// 计数值
static Paint paint;
static char  status;
static char  DarkLock = 0;
static char  WaterCount;
static TimeClock timeClock0;

extern RGB   RGBArray[80];
extern RGB   backupRGB[80];

static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
};

pModel getModelTecho_DubStep(void)
{
	return (pModel)&model;
}

static void init (void)
{
	
}

static void setting(int mode,int data)
{
	setSoundCB(runSoundCB);
	setSound(1);
}

static void setUp(void)
{
	maxDimmer = 2048;
	Dimmer    = 1.0f;
	ColorCount = 0;
	setPaint(&paint);																	// 设置画笔
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	setSoundCB(runSoundCB);
	setSound(1);
	DarkLock = 0;
	pRGB pdest = backupRGB;
	for(int i =0;i<80;i++){
		pdest->red = 0;
		pdest->green = 0;
		pdest->blue = 0;
	}
	WaterCount = 0;
	
	timeClock0.x0 = 0;
	timeClock0.y0 = 0;
	timeClock0.count = 0;
	timeClock0.status = 0;
	
}

static void run  (pNodeList pnode,void*paramter)
{
	
	static char count = 0;
	pArray 		 array;
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	pPixelHead floorHead,diamondHead;
	int        i;
	
	calculateColor();																	// 计算当前颜色值
	
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	
	moveLineRGB(RGBArray,80,1);
	if(++count >= WATERSIZE){
		count =0;
	}
	
	if(DarkLock !=0){
		for(i=0;i<floorHead->heigh;i++){
			setPixelLine(floorHead,i,backupRGB);
		}
//		for(i=0;i<diamondHead->heigh;i++){
//			setPixelLine(diamondHead,i,backupRGB);
//		}
	}else{
		for(i=0;i<floorHead->heigh;i++){
			setPixelLine(floorHead,i,RGBArray);
		}
//		for(i=0;i<diamondHead->heigh;i++){
//			setPixelLine(diamondHead,i,RGBArray);
//		}
	}

	array	= getArrayIndex(0);
	
	calculateScreen(array,&timeClock0);
	
	if(DarkLock ==0){
		while(plist){
			
			punion = plist->pHead;
			if(plist->nodeID == floorHead->nodeID){plist= plist->pNext;continue;}
			if(plist->mode == ARTNETMODE_ARRAY){
				// 只需要调用清屏程序
	//			clearScreen(plist->parray);
			}else{
				while(punion){
					moveLine(&punion->lineNode,1);
					punion->lineNode.Data[0].rgb.red = waterLine[count];
					punion->lineNode.Data[0].rgb.green = waterLine[count];
					punion->lineNode.Data[0].rgb.blue = waterLine[count];
	//				adjectLineLight(&punion->lineNode,Dimmer);
	//				clearLine(&punion->lineNode);
					punion = (pUnionNode)punion->arrayNode.pNext;
				}
			}
			plist = plist->pNext;
		}
	}else{
		while(plist){
			
			punion = plist->pHead;
			if(plist->nodeID == floorHead->nodeID||plist->nodeID==diamondHead->nodeID){plist= plist->pNext;continue;}
			if(plist->mode == ARTNETMODE_ARRAY){
				// 只需要调用清屏程序
	//			clearScreen(plist->parray);
			}else{
				while(punion){
//					moveLine(&punion->lineNode,1);
//					punion->lineNode.Data[0].rgb.red = waterLine[count];
//					punion->lineNode.Data[0].rgb.green = waterLine[count];
//					punion->lineNode.Data[0].rgb.blue = waterLine[count];
	//				adjectLineLight(&punion->lineNode,Dimmer);
					clearLine(&punion->lineNode);
					punion = (pUnionNode)punion->arrayNode.pNext;
				}
			}
			plist = plist->pNext;
		}
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
	setSound(0);
}

static void runSoundCB(int16_t*pdata,int16_t len)
{
	Int16 max = getMax(pdata,len);
	Dimmer       = ((float)max)/((float)maxDimmer);
	printf("maxDimmer = %d max = %d Dimmer = %f line = %d\r\n",maxDimmer,max,Dimmer,__LINE__);
	maxDimmer		 = max;
	
}

static void calculateColor(void)
{
	Int16 r,g,b;

	if(DarkLock!=0){
		paint.background.rgb .red   = 0;
		paint.background.rgb .blue  = 0;
		paint.background.rgb .green = 0;
		DarkLock --;
		return;
	}
	if(ColorCount > T1){
		ColorCount = 0;
//		status  ++;
	}
	if(ColorCount <= T2){
		paint.background.rgb .red = getSinValue(ColorCount);
		paint.background.rgb .green = 0;
		paint.background.rgb .blue = 255 - paint.background.rgb .red;
	}else if(ColorCount <= T3){
		paint.background.rgb .red = getSinValue(ColorCount);
		paint.background.rgb .green = 255 - paint.background.rgb .red;
		paint.background.rgb .blue  = 0;
	}else if(ColorCount <= T4){
		paint.background.rgb .red   = 0;
		paint.background.rgb .blue  = getSinValue(ColorCount - T);
		paint.background.rgb .green = 255 - paint.background.rgb .blue;
	}
	
	// 防止过度饱和
	r = paint.background.rgb .red   * Dimmer;
	g = paint.background.rgb .blue  * Dimmer;
	b = paint.background.rgb .green * Dimmer;
	if(maxDimmer > 500){
		if(r > 255||g>255||b>255){
			paint.background.rgb .red   = 255;
			paint.background.rgb .blue  = 255;
			paint.background.rgb .green = 255;
			status ++;
			DarkLock = 3;
		}
	}
//	if(g > 255){paint.background.rgb .blue  = 255;}
//	if(b > 255)paint.background.rgb .green = 255;
	
	ColorCount ++;
	Dimmer = 1;

	switch(status){
		case 0:{
			RGBArray[0].red = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
			RGBArray[0].green = 0;
			RGBArray[0].blue = 0;
		}break;
		case 1:{
			RGBArray[0].red = waterLine[WaterCount];;
			RGBArray[0].green = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
			RGBArray[0].blue = 0;
		}break;
		case 2:{
			RGBArray[0].red = 0;
			RGBArray[0].green = waterLine[WaterCount];
			RGBArray[0].blue = 0;//getSinValue(ColorCount)*Dimmer;
		}break;
		case 3:{
			RGBArray[0].red = 0;//getSinValue(ColorCount)*Dimmer;
			RGBArray[0].green = waterLine[WaterCount];
			RGBArray[0].blue = waterLine[WaterCount];
		}break;
		case 4:{
			RGBArray[0].red = 0;//getSinValue(ColorCount)*Dimmer;
			RGBArray[0].green = 0;
			RGBArray[0].blue = waterLine[WaterCount];
		}break;
		case 5:{
			RGBArray[0].red = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
			RGBArray[0].green = 0;
			RGBArray[0].blue = waterLine[WaterCount];
		}break;
		default:{
			status = 0;
		}
	}
	WaterCount++;
	if(++WaterCount>= WATERSIZE){
		WaterCount = 0;
	}
}

static void calculateScreen(pArray array,pTimeClock clock)
{
	Int16 x1,y1;
	Paint backupPaint;
	x1 = array->width -clock->x0;
	y1 = array->heigh -clock->y0;
	backupPaint.background.rgbw = (RGBW){0,0,0,0};
	backupPaint.color      = paint.background;
	setPaint(&backupPaint);
	clearScreen(array);
	drawLineXY(array,clock->x0,clock->y0,11-clock->x0,5-clock->y0);											// 绘制直线
	drawLineXY(array,12 + clock->x0,clock->y0,23-clock->x0,5-clock->y0);											// 绘制直线
	
//	drawLineXY(array,5-clock->y0,clock->x0,6+clock->y0,5-clock->x0);											// 绘制直线
//	drawLineXY(array,17-clock->y0,clock->x0,18+clock->y0,5-clock->x0);											// 绘制直线
	// 绘制相对于第一条的交叉线
//	drawLineXY(array,clock->y0,clock->y0,11-clock->x0,11-clock->y0);											// 绘制直线
//	drawLineXY(array,clock->x0,clock->y0,11-clock->x0,11-clock->y0);											// 绘制直线
//	drawLineXY(array,clock->x0,clock->y0,x1,y1);											// 绘制直线
	if(++clock->count >=1){
		clock->count = 0;
		switch(clock->status){
			case 0:if(++clock->x0>= 11)clock->status++;break;
			case 1:if(++clock->y0>= 5)clock->status++;break;
			case 2:if(--clock->x0==0)clock->status++;break;
			case 3:if(--clock->y0==0)clock->status=0;break;
		}
	}else{
		
	}
	setPaint(&paint);
	
}
