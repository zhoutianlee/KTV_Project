#include "hal_time.h"
#include "stm32f4xx.h"
#include "includes.h"
static void initTimer(int usTick);
static char startTimer(int secTick,int msTick,int usTick,timerCB func);
static char startTimerSec(int secTick,int usTick,timerCB func);

static void endTimer(void);
static void initTimerBSP(void);
static void initNVICBSP(void);

static  const ITimer timer = {
	initTimer,
	startTimer,
	startTimerSec,
	endTimer
};

static int msTick;
static int secTick;
static timerCB func;
pITimer getTimerHandle(void)
{
	return (pITimer)&timer;
}

static void initTimer(int usTick)
{
	msTick  = 1000;
	secTick = 1000*msTick;
	func    = NULL;
	initTimerBSP();
	initNVICBSP();
}

static char startTimer(int _secTick,int _msTick,int _usTick,timerCB _func)
{
	unsigned long tick = _secTick*secTick+_msTick*msTick+_usTick;
	if(tick ==0)return 0;
	func = _func;
	HAL_GENERAL_TIM->DIER &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  &=~(0X01);// 关闭
	HAL_GENERAL_TIM->ARR   = tick;
	HAL_GENERAL_TIM->CNT   = 0;
	HAL_GENERAL_TIM->EGR   = TIM_PSCReloadMode_Immediate;   
	HAL_GENERAL_TIM->SR   &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->DIER |= TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  |= 0x01;
	return 1;
	
}

static char startTimerSec(int _secTick,int usTick,timerCB _func)
{
	unsigned long tick = _secTick*secTick +usTick;
	if(tick ==0)return 0;
	func = _func;
	HAL_GENERAL_TIM->DIER &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  &=~(0X01);// 关闭
	HAL_GENERAL_TIM->ARR   = tick;
	HAL_GENERAL_TIM->CNT   = 0;
	HAL_GENERAL_TIM->EGR   = TIM_PSCReloadMode_Immediate;   
	HAL_GENERAL_TIM->SR   &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->DIER |= TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  |= 0x01;
	return 1;
}

static void endTimer(void)
{
	HAL_GENERAL_TIM->DIER &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->CR1 &= ~0X01;
	func = NULL;
}

static void initTimerBSP(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(HAL_GENERAL_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 0xffffff-1;       
	
	// 高级控制定时器时钟源TIMxCLK = HCLK/2=90MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(HAL_GENERAL_TIM, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(HAL_GENERAL_TIM, TIM_FLAG_Update);
	
}

static void initNVICBSP(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = HAL_GENERAL_TIM_IRQn; 	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler (void)
{
	OSIntEnter();
	HAL_GENERAL_TIM->SR   &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  &=~(0X01);// 关闭
	if(func != NULL)func();
	OSIntExit();
}

