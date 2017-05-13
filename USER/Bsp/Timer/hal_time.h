

#ifndef __HAL__TIME__H
#define __HAL__TIME__H

#include "stdlib.h"

#define HAL_GENERAL_TIM           		TIM2
#define HAL_GENERAL_TIM_CLK       		RCC_APB1Periph_TIM2

#define HAL_GENERAL_TIM_IRQn					TIM2_IRQn
#define HAL_GENERAL_TIM_IRQHandler    TIM2_IRQHandler

typedef void(*timerCB)(void);

typedef struct ITimer{

	void (*initTimer) (int usTick);
	char (*startTimer)(int secTick,int msTick,int usTick,timerCB func);
	char (*startTimerSec)(int secTick,int usTick,timerCB func);
	void (*endTimer)  (void);
	
}ITimer,*pITimer;

pITimer getTimerHandle(void);

#endif
