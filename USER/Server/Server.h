

#ifndef __SERVER__H
#define __SERVER__H

#include <includes.h>
#include <ff.h>
#include "api.h"

extern OS_SEM 			semNetworkState;				// ÍøÂçÆô¶¯×´Ì¬
extern OS_SEM			  semFileState;						// ÎÄ¼şÏµÍ³´îÔØ×´Ì¬
extern FATFS				fs;											// ÓÃÓÚ¹ÒÔÚÎÄ¼şÏµÍ³¸
extern uint32_t     IPaddress;							// ±¾µØÍ¨¹ıDHCP·şÎñ·ÖÅäµÄ IP£¬¼´Îª±¾»úIP

typedef void (*NetUDPCallback)(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port);


#ifdef  __cplusplus
extern  "C"{
#endif
	
void   initServer(void*p_arg);							// ÔÚÏµÍ³³õÊ¼»¯Íê±Ïºó½øĞĞµ÷ÓÃ
void   setUDPListener(int port,NetUDPCallback func);
struct udp_pcb*getUDPArtNet(void);	
#ifdef  __cplusplus	
}
#endif

#endif
