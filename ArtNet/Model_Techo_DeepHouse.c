

#include "Model_Techo_DeepHouse.h"
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
	
const static Int8   waterLine[] = {0,0,0,0,10,20,40,80,160,200,255,200,160,80,40,20,10,0,0,0,0};//{200,250,200,170,130,100,50,25,10,5,0,0,0,0,0,0};

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
static void drawTrangle(pArray array,Int8 R);
static void clearDiamond(RGB rgb);
static void calculateDiamond(void);
static void drawDiamondCircle(int x,int y,Int16 R);
static void calculateParticle(pParticle particle);

static Int16 maxDimmer;										//	最大亮度
static float Dimmer;											// 亮度系数
static Int16 ColorCount;																		// 计数值
static Paint paint;
static char  status;
static char  turnFlag ;
static char  DarkLock = 0;
static char  WaterCount;
static TimeClock timeClock0;
static char  R;
static Particle particle[5];
static PointHandle pPhandle[5];
static RGB   pRGBArray[5];

extern RGB   RGBArray[80];
extern RGB   backupRGB[80];
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

pModel getModelTecho_DeepHouse(void)
{
	return (pModel)&model;
}

static void init (void)
{
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
	particle[1].vy = 1;
	particle[1].x  = 70;
	particle[1].y  = 10;
	particle[1].dltVx = 50;
	particle[1].dltVy = 10;
	particle[1].size  = 3;
	particle[1].sumX = 0;
	particle[1].sumY= 0;
	
	particle[2].vx = 1;
	particle[2].vy = 2;
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
	particle[4].dltVy = 10;
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
	pRGBArray[3] = (RGB){0,255,0};
	pRGBArray[4] = (RGB){255,255,255};
	
}

static void setting(int mode,int data)
{
	setSoundCB(runSoundCB);
	setSound(1);
}

