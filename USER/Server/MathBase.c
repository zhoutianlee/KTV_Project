

#include "MathBase.h"

int16_t   getMax(int16_t*pdata,int len)
{
	int16_t max = 0;
	
	while(len--){
		if(max <*pdata)max = *pdata;
		pdata++;
	}
	return max;
}
int16_t   getMin(int16_t*pdata,int len)
{
	int16_t min = 0x7fff;												// È¥µô·ûºÅÎ»
	
	while(len--){
		if(min >*pdata)min = *pdata;
		pdata++;
	}
	return min;
}

