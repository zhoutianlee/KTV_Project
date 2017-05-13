
#include "bsp.h"
#include "Model_KTV_Rythem.h"
#include "stdio.h"
#include "SinTable.h"
#define RGBARRAY_SIZE 			sizeof(rgbArray1)/sizeof(RGB)
	
static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static void runSoundCB(int16_t*pdata,int16_t len);

static RGB rgbArray[]={{255,255,255},{200,200,200},{150,150,150},{100,100,100},{50,50,50},
												{25,25,25},{12,12,12},{6,6,6},{0,0,0},{0,0,0},
												{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
static RGB rgbArray1[]={{125,0,0},{125,0,0},{125,0,0},{125,0,0},{0,125,0},
												{0,125,0},{0,125,0},{0,125,0},{0,125,0},{0,125,0},
												{0,0,125},{0,0,125},{0,0,125},{0,0,125},{0,0,125}};
static RGB rgbArray2[]={{255,255,255},{255,255,255},{255,255,255},{255,255,255},{255,255,255},
												{255,255,255},{255,255,255},{255,255,255},{255,255,255},{255,255,255},
												{255,255,255},{255,255,255},{255,255,255},{255,255,255},{255,255,255},};
static Paint   			paint;
static uint16_t    	maxValue;
static float   			dimmer;
static float        sumDimmer;												
static char arrayCount=0;
static char         flagEnable;												
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

pModel getModelKTV_Rythem(void)
{
	return (pModel)&model;
}

static void init (void)
{
	
}

static void setting(int mode,int data)
{
	// KTV音乐律动模式中，不允许调节灯光亮度
}

static void setUp(void)
{
	pPixelHead floorHead,diamondHead;
	int i;
	setSoundCB(runSoundCB);
	setSound(1);
	paint.background.rgbw = (RGBW){0,0,0,0};
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	setPaint(&paint);
	dimmer = 1.0f;
	sumDimmer = 1.0f;
	maxValue = 4096;
	arrayCount = 0;
	
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	for(i=0;i<floorHead->width;i++){
		RGBArray[i].red = 255;
		RGBArray[i].blue = 255;
		RGBArray[i].green = 255;
	}
	for(i=floorHead->width;i<diamondHead->width;i++){
		RGBArray[i].red = 255;
		RGBArray[i].blue = 255;
		RGBArray[i].green = 255;
	}
	for(i = 0;i < diamondHead->heigh;i++){
		setPixelLine(diamondHead,i,RGBArray);
	}
	for(i = 0;i < floorHead->heigh;i++){
		setPixelLine(floorHead,i,RGBArray);
	}
}

static void run  (pNodeList pnode,void*paramter)
{
	pNodeList  plist = getNodeListHead();
	pArray 		array;
	int count ;
	pPixelHead floorHead,diamondHead;
	int i;
	// 进行颜色计算
	
	// 线进行颜色整体的变化
	pUnionNode punion;
	array	= getArrayIndex(0);if(array)adjectArrayLight(array,dimmer);
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	
	getPixelLine(floorHead,0,RGBArray);
	moveLineRGB(RGBArray,floorHead->width,1);
	for(i=1;i<floorHead->width;i++){
		RGBArray[i].red *= (float)dimmer ;
		RGBArray[i].blue *= (float)dimmer;
		RGBArray[i].green *= (float)dimmer;
	}
	RGBArray[0].red =   (float)rgbArray[arrayCount].red*sumDimmer;
	RGBArray[0].green = (float)rgbArray[arrayCount].green*sumDimmer;
	RGBArray[0].blue =  (float)rgbArray[arrayCount].blue*sumDimmer;
	for(i = 0;i < floorHead->heigh;i++){
		setPixelLine(floorHead,i,RGBArray);
	}
	
	getPixelLine(diamondHead,0,RGBArray);
	moveLineRGB(RGBArray,diamondHead->width,1);
	for(i=0;i<diamondHead->width;i++){
		RGBArray[i].red *= dimmer;
		RGBArray[i].blue *= dimmer;
		RGBArray[i].green *= dimmer;
	}
	RGBArray[0].red = rgbArray[arrayCount].red*sumDimmer;
	RGBArray[0].green = rgbArray[arrayCount].green*sumDimmer;
	RGBArray[0].blue = rgbArray[arrayCount].blue*sumDimmer;
	for(i = 0;i < diamondHead->heigh;i++){
		setPixelLine(diamondHead,i,RGBArray);
	}
	
	while(plist){
		punion = plist->pHead;
		if(plist->mode == ARTNETMODE_LINE){
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				// 整体颜色渐变
				// 此处用于改变颜色
				moveLine(&punion->lineNode,1);
				adjectLineLight(&punion->lineNode,dimmer);
				(*(pRGB)punion->lineNode.Data).red   = rgbArray[arrayCount].red*sumDimmer +1;
				(*(pRGB)punion->lineNode.Data).green = rgbArray[arrayCount].green*sumDimmer+1;
				(*(pRGB)punion->lineNode.Data).blue  = rgbArray[arrayCount].blue*sumDimmer +1;
				punion = (pUnionNode)punion->arrayNode.pNext;
			}
		}
		
		plist = plist->pNext;
	}
	if(++arrayCount >= RGBARRAY_SIZE){
		arrayCount =0;
	}
//	if(dimmer >255.0)printf("dimmer is max = %f\r\n",dimmer);
	dimmer = 1.0f;
	flagEnable = 1;
//	
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
	int16_t*pdest = pdata;
	
	int     i;
	// 位了节省方便，全部参数乘于2048
	// 变为正数
	
//	printf("len = %d\n",len);
	int16_t max = 0;
	if(flagEnable ==1){
		for(i=0;i<len-1;i++){
			if(*pdest > max)max = *pdest;
			pdest ++;
		}
		if(max > 1024) max = 1024;
		max      = max>>2;
		dimmer   = ((float)max/(float)maxValue);
		maxValue = max;															// 方便归一化
		sumDimmer*=dimmer;													// 最新的数据点应该变化的亮度
		// 计算出来的最大值，作为当前亮度输出
		// 律动的基本属性，就是根据音乐最高能量值作为当前的亮度输出
		flagEnable = 0;
	}

}
