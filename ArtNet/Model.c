
#include "Model.h"
#include "Model_Normal.h"
#include "Model_KTV.h"
#include "Model_Techo.h"

#include "stdio.h"

static pModel model;
static MODELTYPE modeltype;
static int       subType;
RGB   RGBArray[80];
RGB   backupRGB[80];

RGB   diamondRGB[25][20];																// ��������
RGB   floorRGB[20][50];																  // �ײ��ƴ�
//void  setModel(pModel _model)
//{
//	if(_model == NULL)return;
//	if(model != NULL){
//		model->destroy();																	// ������
//	}
//	model = _model;
//	model->setUp();																			// ����׼��
//}

//pModel getModel(void)
//{
//	return model;
//}

void    initModel(void)
{
	initModelNormal();
	initModelKTV();
	initModelTecho();
	model = getModelNormal(0);													// ����Ϊ��ͨ���ģʽ
}

void    setModelType(MODELTYPE type,int __subType)
{
	__subType --;
	if(__subType <0)__subType = 0;
	if(modeltype == type &&subType == __subType)return;
	
	modeltype = type;
	subType   = __subType;
	
	if(model!= NULL)
	{
		model->destroy();										// ��ֹͣ����ģʽ������״̬
		printf("mode %s is destroy\n",model->getName());
	}
	
	switch(type){
		case MODELTYPE_NORMAL:{
			model = getModelNormal(subType);
		}break;
		case MODELTYPE_KTV:{
			model = getModelKTV(subType);
		}break;
		case MODELTYPE_DIANYIN:{
			model = getModelTecho(subType);
		}break;
	}
	if(model != NULL)
	{
		model->setUp();										// ������ģʽ
		printf("mode %s is running\n",model->getName());
	}
}

void    runModel(pNodeList pnode,void*paramter)
{
	if(model != NULL){
		model->run(pnode,paramter);												// ����
	}
}

void    setCurrentData(MODELDATA type,Int16 data1)
{
	if(model!= NULL)model->setting(type,data1);
}

void    addPoint(Point point,pPointHandle phandle)
{
	int i;
	if(phandle!= NULL){
		for(i=1;i<phandle->len;i++){
			phandle->point[phandle->len-i] = phandle->point[phandle->len-i-1];
		}
		phandle->point[0] = point;
	}
}
