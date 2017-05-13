
#include "NetworkDHCP.h"
#include "netconf.h"

static  OS_TCB                        networkDHCPTCB;
static  CPU_STK                       networkDHCPSTK[APP_CFG_TASK_OBJ_STK_SIZE];

void initNetworkDHCP(void*p_arg)
{
	OS_ERR err;
	LwIP_Init();																								/* initation DHCP */ 
//#ifdef USE_DHCP
	OSTaskCreate(&networkDHCPTCB,                              	/* Create the start task                                */
								"DHCPServer",
								LwIP_DHCP_task,
								0u,
								APP_CFG_TASK_NETWORKDHCP_PRIO,								/* App priority	*/
								&networkDHCPSTK[0u],
								networkDHCPSTK[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							  &err);
	if(err!= OS_ERR_NONE){
		printf("create initNetworkDHCP App error\r\n");
	}
//#endif
}
