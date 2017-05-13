



#include "Model_Normal_Romatic.h"
#include "stdio.h"

// 当前模式为幻彩，低速
// 取紫色、粉色、蓝色为主色调，进行交替变化

static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static void floorDisplay(void);
static void clearFloorSrceen(void);
static void clearDiamond(RGB rgb);
static void calculateDiamond(void);
static void drawDiamondCircle(int x,int y,Int16 R);
static void calculateParticle(pParticle particle);
static void diamondDisplay(void);

const static Int8  arrayMode0[]={255,220,180,150,120,80,50,40,20,10,5,3,2,1,0,0,0,0,0,0,0};
const static Int8  arrayMode1[]={0,0,0,0,5,10,20,40,80,120,150,200,255,255,200,150,120,80,40,20,10,5,0,0,0,0};
const static Int16 arrayTime[]={200,200,300,500,500,500,500,500,500,500,500,500,500,500,500,500,500};					// 模式之间切换的时间

const static Int8  arraySpeed[]={1,2,1,1,2,2,1,1,2,2,1,1,1};
const static Int8   Cycle[] = {20,18,24,17,12,19,30,16,13,22,21};

static Int8 array[][2] = {{220,30},{150,30},{100,15},{50,10}};								// 是个等级
static Int8 max;																			// 最大数字
static Int8 base;																			// 基础阈值
static Int8 layer;																		// 等级
static Int8 mode;
static Int8  arrayModeCount;
static Int16 modeCount;
static Int8  turnFlag;
static Int8  speedMove;																// 速度
static Paint paint;
static pPixelHead floorHead,diamondHead;
static Int8  arrayStatus[20];													// 用于分别记录arrayStatus的状态
static Int8  arrayCount[20];
static Particle particle[5];
static PointHandle pPhandle[5];
static RGB   pRGBArray[5];
static Int16 runCount;
static Int16 runMode;
static Int16 runCoutSwitch;
extern RGB   RGBArray[80];
extern RGB   diamondRGB[25][20];

static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
};

pModel getModelNormal_Romatic(void)
{
	return (pModel)&model;
}

static void init (void)
{
	layer = 0;
	max = array[layer][0];
	base = array[layer][1];
	
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	
	particle[0].vx = 1;
	particle[0].vy = 1;
	particle[0].x  = 5;
	particle[0].y  = 5;
	particle[0].dltVx = 60;
	particle[0].dltVy = 30;
	particle[0].size  = 3;
	particle[0].sumX = 0;
	particle[0].sumY= 0;
	
	particle[1].vx = 1;
	particle[1].vy = 2;
	particle[1].x  = 70;
	particle[1].y  = 10;
	particle[1].dltVx = 50;
	particle[1].dltVy = 10;
	particle[1].size  = 3;
	particle[1].sumX = 0;
	particle[1].sumY= 0;
	
	particle[2].vx = 1;
	particle[2].vy = 1;
	particle[2].x  = 15;
	particle[2].y  = 45;
	particle[2].dltVx = 40;
	particle[2].dltVy = 10;
	particle[2].size  = 3;
	particle[2].sumX = 0;
	particle[2].sumY= 0;
	
	particle[3].vx = 1;
	particle[3].vy = 1;
	particle[3].x  = 65;
	particle[3].y  = 85;
	particle[3].dltVx = 40;
	particle[3].dltVy = 10;
	particle[3].size  = 3;
	particle[3].sumX = 0;
	particle[3].sumY= 0;
	
	particle[4].vx = 1;
	particle[4].vy = 1;
	particle[4].x  = 15;
	particle[4].y  = 15;
	particle[4].dltVx = 40;
	particle[4].dltVy = 60;
	particle[4].size  = 3;
	particle[4].sumX = 0;
	particle[4].sumY= 0;
	
	int i;
	for(i=0;i<5;i++){
		pPhandle[i].len = 10;
	}
	pRGBArray[0] = (RGB){255,0,0};
	pRGBArray[1] = (RGB){255,255,0};
	pRGBArray[2] = (RGB){255,0,255};
	pRGBArray[3] = (RGB){150,20,0};
	pRGBArray[4] = (RGB){255,255,255};
	
	runCount = 0;
	runMode = 0;
	
	paint.background.rgbw = (RGBW){0,0,0,0};
}

static void setting(int mode,int data)
{
	
}

static void setUp(void)
{
	pArray 		array;
	mode = 0;
	modeCount = 0;
	turnFlag = 0;
	speedMove = 0;
	arrayModeCount = 0;
	setPaint(&paint);
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	clearFloorSrceen();																// 清除底板所有的灯点
	
	
	runCoutSwitch = 0;
	clearDiamond((RGB){0,0,0});
	setPaint(&paint);
	array	= getArrayIndex(0);clearScreen(array);
}

