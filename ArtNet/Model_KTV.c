

#include "Model_KTV.H"



#include "Model_KTV.h"
#include "Model_KTV_Soft.h"
#include "Model_KTV_Rythem.h"
#include "Model_KTV_Romatic.h"
#include "Model_KTV_Dynamic.h"
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
static char*  getModelKTVName(void);
static const Model model = {
	init,
	setting,
	setUp,
	run,
	isEnd,
	end,
	destroy,
	getModelKTVName
};

static  char*modelName[] = {
	"抒情--Soft",
	"浪漫--Romatic",
	"律动--Rythem",
	"动感--Dynamic"
};

void   initModelKTV   (void)
{
	modelArray[0] = getModelKTV_Soft();
	modelArray[1] = getModelKTV_Romatic();
	modelArray[2] = getModelKTV_Rythem();
	modelArray[3] = getModelKTV_Dynamic();
	
	modelArray[0]->init();
	modelArray[1]->init();
	modelArray[2]->init();
	modelArray[3]->init();
	current       = modelArray[0];
}

pModel getModelKTV(int _type)
{
	type = _type;
	if(_type <4)current = modelArray[_type];
	return (pModel)&model;
}

int    getModelKTVType(void)
{
	return type;
}

static char*  getModelKTVName(void)
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

