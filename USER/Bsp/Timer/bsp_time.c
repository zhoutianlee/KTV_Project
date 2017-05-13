

#include "bsp_time.h"
#include "stm32f4xx.h"
#include "includes.h"

#pragma pack(push)
#pragma pack(1)
typedef struct timeHeader{
	char          reload:1;
	char          used:1;
	char          start:1;
	void*         parameter;
	unsigned long saveTick;
	unsigned long tick;
	timeCB        func;
}timeHeader,*ptimeHeader;

typedef struct timeList{
	
	timeHeader   header;
	
	struct timeList*pNext;
	struct timeList*pPrev;
	
}timeList,*ptimeList;

#pragma pack(pop)
// variable
static ptimeList header;
static char      start;
static int       tusTick;
static unsigned long tsecTick;
static unsigned long tmsTick;
static char      lock;
static unsigned long tick;
// local function
static void   initTimeBSP(int usTick);
static void   initNVICBSP(void);
static void   runTick(void);
//static ptimeList getTimeList(char reload,unsigned long id,unsigned long tick,timeCB func);

void       		initTimeUp(int usTick)
{
	initTimeBSP(usTick);
	initNVICBSP();
	tusTick = usTick;
	tmsTick  = 1000/usTick;
	tsecTick = 1000000/usTick;
	lock = 0;
	start = 0;
	tick = 0;
}

unsigned long getTimeTick(unsigned long sec,unsigned long ms,unsigned long us)
{
	return tsecTick*sec+tmsTick*ms+us/tusTick;
}

//TimeHeader*		createTime(char reload,unsigned long id,unsigned long tick,timeCB func)
//{
//	return (TimeHeader*)getTimeList(reload,id,tick,func);
//}

void          startTime(void*time)
{
	if(time!= NULL){
		
		((ptimeList)time)->header.start = 1;
		
		lock = 1;
		if(header!= NULL){
			((ptimeList)time)->pNext = header;
			header->pPrev = ((ptimeList)time);
		}
		header = ((ptimeList)time);
		lock = 0;
		if(start == 0){
			start = 1;
			TIM_Cmd(GENERAL_TIM, ENABLE);	            // ʹ�ܶ�ʱ��
		}
	}
}

char       		closeTime(TimeHandle*time)
{
	ptimeList list,ptemp;
	if(time != NULL){
	((ptimeList)time)->header.used = 0;						// ���û����ĵ��õ�closeTime�ᱻ���������ڴ�
	}
	if(lock == 0){
		lock = 1;
		list = header;
		while(list !=NULL){
			if(list->header.used ==0){
				ptemp = list;
				if((list = list->pNext)!= NULL){
					list->pPrev =ptemp->pPrev;
				}
				if(ptemp->pPrev !=NULL){
					ptemp->pPrev->pNext = list;
				}
				if(ptemp == header){
					header = list;
				}
				free(ptemp);
			}else{
				list = list->pNext;
			}
		}
		lock = 0;
	}
	if(header == NULL){
		TIM_Cmd(GENERAL_TIM, DISABLE);	            // ʹ�ܶ�ʱ��
	}
	return 1;
}

TimeHandle* createTime(char reload,void*parameter,unsigned long tick,timeCB func)
{
	ptimeList plist = (ptimeList)malloc(sizeof(timeList));
	if(plist != NULL){
		plist->header.parameter = parameter;
		plist->header.reload = reload;
		plist->header.saveTick = tick;
		plist->header.tick = tick;
		plist->header.start = 0;
		plist->header.used = 1;
		plist->header.func = func;
		plist->pNext = NULL;
		plist->pPrev = NULL;
	}
	return (TimeHandle*)plist;
}
TimeHandle*   createTimeSec(char reload, void*parameter,unsigned long sec,timeCB func)
{
	ptimeList plist = (ptimeList)malloc(sizeof(timeList));
	if(plist!= NULL){
		sec *=tsecTick;
		plist->header.parameter = parameter;
		plist->header.reload = reload;
		plist->header.saveTick = sec;
		plist->header.tick = sec;
		plist->header.start = 0;
		plist->header.used = 1;
		plist->header.func = func;
		plist->pNext = NULL;
		plist->pPrev = NULL;
	}
	return (TimeHandle*)plist;
	
}

TimeHandle*   createTimeMSec(char reload, void*parameter,unsigned long msec,timeCB func)
{
	ptimeList plist = (ptimeList)malloc(sizeof(timeList));
	if(plist!= NULL){
		msec *=tmsTick;
		plist->header.parameter= parameter;
		plist->header.reload = reload;
		plist->header.saveTick = msec;
		plist->header.tick = msec;
		plist->header.start = 0;
		plist->header.used = 1;
		plist->header.func = func;
		plist->pNext = NULL;
		plist->pPrev = NULL;
	}
	return (TimeHandle*)plist;
}

void          modifyTime(void*timeHandle,unsigned long tick)
{
	if(timeHandle!= NULL){
		((ptimeList)timeHandle)->header.tick = tick+1;
		((ptimeList)timeHandle)->header.used = 1;
	}
}
// ���ж�ʱ��
static void   runTick(void)
{
	ptimeList list=header;
	
	if(lock ==0){
		lock = 1;
		while(list!= NULL){
			if(list->header.used == 1&&
				 list->header.start==1&&--list->header.tick==0){
				 if(list->header.reload ==1){
					list->header.tick = list->header.saveTick;
				 }else{
					list->header.used = 0;
				 }
				 list->header.func(list->header.parameter);
			}
				 list = list->pNext;
		}
		lock = 0;
	}
	
}

static void   initTimeBSP(int usTick)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = usTick-1;       
	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=90MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(GENERAL_TIM,TIM_IT_Update,ENABLE);
}

static void   initNVICBSP(void)
{
		NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM5_IRQHandler(void)
{
	OSIntEnter();
	GENERAL_TIM->SR   &= ~TIM_IT_Update;
	tick++;
	runTick();
	OSIntExit();
}

int           getmsTick(void)
{
	return tick;
}

void          setmsTick(int _tick)
{
	tick = _tick;
}
