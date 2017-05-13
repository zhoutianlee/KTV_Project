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
	HAL_GENERAL_TIM->CR1  &=~(0X01);// �ر�
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
	HAL_GENERAL_TIM->CR1  &=~(0X01);// �ر�
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

	// ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(HAL_GENERAL_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 0xffffff-1;       
	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=90MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(HAL_GENERAL_TIM, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(HAL_GENERAL_TIM, TIM_FLAG_Update);
	
}

static void initNVICBSP(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = HAL_GENERAL_TIM_IRQn; 	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler (void)
{
	OSIntEnter();
	HAL_GENERAL_TIM->SR   &= ~TIM_IT_Update;
	HAL_GENERAL_TIM->CR1  &=~(0X01);// �ر�
	if(func != NULL)func();
	OSIntExit();
}

