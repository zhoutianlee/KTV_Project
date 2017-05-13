

#include "Model_KTV_SOFT.H"
#include "stdio.h"
#include "bsp.h"
#include "sintable.h"
#include "math.h"
// 变态模式

#define FLOOR_LINE_SIZE						7												// 7条，每条50个像素点
#define FLOOR_BLOCK_SIZE					50/6.0f
#define RGBARRAY_SIZE 						sizeof(rgbArray)/sizeof(RGB)
#define T 		512
#define T1		(T*3/2)
#define T2		(T/2)
#define T3		 T
#define T4 		 T1

static RGB rgbArray[]={{255,255,255},{200,200,200},{150,150,150},{100,100,100},{50,50,50},
												{25,25,25},{12,12,12},{6,6,6},{0,0,0},{0,0,0},
												{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static void dispFloor(void);
static void dispDiamond(void);
static void calculateFloor(void);
static void clearFloor(void);
static void setFloorBlock(int row,int column,RGB rgb);
static void culcateCircle(int row);
static void calculateColor(void);

static Int16 status;
static Int16 statusCount;
static Int16 statusData;																	// 用来计算随机数
static Int16 turnCount;
static RGB	 colorRGB;
static char arrayCount=0;
static unsigned char Dimmer;
extern RGB   floorRGB[20][50];														// 底板映射
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

pModel getModelKTV_Soft(void)
{
	return (pModel)&model;
}

static void init (void)
{
	
}

static void setting(int mode,int data)
{
	
}

static void setUp(void)
{
	  
	int i ,j;
	status = 0;
	statusCount = 0;
	statusData = 1234;
	for(i=0;i<20;i++){
		for(j=0;j<50;j++){
			floorRGB[i][j].red = 0;
			floorRGB[i][j].green = 0;
			floorRGB[i][j].blue = 0;
		}
	}
	
	for(i=0;i<25;i++){
		for(j=0;j<20;j++){
			diamondRGB[i][j].red = 0;
			diamondRGB[i][j].green = 0;
			diamondRGB[i][j].blue = 0;
		}
	}
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
	clearFloor();
	pArray array = getArrayIndex(0);
	clearScreen(array);
}

static void run  (pNodeList pnode,void*paramter)
{ 
	static char divFreq=1;
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	pPixelHead floorHead,diamondHead;
	calculateFloor();
	dispFloor();
	dispDiamond();
	floorHead   = getFloorHead();
	diamondHead = getDiamondHead();
	while(plist){
		
		punion = plist->pHead;
		if(plist->nodeID == floorHead->nodeID||plist->nodeID==diamondHead->nodeID){plist= plist->pNext;continue;}
		if(plist->mode == ARTNETMODE_ARRAY){
			punion = (pUnionNode)punion ->arrayNode.pNext;
			
		}else{
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}

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

static void dispFloor(void)
{
	// 进行底板页面显示
	int i ,j;
	pPixelHead floorHead = getFloorHead();
	for(i=0;i<FLOOR_LINE_SIZE*2;i++){
		setPixelLine(floorHead,i,floorRGB[i]);
	}
}

static void dispDiamond(void)
{
	// 进行底板页面显示
	int i ,j;
	pPixelHead diamondHead = getDiamondHead();
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

static void calculateFloor(void)
{
	int i,j;
	static unsigned int value=0;
	calculateColor();
	int data;
	switch(status){
		case 0:{
			// 汇聚，向中心靠拢
			for(i=0;i<=14;i++){
				moveLineRGB(&floorRGB[i][0],25,1);
				moveLineRGB(&floorRGB[i][25],25,-1);
				floorRGB[i][0] = colorRGB;
				floorRGB[i][50-1] = colorRGB;
			}
//			for(i=0;i<20;i++){
//				moveLineRGB(&diamondRGB[i][0],10,1);
//				moveLineRGB(&diamondRGB[i][10],10,-1);
//				diamondRGB[i][0] = colorRGB;
//				diamondRGB[i][20-1] = colorRGB;
//			}
			for(i=0;i<20;i++){
				for(j=0;j<20;j++){
					// 计算坐标点的值
					
					if((data =250*sin(2*3.14f*sqrt(((i-10)*(i-10)+(j-10)*(j-10)))/10.0f + 25*turnCount*3.14/180.0f)-50)<0)data =0;
						//if((data =250*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + 50*value*3.14/180.0f)-50)<0)data =0;
					diamondRGB[i][j].red = data;//data =200*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value);//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					diamondRGB[i][j].green = data;//floorRGB[i][j].red;
					diamondRGB[i][j].blue	 = 0;
					
				}
			}
			// 添加新的像素点
			if(++turnCount >= 1000){
				turnCount =0;
				status ++;
			}
		}break;
		case 1:{
			// 发散
			for(i=0;i<=14;i++){
				moveLineRGB(&floorRGB[i][0],25,-1);
				moveLineRGB(&floorRGB[i][25],25,1);
				floorRGB[i][25-1] = colorRGB;
				floorRGB[i][25] = colorRGB;
			}
//			for(i=0;i<20;i++){
//				moveLineRGB(&diamondRGB[i][0],10,-1);
//				moveLineRGB(&diamondRGB[i][10],10,1);
//				diamondRGB[i][9] = colorRGB;
//				diamondRGB[i][10] = colorRGB;
//			}
			for(i=0;i<20;i++){
				for(j=0;j<20;j++){
					// 计算坐标点的值
					
					if((data =250*cos(2*3.14f*sqrt(((i-10)*(i-10)+(j-10)*(j-10)))/10.0f + 25*turnCount*3.14/180.0f)-50)<0)data =0;
					diamondRGB[i][j].red = data;//data =200*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value);//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					diamondRGB[i][j].green = 0;//floorRGB[i][j].red;
					diamondRGB[i][j].blue	 = data;
					
				}
			}
			if(++turnCount >= 1000){
				turnCount =0;
				status ++;
			}
		}break;
	}
}

static void calculateFloor0(void)
{
	int index;
	int i,j;
	int data;
	static unsigned int value = 0;
//	status = 1;
	
	switch(status){
		case 0:{
			clearFloor();
			for(i=0;i<50;i++){
				floorRGB[value][i] = (RGB){255,0,0};
			}
			if(++value>14){
				value = 0;
				status ++;
			}
		}break;
		case 1:{
			clearFloor();
			for(i=0;i<50;i++){
				floorRGB[value][i] = (RGB){255,255,0};
				floorRGB[value+7][i] = (RGB){255,255,0};
			}
			if(++value>7){
				value = 0;
				status ++;
			}
		}break;
		case 2:{
			clearFloor();
			setFloorBlock(value,value,(RGB){255,255,255});
			if(++value >7){
				value = 0;
				status++;
			}
		}break;
		case 3:{
			clearFloor();
			setFloorBlock(7-value,value,(RGB){0,255,255});
			if(++value >7){
				value = 0;
				status++;
			}
		}break;
		case 4:{
			clearFloor();
			setFloorBlock(7-value,7-value,(RGB){0,255,0});
			if(++value >7){
				value = 0;
				status++;
			}
		}break;
		case 5:{
			clearFloor();
			setFloorBlock(value,7-value,(RGB){0,0,255});
			if(++value >7){
				value = 0;
				status++;
			}
		}break;
		case 6:{
			clearFloor();
			for(i=0;i<50;i++){
				floorRGB[7-value][i] = (RGB){0,255,0};
				floorRGB[14-value][i] = (RGB){0,255,0};
			}
			if(++value>7){
				value = 0;
				status ++;
				clearFloor();
			}
		}break;
		case 7:{
			

//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					
					if((data =250*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + 50*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red = data;//data =200*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value);//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = 0;//floorRGB[i][j].red;
					floorRGB[i][j].blue	 = 0;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 100){
				value = 0;
				status++;
			}
		}break;
		case 8:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 45*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = data;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].blue = floorRGB[i][j].red;
					floorRGB[i][j].green= 0;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		case 9:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 40*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = data;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = floorRGB[i][j].red;
					floorRGB[i][j].blue= 0;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		case 10:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 35*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = data;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = floorRGB[i][j].red;
					floorRGB[i][j].blue= floorRGB[i][j].red;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		
		case 11:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 30*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = 0;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = data;
					floorRGB[i][j].blue= data;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		case 12:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 25*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = 0;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = data;
					floorRGB[i][j].blue= 0;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		case 13:{
//			clearFloor();
			// im(x,y) = sin(2*pi*20*((x-a)^2+(y-a)^2))^0.5+tha);
			for(i=0;i<7;i++){
				for(j=0;j<50;j++){
					// 计算坐标点的值
					if((data =250*cos(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/30.0f + 20*value*3.14/180.0f)-50)<0)data =0;
					floorRGB[i][j].red  = 0;//125*sin(2*3.14f*sqrt(((i-3)*(i-3)+(j-25)*(j-25)))/20.0f + value)+125;//getSinValue(sqrt((i-3)*(i-3)+(j-25)*(j-25))+value);
					floorRGB[i][j].green = 0;
					floorRGB[i][j].blue= data;
					
					floorRGB[14-i][j] = floorRGB[i][j];
				}
			}
			if(++value >= 300){
				value = 0;
				status++;
			}
		}break;
		default:{
			status = 0;
			value = 0;
		}
	}
}

static void clearFloor(void)
{
	int i ,j;
	for(j=0;j<25;j++){
		for(i=0;i<50;i++){
			floorRGB[j][i] = (RGB){0,0,0};
		}	
	}	
}

static void setFloorBlock(int row,int column,RGB rgb)
{
	int i;
	for(i=FLOOR_BLOCK_SIZE*column;i<FLOOR_BLOCK_SIZE*(column+1);i++){
		floorRGB[row][i] = rgb;
		floorRGB[row+1][i] = rgb;
		floorRGB[14-row-1][i] = rgb;
		floorRGB[14-row-2][i] = rgb;
	}
}

static void calculateColor(void)
{
	static int  myCount = 0;
	static char flag = 0;
	if(flag == 0){
		if(++myCount >=10){
			myCount = 0;
			flag = 1;
		}
	}else{
		colorRGB = (RGB){0,0,0};
		if(++myCount >=5){
			myCount = 0;
			flag = 0;
		}
		return;
	}
	if(statusCount > T1){
		statusCount = 0;
	}
	if(statusCount <= T2){
		colorRGB.red = getSinValue(statusCount);
		colorRGB.green = 0;
		colorRGB.blue = 255 - colorRGB.red;
	}else if(statusCount <= T3){
		colorRGB.red = getSinValue(statusCount);
		colorRGB.green = 255 - colorRGB.red;
		colorRGB.blue  = 0;
	}else if(statusCount <= T4){
		colorRGB.red   = 0;
		colorRGB.blue  = getSinValue(statusCount - T);
		colorRGB.green = 255 - colorRGB.blue;
	}
	
	colorRGB.red >>=1;
	colorRGB.green>>=1;
	colorRGB.blue >>=1;
//	if(colorRGB.red>50){
//		colorRGB.red -=50;
//	}else{
//		colorRGB.red = 0;
//	}
	
//	if(colorRGB.green>50){
//		colorRGB.green -=50;
//	}else{
//		colorRGB.green = 0;
//	}
//	
//	if(colorRGB.blue>50){
//		colorRGB.blue -=50;
//	}else{
//		colorRGB.blue = 0;
//	}
	statusCount +=5;
}


