
#include "TimerServer.h"
#include "ArtMem.h"
#include "ArtNetType.h"

static  void*				__malloc		 (uint16_t    size);
static  void			  __free			 (void*ptr);

pListHandle     listTSHandle;

static const IList    listCBFunc={
	__malloc,
	__free
};

void	 initListTimerServer(void)
{
	listTSHandle = listFunc.createHandle(sizeof(TimerServer),(pIList)&listCBFunc);
}

static  void*				__malloc		 (uint16_t    size){return memFunc.getMem(timeMemHandle,size);}
static  void			  __free			 (void*ptr){memFunc.putMem(timeMemHandle,ptr);}

void   appendTimerServer(uint32_t tick,timerServer func,void*parameter,uint8_t reload)
{
	pTimerServer plink;
	if(func == NULL)return;
	plink = (pTimerServer)listFunc.createObject(listTSHandle);
	if(plink != NULL){
		plink->tick = tick;
		plink->saveTick = tick;
		plink->reload = reload;
		plink->parameter = parameter;
		plink->func			 = func;
		plink->destroy	 = 0;
		listFunc.appendObject(listTSHandle,(pList)plink);
	}
}

void   runTimerServer		(void)
{
	pTimerServer plink  =(pTimerServer)listFunc.getFirst(listTSHandle);
	while(plink){
		if(plink->destroy ==0 &&--plink->tick ==0){
			
			if(plink->reload ==0)plink->destroy =1;
			else plink->tick = plink ->saveTick;
			plink->func(plink->parameter);
			
		}
		plink = (pTimerServer)((pList)plink)->pNext;
	}
}

void   sortTimerServer (void)
{
	pTimerServer plink,ptemp;
	plink = (pTimerServer)listFunc.getFirst(listTSHandle);
	ptemp = NULL;
	while(plink){
		if(plink->destroy){
			ptemp = (pTimerServer)((pList)plink)->pNext;
			listFunc.removeObject(listTSHandle,((pList)plink));
			plink = ptemp;
		}else{
			plink = (pTimerServer)((pList)plink)->pNext;
		}
	}
}

