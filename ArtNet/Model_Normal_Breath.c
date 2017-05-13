
#include "Model_Normal_Breath.h"

static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void clearRGBArray(RGB rgb);

static void destroy(void);
static RGB max;
static RGB upDlt;
static RGB downDlt;
static RGB current;
static char direction;
extern RGB   RGBArray[80];
static Paint paint;
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
};

pModel getModelNormal_Breath(void)
{
	return (pModel)&model;
}

static void init (void)
{
	// 初始化运算因子
	upDlt.red = upDlt.green = upDlt.blue =  4;
	downDlt.red = downDlt.green = downDlt.blue = 4;
	max.red = max.green = max.blue = 0xff;
	current.red = current.green = current.blue =  0;
	direction = 0;
}

static void setting(int mode,int data)
{
	
}

static void setUp(void)
{
	current.red = current.green = current.blue =  0;
	direction = 0;
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
	pArray 		 array;
	Int16			 count;
	pPixelHead floorHead,diamondHead;
	
	floorHead 	 = getFloorHead();
	diamondHead  = getDiamondHead();
	
//	for(count = 0;count < floorHead->heigh;count++){
//		getPixelLine(floorHead,count,RGBArray);
//		clearRGBArray(current);
//		setPixelLine(floorHead,count,RGBArray);
//	}
//	for(count = 0;count < diamondHead->heigh;count++){
//		getPixelLine(diamondHead,count,RGBArray);
//		clearRGBArray(current);
//		setPixelLine(diamondHead,count,RGBArray);
//	}
	paint.background.rgb = current;
	setPaint(&paint);
	
	while(plist){
		
		if(plist->mode == ARTNETMODE_LINE ){
			// 只运算线性组建
			punion = plist->pHead;
			while(punion){
				clearLine(&punion->lineNode);									// 清除颜色
				punion = (pUnionNode)punion ->lineNode.pNext;
			}
			
		}
		plist = plist->pNext;
	}
	
	if(direction ==0){
		if(current.red +upDlt.red >= max.red){
			current.red = max.red;
			direction = 1;
		}else current.red += upDlt.red;
		if(current.green +upDlt.green >= max.green){
			current.green = max.green;
			direction = 1;
		}else current.green +=upDlt.green;
		if(current.blue +upDlt.blue >= max.blue){
			current.blue = max.blue;
			direction = 1;
		}else current.blue += upDlt.blue;

	}else{
		if(current.red<= downDlt.red){
			current.red = 0;
			direction = 0;
		}else current.red -= downDlt.red;
		if(current.green<= downDlt.green){
			current.green = 0;
			direction = 0;
		}else current.green -= downDlt.green;
		if(current.blue<= downDlt.blue){
			current.blue = 0;
			direction = 0;
		}else current.blue -= downDlt.blue;
		
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

static void clearRGBArray(RGB rgb)
{
	Int16 i;
	for(i = 0;i< 80;i++){
		RGBArray[i] = rgb;
	}
}
