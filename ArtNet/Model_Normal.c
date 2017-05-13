

#include "Model_Normal.h"
#include "Model_Normal_Soft.h"
#include "Model_Normal_Breath.h"
#include "Model_Normal_Romatic.h"
#include "Model_Normal_HeartBeat.h"
static int    type;
static pModel modelArray[4];
static pModel current;
static void init (void);
static void setting(int mode,int data);
static void setUp(void);
static void run  (pNodeList pnode,void*paramter);
static char isEnd(pNodeList pnode,void*paramter);
static void end  (pNodeList pnode,void*paramter);
static void destroy(void);
static char*  getModelNormalName(void);
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
	getModelNormalName
};

static const  char*modelName[] = {
	"ÈáºÍ--Soft",
	"ÀËÂþ--Romatic",
	"ºôÎü--Breath",
	"ÐÄÌø--Hearthbeat"
};

void   initModelNormal   (void)
{
	modelArray[0] = getModelNormal_Soft();
	modelArray[1] = getModelNormal_Romatic();
	modelArray[2] = getModelNormal_Breath();
	modelArray[3] = getModelNormal_HeartBeat();
	
	modelArray[0]->init();
	modelArray[1]->init();
	modelArray[2]->init();
	modelArray[3]->init();
	current       = modelArray[0];
	current->setUp();
}

pModel getModelNormal(int _type)
{
	type = _type;
	if(type<=0)type = 0;
	if(type >=3) type = 3;
	current = modelArray[_type];
	
	return (pModel)&model;
}

int    getModelNormalType(void)
{
	return type;
}

static char*  getModelNormalName(void)
{
	return (char*)modelName[type];
}

static void init (void)
{
	current->init();
}

static void setting(int mode,int data)
{
	current->setting(mode,data);
}

static void setUp(void)
{
	current->setUp();
}

static void run  (pNodeList pnode,void*paramter)
{
	current->run(pnode,paramter);
}

static char isEnd(pNodeList pnode,void*paramter)
{
	return current->isEnd(pnode,paramter);
}

static void end  (pNodeList pnode,void*paramter)
{
	current->end(pnode,paramter);
}

static void destroy(void)
{
	current->destroy();
}

