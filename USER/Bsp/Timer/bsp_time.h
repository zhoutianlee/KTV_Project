

#ifndef __BSP_TIME__H
#define __BSP_TIME__H

#include "stdlib.h"

#define GENERAL_TIM           		TIM5
#define GENERAL_TIM_CLK       		RCC_APB1Periph_TIM5

#define GENERAL_TIM_IRQn					TIM5_IRQn
#define GENERAL_TIM_IRQHandler    TIM5_IRQHandler

typedef void (*timeCB)(void*parameter);

typedef void* TimeHandle;

void       		initTimeUp(int usTick);
unsigned long getTimeTick(unsigned long sec,unsigned long ms,unsigned long us);
TimeHandle*		createTime(char reload,void*parameter,unsigned long tick,timeCB func);
TimeHandle*   createTimeSec(char reload, void*parameter,unsigned long sec,timeCB func);
TimeHandle*   createTimeMSec(char reload, void*parameter,unsigned long msec,timeCB func);
void          startTime(void*timeHandle);
char       		closeTime(TimeHandle*time);
void          modifyTime(void*timeHandle,unsigned long tick);
int           getmsTick(void);
void          setmsTick(int tick);
#endif