static void setUp(void)
{
	int i;
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
	for(i =0;i<80;i++){
		pdest->red = 0;
		pdest->green = 0;
		pdest->blue = 0;
	}
	WaterCount = 0;
	
	timeClock0.x0 = 0;
	timeClock0.y0 = 0;
	timeClock0.count = 0;
	timeClock0.status = 0;
	
	R = 0;
	turnFlag = 1;
	
	clearDiamond((RGB){0,0,0});
	for(i=0;i<5;i++){}{
		pPhandle[i].len = 10;
	}
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
	
	if(turnFlag == 1)moveLineRGB(RGBArray,80,1);
	else moveLineRGB(RGBArray,80,-1);
		
	if(++count >= WATERSIZE){
		count =0;
	}
	
//	printf("diamond width = %d\r\n",diamondHead->width);
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
 calculateDiamond();
	
//	printf("heigh = %d\r\n",diamondHead->heigh);
	// 重构映射
	for(i=16;i<=diamondHead->heigh;i++){
		setPixelLine(diamondHead,i-16,diamondRGB[i]);
	}
	
	for(i=0;i<16;i++){
		setPixelLine(diamondHead,i+(diamondHead->heigh-16),diamondRGB[i]);
	}
	
	array	= getArrayIndex(0);
	
	calculateScreen(array,&timeClock0);
	
	
	if(DarkLock ==0){
		while(plist){
			
			punion = plist->pHead;
			if(plist->nodeID == floorHead->nodeID||plist->nodeID==diamondHead->nodeID){plist= plist->pNext;continue;}
			if(plist->mode == ARTNETMODE_ARRAY){
				// 只需要调用清屏程序
	//			clearScreen(plist->parray);
			}else{
				if(turnFlag == 1){
					while(punion){
						moveLine(&punion->lineNode,1);
						punion->lineNode.Data[0].rgb.red = waterLine[count];
						punion->lineNode.Data[0].rgb.green = waterLine[count];
						punion->lineNode.Data[0].rgb.blue = waterLine[count];
						punion = (pUnionNode)punion->arrayNode.pNext;
					}
				}else{
					while(punion){
						moveLine(&punion->lineNode,-1);
						punion->lineNode.Data[punion->lineNode.len].rgb.red = waterLine[count];
						punion->lineNode.Data[punion->lineNode.len].rgb.green = waterLine[count];
						punion->lineNode.Data[punion->lineNode.len].rgb.blue = waterLine[count];

						punion = (pUnionNode)punion->arrayNode.pNext;
					}
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
//	printf("maxDimmer = %d max = %d Dimmer = %f line = %d\r\n",maxDimmer,max,Dimmer,__LINE__);
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
			if(turnFlag == 1)turnFlag = -1;
			else turnFlag = 1;
		}
	}
	
	ColorCount ++;
	Dimmer = 1;

	if(turnFlag == 1){
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
	}else{
		switch(status){
			case 0:{
				RGBArray[79].red = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
				RGBArray[79].green = 0;
				RGBArray[79].blue = 0;
			}break;
			case 1:{
				RGBArray[79].red = waterLine[WaterCount];;
				RGBArray[79].green = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
				RGBArray[79].blue = 0;
			}break;
			case 2:{
				RGBArray[79].red = 0;
				RGBArray[79].green = waterLine[WaterCount];
				RGBArray[79].blue = 0;//getSinValue(ColorCount)*Dimmer;
			}break;
			case 3:{
				RGBArray[79].red = 0;//getSinValue(ColorCount)*Dimmer;
				RGBArray[79].green = waterLine[WaterCount];
				RGBArray[79].blue = waterLine[WaterCount];
			}break;
			case 4:{
				RGBArray[79].red = 0;//getSinValue(ColorCount)*Dimmer;
				RGBArray[79].green = 0;
				RGBArray[79].blue = waterLine[WaterCount];
			}break;
			case 5:{
				RGBArray[79].red = waterLine[WaterCount];//getSinValue(ColorCount)*Dimmer;
				RGBArray[79].green = 0;
				RGBArray[79].blue = waterLine[WaterCount];
			}break;
			default:{
				status = 0;
			}
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
	
	if(Dimmer > 1){
		R++;
	}else {//if(Dimmer < 0.95){
		R--;
	}
	if(R <0)R = 0;
	if(R >6)R = 5;
	
	drawTrangle(array,R);
	
	setPaint(&paint);
	
}


static void drawTrangle(pArray array,Int8 R)
{
	// 两个中心点，分别是 5,3;17,3;
	if(R ==0){
		
		drawLineXY(array,4,2,6,2);
		drawLineXY(array,4,3,6,3);
		drawLineXY(array,12+4,2,12+6,2);
		drawLineXY(array,12+4,3,12+6,3);
	}else if(R <=2){
		
		drawLineXY(array,5-R,2-R,5+R,2-R);											// 绘制直线
		drawLineXY(array,5-R,3+R,5+R,3+R);											// 绘制直线
		drawLineXY(array,5-R,2-R,5-R,3+R);											// 绘制直线
		drawLineXY(array,5+R,2-R,5+R,3+R);											// 绘制直线
		
		drawLineXY(array,12+5-R,2-R,12+5+R,2-R);											// 绘制直线
		drawLineXY(array,12+5-R,3+R,12+5+R,3+R);											// 绘制直线
		drawLineXY(array,12+5-R,2-R,12+5-R,3+R);											// 绘制直线
		drawLineXY(array,12+5+R,2-R,12+5+R,3+R);											// 绘制直线
	}else if(R < 6){
		drawLineXY(array,5-R,0,5+R,0);											// 绘制直线
		drawLineXY(array,5-R,5,5+R,5);											// 绘制直线
		drawLineXY(array,5-R,0,5-R,5);											// 绘制直线
		drawLineXY(array,5+R,0,5+R,5);											// 绘制直线
		
		drawLineXY(array,12+5-R,0,12+5+R,0);											// 绘制直线
		drawLineXY(array,12+5-R,5,12+5+R,5);											// 绘制直线
		drawLineXY(array,12+5-R,0,12+5-R,5);											// 绘制直线
		drawLineXY(array,12+5+R,0,12+5+R,5);											// 绘制直线
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
//		for(j = 0;j<particle[i].size;j++)
//			drawDiamondCircle(particle[i].x,particle[i].y,j);
	}
	// 绘图
	for(i=0;i<5;i++){
		for(j=0;j<10;j++){
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].red = pRGBArray[i].red>>j; 
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].green = pRGBArray[i].green>>j; 
			diamondRGB[pPhandle[i].point[j].heigh][pPhandle[i].point[j].width].blue = pRGBArray[i].blue>>j; 
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

//static void runSoundCB(int16_t*pdata,int16_t len)
//{
//	static int count =0;
//	int i;
//	if(count%31 ==0)
//	printf("deephouse call\r\n");
//	count ++;
//	// 需要运算FFT数据
//	for(i=0;i<len;i++){
//		bufferIn[i] = *pdata++;
//	}
//	// 进行FFT运算
//	cr4_fft_256_stm32(bufferOut,bufferIn,256);
//	for(i=0;i<128;i++){
//		short x,y;
//		x = bufferOut[i];
//		y = bufferOut[i]>>16;
//		bufferMag[i] = x*x + y*y;
//	}
//	printf("finish fft work\n");
//	// 运算玩FFT数据
//}
