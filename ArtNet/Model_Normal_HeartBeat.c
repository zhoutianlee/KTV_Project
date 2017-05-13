

#include "Model_Normal_HeartBeat.h"
#include "stdio.h"

#define  UP0_TIME						12
#define  UP1_TIME						18

// 只支持红色的心跳模式
static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static Int16 status;
extern RGB   RGBArray[80];
static Paint paint;
static Paint clearPaint ;
static Int16 count;
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
};

pModel getModelNormal_HeartBeat(void)
{
	return (pModel)&model;
}

static void init (void)
{
	clearPaint.background.rgbw = (RGBW){0,0,0,0};
}

static void setting(int mode,int data)
{
	
}

static void setUp(void)
{
//	pPaint  paint= getPaint();
//	
//	if(paint != NULL){
//		paint->color.rgbw.red = 0x00;
//		paint->color.rgbw.green =0;
//		paint->color.rgbw.blue  = 0;
//		paint->color.rgbw.white = 0;
//		
//		paint->background.rgbw= (RGBW){0,0,0,0};
//	}
	paint.background.rgbw = (RGBW){0,0,0,0};
	paint.color.rgbw = (RGBW){0,0,0,0};
	setPaint(&paint);
	status = 0;
	count  = 0;
	// 需要禁止哪些屏幕？
	// 禁止矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,0);				// 锁定矩阵屏
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// 生磨枪
}

static void run  (pNodeList pnode,void*paramter)
{
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	Int16      len;
	static    char flag = 0;
	static    char R =0;
	pArray 		array;
	pPixelHead floorHead;
	
	
	if(status < 12){
		status ++;
		// 心跳数值向上累加
		if(paint.background.rgbw.red+20<=0xff)
		paint.background.rgbw.red += 20;
		R ++;
	}else if(status < 24){
		//　快速下滑，模拟心跳
		status ++;
		if(paint.background.rgbw.red>=20)
		paint.background.rgbw.red -= 20;
		R--;
	}else if(status <42){
		
		status ++;
		if(paint.background.rgbw.red+4<=0xff)
		paint.background.rgbw.red += 4;
		R++;
	}else if(status <60){
		status ++;
		if(paint.background.rgbw.red>=4)
		paint.background.rgbw.red -= 4;
		R--;
	}else{
		status =0;
		R=0;
	}
	
	// 执行所有相关屏幕的清屏处理
//	setPaint(&paint);
	setPaint(&clearPaint);
	clearPaint.color.rgbw = paint.background.rgbw;
	array	= getArrayIndex(0);if(array!= 0)clearScreen(array);				// 矩阵屏
	array = getArrayIndex(1);if(array!= 0)clearScreen(array);drawCircleXY(array,37,30,R*1.5);
//	if(count ==0){
//		setPaint(&clearPaint);
//		array	= getArrayIndex(0);if(array!= 0)clearScreen(array);				// 矩阵屏
//		array = getArrayIndex(1);if(array!= 0)clearScreen(array);drawCircleXY(array,37,30,R);
//	}else if(count >= 1000){
//		setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,1);				// 生磨枪
//		setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// 锁定矩阵屏
//	}
	setPaint(&paint);
	while(plist){
		
		punion = plist->pHead;
		if(plist ->enable != 1 ){goto Normal_HeartBeat_run_next;}
		if(plist->mode == ARTNETMODE_ARRAY){
			goto Normal_HeartBeat_run_next;				// 如果是设备不存在，则继续
//			if(punion->arrayNode.msg.isExit ==0)goto Normal_HeartBeat_run_next;				// 如果是设备不存在，则继续
//			clearScreen(plist->parray);													// 还是意思一下请一下屏幕咯
//			punion = (pUnionNode)punion ->arrayNode.pNext;
		}else{
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0 ){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				clearLine(&punion->lineNode);
				punion = (pUnionNode)punion->arrayNode.pNext;
			}
		}
Normal_HeartBeat_run_next:
		plist = plist->pNext;
	}
	count ++;
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


