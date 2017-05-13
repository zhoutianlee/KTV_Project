
#include "Model_KTV_Dynamic.h"
#include "bsp.h"
#include "stdio.h"
#include "SinTable.h"
//#include "bsp.h"
// �ο�matlab���� lightTest
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
static void runSoundCB(int16_t*pdata,int16_t len);
static void calculateColor(void);

static Int8 array[][2] = {{220,30},{150,30},{100,15},{50,10}};								// �Ǹ��ȼ�
static Int8 max;																			// �������
static Int8 base;																			// ������ֵ
static Int8 layer;																		// �ȼ�
static Int16 count;																		// ����ֵ
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

pModel getModelKTV_Dynamic(void)
{
	return (pModel)&model;
}

static void init (void)
{
	layer = 3;
	max   = array[layer][0];
	base  = array[layer][1];
}

static void setting(int mode,int data)
{
	// ������Ҫ����layer��ֻ
}

static void setUp(void)
{
	setSoundCB(runSoundCB);
	setSound(1);
	paint.background.rgbw = (RGBW){0,0,0,0};
	setPaint(&paint);
	setLightTypeEnable(LIGHT_TYPE_DIAMOND1,1);				// ����������
	setLightTypeEnable(LIGHT_TYPE_DIAMOND0,1);
	setLightTypeEnable(LIGHT_TYPE_FLOOR,1);
	setLightTypeEnable(LIGHT_TYPE_TOP,1);
	setLightTypeEnable(LIGHT_TYPE_TV,1);
	setLightTypeEnable(LIGHT_TYPE_SOUNDWAVE,0);				// ��ĥǹ
}

static void run  (pNodeList pnode,void*paramter)
{
	pNodeList  plist = getNodeListHead();
	pArray 		array;
	// ������ɫ����
	calculateColor();
	
	// �߽�����ɫ����ı仯
	pUnionNode punion;
	array	= getArrayIndex(0);if(array)clearScreen(array);
	
	while(plist){
		punion = plist->pHead;
		if(plist->mode == ARTNETMODE_LINE){
			while(punion){
				
				if(punion->lineNode.msg.isExit ==0){punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				// ������ɫ����
				// �˴����ڸı���ɫ
				clearLine(&punion->lineNode);
				punion = (pUnionNode)punion->arrayNode.pNext;
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
	setSound(0);
}

static void runSoundCB(int16_t*pdata,int16_t len)
{
//	static int count =0;
//	if(count%31 ==0)
//	printf("dynamic call\r\n");
//	count ++;
}

static void calculateColor(void)
{
	if(count > T1){
		count = 0;
	}
	if(count <= T2){
		paint.background.rgbw .red = getSinValue(count);
		paint.background.rgbw .green = 0;
		paint.background.rgbw .blue = 255 - paint.background.rgbw .red;
	}else if(count <= T3){
		paint.background.rgbw .red = getSinValue(count);
		paint.background.rgbw .green = 255 - paint.background.rgbw .red;
		paint.background.rgbw .blue  = 0;
	}else if(count <= T4){
		paint.background.rgbw .red   = 0;
		paint.background.rgbw .blue  = getSinValue(count - T);
		paint.background.rgbw .green = 255 - paint.background.rgbw .blue;
	}
		paint.background.rgbw.white = 0;
	
	// ���й�һ��
	paint.background.rgbw .red = ((paint.background.rgbw .red*max)>>8)+base;
	paint.background.rgbw .blue = ((paint.background.rgbw .blue*max)>>8)+base;
	paint.background.rgbw .green = ((paint.background.rgbw .green*max)>>8)+base;
	count ++;
}
