

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
	RGBW	    rgbw;																		// ��ɫ
	Int16     dimmer;																	// ����
	MODELTYPE type;																		// ģʽ
	
}PublicModel,*pPublicModel;
typedef struct Model{

	void (*init) (void);															// ���ڳ�ʼ���ڴ�Ȳ���
	void (*setting)(int type,int data);
	void (*setUp)(void);															// ����ģʽ�л��������ȵ���setUp
	void (*run)  (pNodeList pnode,void*paramter);			// ����
	char (*isEnd)(pNodeList pnode,void*paramter);			// �ж��Ƿ���Ҫ����
	void (*end)  (pNodeList pnode,void*paramter);			// �����ص�����
	void (*destroy)(void);														// ����һ����ͼ����
	char*(*getName)(void);
}Model,*pModel;

typedef struct PointHandl{
	int   len;
	Point point[10];
}PointHandle,*pPointHandle;

typedef struct Particle{	
	int  	 x;
	int  	 y;
	int  	 vx;																					// ÿ����һ�Σ��ƶ�һ��
	int  	 vy;
	int  	 sumX;
	int  	 sumY;
	int  	 dltVx;																				// �൱���ٶȵ�С����
	int  	 dltVy;																				// ͬ��
	Int16  size;																				// ��С
}Particle,*pParticle;

typedef struct Meteor{																// ����

	Particle particle;																	// ����
	Int16  len;
	//     ���ǵ���ʼ����ɫ����
	Int8   red;
	Int8   green;
	Int8	 blue;
	Int8   white;
	//     ��ɫ�仯�ݶ�
	Int8   dltRed;
	Int8   dltGreen;
	Int8   dltBlue;
	Int8   dltWhite;
}Meteor,*pMeteor;

typedef struct FireWorks{															// ������ǵƹ����̻�

	Meteor    meteor;
	struct    FireWorks*pNext;
	
}FireWorks,*pFireWorkes;

void    initModel(void);
//void  	setModel(pModel model);
//pModel 	getModel(void);
void    setModelType(MODELTYPE type,int subType);
void    runModel(pNodeList pnode,void*paramter);				// �����в���
void    setCurrentData(MODELDATA type,Int16 data2);			// 
void    addPoint(Point point,pPointHandle phandle);

#endif