static void run  (pNodeList pnode,void*paramter)
{
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	floorDisplay();
	diamondDisplay();

	
	while(plist){
		
		punion = plist->pHead;
		if(plist->nodeID == floorHead->nodeID||plist->nodeID==diamondHead->nodeID){plist= plist->pNext;continue;}
		if(plist->mode == ARTNETMODE_ARRAY){
			punion = (pUnionNode)punion ->arrayNode.pNext;
			
		}else{
			
//			if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
			switch(runMode){
				case 0:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = arrayMode0[runCount];
						punion->lineNode.Data->rgb.green = arrayMode0[runCount];
						punion->lineNode.Data->rgb.blue = arrayMode0[runCount];
						punion = (pUnionNode)punion->arrayNode.pNext;
						
					}
					if(++runCount>= sizeof(arrayMode0)/sizeof(Int8))runCount = 0;
					if(++runCoutSwitch >= 450){
						runCoutSwitch = 0;
						runMode ++;
					}
				}break;
				case 1:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = arrayMode1[runCount];
						punion->lineNode.Data->rgb.green = arrayMode1[runCount];
						punion->lineNode.Data->rgb.blue = 0;
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode1)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				case 2:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = arrayMode1[runCount];
						punion->lineNode.Data->rgb.green = 0;
						punion->lineNode.Data->rgb.blue = arrayMode1[runCount];
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode1)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				case 3:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = arrayMode0[runCount];
						punion->lineNode.Data->rgb.green = 0;
						punion->lineNode.Data->rgb.blue = 0;
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode0)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				
				case 4:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = 0;
						punion->lineNode.Data->rgb.green = arrayMode1[runCount];
						punion->lineNode.Data->rgb.blue = 0;
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode1)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				case 5:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = 0;
						punion->lineNode.Data->rgb.green = 0;
						punion->lineNode.Data->rgb.blue = arrayMode0[runCount];
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode0)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				
				case 6:{
					while(punion){
						if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data->rgb.red = 0;
						punion->lineNode.Data->rgb.green = arrayMode1[runCount];
						punion->lineNode.Data->rgb.blue = arrayMode1[runCount];
						punion = (pUnionNode)punion->arrayNode.pNext;
						
						
					}
					if(++runCount>= sizeof(arrayMode1)/sizeof(Int8))runCount = 0;
						if(++runCoutSwitch >= 450){
							runCoutSwitch = 0;
							runMode ++;
						}
				}break;
				default:{
					runMode = 0;
					runCoutSwitch = 0;
				}
			}
		}
		plist = plist->pNext;
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

