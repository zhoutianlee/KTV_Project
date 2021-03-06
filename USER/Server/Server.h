

#ifndef __SERVER__H
#define __SERVER__H

#include <includes.h>
#include <ff.h>
#include "api.h"

extern OS_SEM 			semNetworkState;				// 网络启动状态
extern OS_SEM			  semFileState;						// 文件系统搭载状态
extern FATFS				fs;											// 用于挂在文件系统�
extern uint32_t     IPaddress;							// 本地通过DHCP服务分配的 IP，即为本机IP

typedef void (*NetUDPCallback)(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port);


#ifdef  __cplusplus
extern  "C"{
#endif
	
void   initServer(void*p_arg);							// 在系统初始化完毕后进行调用
void   setUDPListener(int port,NetUDPCallback func);
struct udp_pcb*getUDPArtNet(void);	
#ifdef  __cplusplus	
}
#endif

#endif
