
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

RGB   diamondRGB[25][20];																// 创建数组
RGB   floorRGB[20][50];																  // 底部灯带
//void  setModel(pModel _model)
//{
//	if(_model == NULL)return;
//	if(model != NULL){
//		model->destroy();																	// 先销毁
//	}
//	model = _model;
//	model->setUp();																			// 启动准备
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
	model = getModelNormal(0);													// 设置为普通柔和模式
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
		model->destroy();										// 先停止上衣模式的运行状态
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
		model->setUp();										// 创建该模式
		printf("mode %s is running\n",model->getName());
	}
}

void    runModel(pNodeList pnode,void*paramter)
{
	if(model != NULL){
		model->run(pnode,paramter);												// 运行
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
