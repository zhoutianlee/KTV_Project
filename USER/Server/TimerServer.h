

#ifndef __TIMERSERVER__H
#define __TIMERSERVER__H

#include "IList.h"

typedef void  (*timerServer)(void*parameter);

typedef struct TimerServer{
	pList				listObject;
	
	uint32_t 		tick;
	uint32_t 		saveTick;
	
	timerServer func;
	void*				parameter;
	uint8_t			reload;
	uint8_t			destroy;
}TimerServer,*pTimerServer;

extern pListHandle     listTSHandle;

void	 initListTimerServer(void);
void   appendTimerServer(uint32_t tick,timerServer func,void*parameter,uint8_t reload);
void   runTimerServer		(void);
void   sortTimerServer (void);												// 用于回收内存

#endif

