

#ifndef __MODEL__H
#define __MODEL__H

#include "DmxOut.h"
#include "ArtNetType.h"
#include "Display.h"
#include "math.h"

typedef enum MODELTYPE{
	MODELTYPE_NORMAL= 1,
	MODELTYPE_KTV,
	MODELTYPE_DIANYIN
}MODELTYPE;

typedef enum MODELDATA{
	MODELDATA_COLOR,
	MODELDATA_DIMMER
}MODELDATA;

typedef struct PublicModel{
	RGBW	    rgbw;																		// 颜色
	Int16     dimmer;																	// 亮度
	MODELTYPE type;																		// 模式
	
}PublicModel,*pPublicModel;
typedef struct Model{

	void (*init) (void);															// 用于初始化内存等参数
	void (*setting)(int type,int data);
	void (*setUp)(void);															// 其他模式切换回来会先调用setUp
	void (*run)  (pNodeList pnode,void*paramter);			// 运行
	char (*isEnd)(pNodeList pnode,void*paramter);			// 判断是否需要结束
	void (*end)  (pNodeList pnode,void*paramter);			// 结束回掉方法
	void (*destroy)(void);														// 销毁一个视图方法
	char*(*getName)(void);
}Model,*pModel;

typedef struct PointHandl{
	int   len;
	Point point[10];
}PointHandle,*pPointHandle;

typedef struct Particle{	
	int  	 x;
	int  	 y;
	int  	 vx;																					// 每调用一次，移动一次
	int  	 vy;
	int  	 sumX;
	int  	 sumY;
	int  	 dltVx;																				// 相当于速度的小数点
	int  	 dltVy;																				// 同上
	Int16  size;																				// 大小
}Particle,*pParticle;

typedef struct Meteor{																// 流星

	Particle particle;																	// 粒子
	Int16  len;
	//     流星灯起始的颜色分量
	Int8   red;
	Int8   green;
	Int8	 blue;
	Int8   white;
	//     颜色变化梯度
	Int8   dltRed;
	Int8   dltGreen;
	Int8   dltBlue;
	Int8   dltWhite;
}Meteor,*pMeteor;

typedef struct FireWorks{															// 多个流星灯构成烟花

	Meteor    meteor;
	struct    FireWorks*pNext;
	
}FireWorks,*pFireWorkes;

void    initModel(void);
//void  	setModel(pModel model);
//pModel 	getModel(void);
void    setModelType(MODELTYPE type,int subType);
void    runModel(pNodeList pnode,void*paramter);				// 带运行参数
void    setCurrentData(MODELDATA type,Int16 data2);			// 
void    addPoint(Point point,pPointHandle phandle);

#endif
