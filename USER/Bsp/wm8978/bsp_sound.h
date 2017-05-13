

#ifndef __BSP_SOUND__H
#define __BSP_SOUND__H

#include "stm32f4xx.h"

void 		initSound(void);
void    setSound(char bitEnable);
void    setSoundCB(void(*)(int16_t *,int16_t len));

#endif
