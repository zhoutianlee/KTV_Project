

#include "Model_Techo.h"
#include "Model_Techo_Dubstep.h"
#include "Model_Techo_Trance.h"
#include "Model_Techo_Electp.h"
#include "Model_Techo_DeepHouse.h"

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
static char*  getModelTechoName(void);
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
	getModelTechoName
};

static const  char*modelName[] = {
	"DubStep",
	"Trance",
	"Electp",
	"DeepHouse"
};

void   initModelTecho   (void)
{
	modelArray[0] = getModelTecho_DubStep();
	modelArray[1] = getModelTecho_Trance();
	modelArray[2] = getModelTecho_Electp();
	modelArray[3] = getModelTecho_DeepHouse();
	
	modelArray[0]->init();
	modelArray[1]->init();
	modelArray[2]->init();
	modelArray[3]->init();
	current       = modelArray[0];
}

pModel getModelTecho(int _type)
{
	type = _type;
	if(_type <4)current = modelArray[_type];
	return (pModel)&model;
}

int    getModelTechoType(void)
{
	return type;
}

static char*  getModelTechoName(void)
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