static void floorDisplay(void)
{
	int i;
	int half = 7;//floorHead->heigh>>1;
	int startCount;

	printf("status mode = %d\r\n",mode);
	switch(mode){
		case 0:{
			
			moveLineRGB(RGBArray,floorHead->width,1);
			RGBArray[0].red = arrayMode0[arrayModeCount];
			RGBArray[0].green = arrayMode0[arrayModeCount];
			RGBArray[0].blue = arrayMode0[arrayModeCount];
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode0)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<half;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			// 模式为0，地板单向
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				clearFloorSrceen();
			}else{
				modeCount++;
			}
		}break;
		case 1:{
			
			moveLineRGB(RGBArray,floorHead->width,1);
			RGBArray[0].red = arrayMode0[arrayModeCount];
			RGBArray[0].green = arrayMode0[arrayModeCount];
			RGBArray[0].blue = arrayMode0[arrayModeCount];
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode0)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=half;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
			}else{
				modeCount++;
			}
		}break;
		case 2:{
			moveLineRGB(RGBArray,floorHead->width,1);
			RGBArray[0].red = arrayMode0[arrayModeCount];
			RGBArray[0].green = arrayMode0[arrayModeCount];
			RGBArray[0].blue = arrayMode0[arrayModeCount];
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode0)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
			}else{
				modeCount++;
			}
		}break;
		case 3:{
			moveLineRGB(RGBArray,floorHead->width,-1);
			RGBArray[floorHead->width-1].red = arrayMode0[arrayModeCount];
			RGBArray[floorHead->width-1].green = arrayMode0[arrayModeCount];
			RGBArray[floorHead->width-1].blue = arrayMode0[arrayModeCount];
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode0)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
			}else{
				modeCount++;
			}
		}break;
		case 4:{
			moveLineRGB(RGBArray,floorHead->width,-1);
			RGBArray[floorHead->width-1].red = arrayMode0[arrayModeCount];
			RGBArray[floorHead->width-1].green = arrayMode0[arrayModeCount];
			RGBArray[floorHead->width-1].blue = 0;
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode1)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
			}else{
				modeCount++;
			}
		}break;
		case 5:{
			moveLineRGB(RGBArray,floorHead->width,1);
			RGBArray[0].red = arrayMode1[arrayModeCount];
			RGBArray[0].green = arrayMode1[arrayModeCount];
			RGBArray[0].blue  = 0;
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode1)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
			}else{
				modeCount++;
			}
		}break;
		case 6:{
			moveLineRGB(RGBArray,floorHead->width,1);
			RGBArray[0].red = arrayMode1[arrayModeCount];
			RGBArray[0].green = 0;
			RGBArray[80].blue  = arrayMode1[arrayModeCount];
			// 固定流星雨颜色交替，
			if(++arrayModeCount >= sizeof(arrayMode1)/sizeof(Int8)){
				arrayModeCount = 0;
			}
			for(i=0;i<floorHead->heigh;i++){
				setPixelLine(floorHead,i,RGBArray);
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
				clearFloorSrceen();
			}else{
				modeCount++;
			}
		}break;
		case 7:{
			
			// 参考流星雨,一定要错徐
			for(i=0;i<half;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode0);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = arrayMode0[arrayCount[i]];
						RGBArray[0].green = arrayMode0[arrayCount[i]];
						RGBArray[0].blue = arrayMode0[arrayCount[i]];
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode0)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
				clearFloorSrceen();
			}else{
				modeCount++;
			}
			
		}break;
		case 8:{
			// 参考流星雨,一定要错徐
			for(i=0;i<half;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode0);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i+half,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = arrayMode0[arrayCount[i]];
						RGBArray[0].green = arrayMode0[arrayCount[i]];
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i+half,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode0)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i+half,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i+half,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
			}else{
				modeCount++;
			}
		}break;
		case 9:{
			// 参考流星雨,一定要错徐
			for(i=0;i<half;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode0);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i+half,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = arrayMode0[arrayCount[i]];
						RGBArray[0].green = 0;
						RGBArray[0].blue = arrayMode0[arrayCount[i]];
						setPixelLine(floorHead,i+half,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode0)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i+half,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i+half,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
			}else{
				modeCount++;
			}
		}break;	
		case 10:{
			// 参考流星雨,一定要错徐
			for(i=0;i<14;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode0);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = arrayMode0[arrayCount[i]];
						RGBArray[0].blue = arrayMode0[arrayCount[i]];
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode0)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
			}else{
				modeCount++;
			}
		}break;	
		case 11:{
			// 参考流星雨,一定要错徐
			for(i=0;i<14;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode1);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = arrayMode1[arrayCount[i]];
						RGBArray[0].blue = arrayMode1[arrayCount[i]];
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode1)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
			}else{
				modeCount++;
			}
		}break;
		case 12:{
			// 参考流星雨,一定要错徐
			for(i=0;i<14;i++){
				
				// 先要计算你状态
				switch(arrayStatus[i]){
					case 0:{
						// 先要经过起始状态
						startCount = i*sizeof(arrayMode1);
						if(i&0x01) startCount += 15;
						else startCount+=5;
						if(arrayCount[i]>=startCount){
							arrayStatus[i]=1;
							arrayCount[i] = 0;
						}else{
							arrayCount[i]++;
						}
					}break;
					case 1:{
						// 已经经过起始状态，运行输入一条流星灯
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = arrayMode1[arrayCount[i]];
						RGBArray[0].green = arrayMode1[arrayCount[i]];
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  sizeof(arrayMode1)/sizeof(Int8)){
							arrayCount[i] = 0;
							arrayStatus[i]++;
						}
					}break;
					case 2:{
						// 需要冷却一段时间，但任然需要移动
						getPixelLine(floorHead,i,RGBArray);
						moveLineRGB(RGBArray,floorHead->width,1);
						RGBArray[0].red = 0;
						RGBArray[0].green = 0;
						RGBArray[0].blue = 0;
						setPixelLine(floorHead,i,RGBArray);
						
						if(++arrayCount[i]>=  20+(i&0x01)*5+i){
							arrayCount[i] = 0;
							arrayStatus[i]--;
						}
					}break;
					default:{
						arrayCount[i]=0;
						arrayStatus[i] =0;
					}break;
				}
				
			}
			if(modeCount >= arrayTime[mode]){
				mode++;
				modeCount = 0;
				arrayModeCount = 0;
				for(i=0;i<20;i++){
					arrayStatus[i] = 0;
					arrayCount[i]=0;
				}
			}else{
				modeCount++;
			}
		}break;
		default:{
			mode = 0;
		}
	}
	
}

