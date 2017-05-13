

#ifndef __SERVER__H
#define __SERVER__H

#include <includes.h>
#include <ff.h>
#include "api.h"

extern OS_SEM 			semNetworkState;				// ��������״̬
extern OS_SEM			  semFileState;						// �ļ�ϵͳ����״̬
extern FATFS				fs;											// ���ڹ����ļ�ϵͳ�
extern uint32_t     IPaddress;							// ����ͨ��DHCP�������� IP����Ϊ����IP

typedef void (*NetUDPCallback)(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port);


#ifdef  __cplusplus
extern  "C"{
#endif
	
void   initServer(void*p_arg);							// ��ϵͳ��ʼ����Ϻ���е���
void   setUDPListener(int port,NetUDPCallback func);
struct udp_pcb*getUDPArtNet(void);	
#ifdef  __cplusplus	
}
#endif

#endif
