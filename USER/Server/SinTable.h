

#ifndef __SINTABLE__H
#define __SINTABLE__H

#include "STM32F4XX.h"

void    	initSinTable(void);
uint16_t  getSinValue(uint16_t i);
uint16_t  getSinValueFl(float angel);
uint16_t  getCosValue(uint16_t i);
uint16_t  getCosValueFl(float angel);

#endif