static void diamondDisplay(void)
{
	int i;
	calculateDiamond();
	
//	for(i=0;i<diamondHead->heigh;i++){
//		setPixelLine(diamondHead,i,diamondRGB[i]);
//	}
	for(i=16;i<=diamondHead->heigh;i++){
		setPixelLine(diamondHead,i-16,diamondRGB[i]);
	}
	
	for(i=0;i<16;i++){
		setPixelLine(diamondHead,i+(diamondHead->heigh-16),diamondRGB[i]);
	}
}
static void clearFloorSrceen(void)
{
	int i;
	for(i=0;i<80;i++)
	RGBArray[i] = (RGB){0,0,0};
	
	for(i=0;i<floorHead->heigh;i++){
		setPixelLine(floorHead,i,RGBArray);
	}
}


static void calculateDiamond(void)
{
	// 
	Paint backupPaint;
	int   i;
	int   j;
	Point p;
	backupPaint.background.rgbw = (RGBW){0,0,0,0};
	backupPaint.color      = paint.background;
	
	setPaint(&backupPaint);
		
	clearDiamond(backupPaint.background.rgb);

	for(i=0;i<5;i++){
		calculateParticle(&particle[i]);
		p.width = particle[i].x;
		p.heigh = particle[i].y;
		addPoint(p,&pPhandle[i]);                                  //添加断点
	}
	// 绘图
	for(i=0;i<2;i++){
		for(j=0;j<10;j++){
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].red = pRGBArray[i].red/(j+1); 
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].green = pRGBArray[i].green/(j+1); 
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].blue = pRGBArray[i].blue/(j+1); 
		}
	}
//		printf("%d x = %d y = %d \r\n",0,particle[0].x,particle[0].y);
	setPaint(&paint);
	
}
// 运算粒子位置
static void calculateParticle(pParticle particle)
{
	particle->sumX += particle->dltVx;
	if(particle->sumX> 100){
		particle->x = particle->x+1;
		particle->sumX -= 100;
	}else if(particle->sumX<-100){
		particle->x = particle->x-1;
		particle->sumX += 100;
	}
	particle->sumY += particle->dltVy;
	if(particle->sumY> 100){
		particle->y = particle->y+1;
		particle->sumY -= 100;
	}else if(particle->sumY<-100){
		particle->y = particle->y-1;
		particle->sumY += 100;
	}
	particle->x += particle->vx;
	particle->y += particle->vy;
	// 边界检测
	if(particle->x <=0|| particle->x >=19){
		// 在X方向速度取反
		particle->vx = -particle->vx;
	}	
	if(particle->y<=0 || particle->y >=19){
		particle->vy = -particle->vy;
	}
	if(particle->x <0)particle->x = 0;
	if(particle->x >19)particle->x = 19;
	if(particle->y <0)particle->y = 0;
	if(particle->y >19)particle->y = 19;
}

// 绘制填充圆形
static void drawDiamondCircle(int x,int y,Int16 R)
{
	// 
	int   tx = 0;
	int   ty = R;
	int   d = 1-R;
	if(R!=0){
		
		while(tx <= ty){
				if(x+tx <= 19&&y+ty<=19) diamondRGB[y+ty][x+tx] = paint.background.rgb;//.color.rgb;
				if(x+tx <= 19&&y-ty>=0)  diamondRGB[y-ty][x+tx] = paint.background.rgb;//paint.color.rgb;
				if(x-tx >=0&&y+ty<=19) 	 diamondRGB[y+ty][x-tx]  = paint.background.rgb;//paint.color.rgb;
				if(x-tx >=0&&y-ty>=0) 	 diamondRGB[y-ty][x-tx]  = paint.background.rgb;//paint.color.rgb;	
				
				if(x+ty <= 19&&y+tx<=19) diamondRGB[y+tx][x+ty] = paint.background.rgb;//paint.color.rgb;
				if(x+ty <= 19&&y-tx>=0)  diamondRGB[y-tx][x+ty] = paint.background.rgb;//paint.color.rgb;
				if(x-ty >=0&&y+tx<=19) 	 diamondRGB[y+tx][x-ty]  = paint.background.rgb;//paint.color.rgb;
				if(x-ty >=0&&y-tx>=0) 	 diamondRGB[y-tx][x-ty]  = paint.background.rgb;//paint.color.rgb;	
				
				if(d <0)d+= (tx*2)+3;
				else d+=((tx-ty)*2)+5,ty--;
				tx++;
			}
	}
}

static void clearDiamond(RGB rgb)
{
	int i,j;
	for(i=0;i<20;i++){
		for(j = 0;j<20;j++){
			diamondRGB[i][j] = rgb;
		}
	}
}
