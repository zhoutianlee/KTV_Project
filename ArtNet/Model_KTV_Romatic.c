

#include "Model_Ktv_Romatic.h"
#include "stdio.h"
#include "SinTable.h"

#define T 		512
#define T1		(T*3/2)
#define T2		(T/2)
#define T3		 T
#define T4 		 T1

static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static void calculateColor(void);

static Int8 array[][2] = {{220,30},{150,30},{100,15},{50,10}};								// 是个等级
static Int8 max;																			// 最大数字
static Int8 base;																			// 基础阈值
static Int8 layer;																		// 等级
static Int16 count;																		// 计数值
static Paint paint;
static Int16 turn;
static int   turnflag;
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

pModel getModelKTV_Romatic(void)
{
	return (pModel)&model;
}

static void init (void)
{
	layer = 0;
	turn  = 0;
	max   = array[layer][0];
	base  = array[layer][1];
	turnflag = 0;
}

static void setting(int mode,int data)
{
	
}

static void setUp(void)
{
	paint.background.rgbw = (RGBW){0,0,0,0};
	setPaint(&paint);
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,0);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	turnflag = 1;
}

static void run  (pNodeList pnode,void*paramter)
{
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	pPixelHead floorHead,diamondHead;
	// 进行颜色计算
	calculateColor();
	
	floorHead   = getFloorHead();
	int i =0;
	if(turnflag >0){
		getPixelLine(floorHead,0,RGBArray);
		moveLineRGB(RGBArray,floorHead->width,turnflag*2);
		RGBArray[0] = paint.color.rgb;
		for(i = 0;i < floorHead->heigh;i++){
			setPixelLine(floorHead,i,RGBArray);
		}
	}else{
		getPixelLine(floorHead,0,RGBArray);
		moveLineRGB(RGBArray,floorHead->width,turnflag*2);
		RGBArray[floorHead->width-1] = paint.color.rgb;
		for(i = 0;i < floorHead->heigh;i++){

			setPixelLine(floorHead,i,RGBArray);
		}
	}
	// 线进行颜色整体的变化
	while(plist){
		punion = plist->pHead;
		
		if(plist->nodeID == floorHead->nodeID){plist= plist->pNext;continue;}
		if(plist->mode == ARTNETMODE_LINE){
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				// 整体颜色渐变
				// 此处用于改变颜色
				moveLine(&punion->lineNode,turnflag*2);
				if(turnflag==1)*(pRGB)punion->lineNode.Data = paint.color.rgb;//[arrayCount];
				else *((pRGB)punion->lineNode.Data  + punion->lineNode.len-1) = paint.color.rgb;
				punion = (pUnionNode)punion->arrayNode.pNext;
			}
		}
		
		plist = plist->pNext;
	}
	// 计算变化值，用于改变方向
	if( ++turn >= 50){
		if(turnflag == 1) turnflag =-1;
		else turnflag = 1;
		turn = 0;
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

static void calculateColor(void)
{
	if(count > T1){
		count = 0;
	}
	if(count <= T2){
		paint.color.rgb .red = getSinValue(count);
		paint.color.rgb .green = 0;
		paint.color.rgb .blue = 255 - paint.color.rgb .red;
	}else if(count <= T3){
		paint.color.rgb .red = getSinValue(count);
		paint.color.rgb .green = 255 - paint.color.rgb .red;
		paint.color.rgb .blue  = 0;
	}else if(count <= T4){
		paint.color.rgb .red   = 0;
		paint.color.rgb .blue  = getSinValue(count - T);
		paint.color.rgb .green = 255 - paint.color.rgb .blue;
	}
	
	// 进行归一化
	paint.color.rgb .red = (paint.color.rgb .red*max>>8)+base;
	paint.color.rgb .blue = (paint.color.rgb .blue*max>>8)+base;
	paint.color.rgb .green = (paint.color.rgb .green*max>>8)+base;
	count ++;
}
