/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Network connection configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "netconf.h"
#include <stdio.h>
#include "stm32f429_phy.h"
#include "tcpip.h"
#include "Server.h"
#include "ethernetif.h"

/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif gnetif;
struct netif gnetif2;
struct netif gnetif3;

//static err_t ethernetif_init_ex(struct netif *netif);
uint32_t IPaddress = 0;

#ifdef USE_DHCP
#include "os.h"
#include "./Bsp/led/bsp_led.h" 
__IO uint8_t DHCP_state;
#endif
extern __IO uint32_t  EthStatus;

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Initializes the lwIP stack
* @param  None
* @retval None
*/
void LwIP_Init(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
	
  /* Create tcp_ip stack thread */
  tcpip_init( NULL, NULL );	
  
#ifdef USE_DHCP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
#else
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif  
	
  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
  struct ip_addr *netmask, struct ip_addr *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))

  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/

//	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3_0);
//	netif_add(&gnetif2,&ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
//	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3_1);
//	netif_add(&gnetif3,&ipaddr, &netmask, &gw, NULL, &ethernetif_init1, &tcpip_input);
//	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  /*  Registers the default network interface.*/
  netif_set_default(&gnetif);

  if (EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG))
  { 
    /* Set Ethernet link flag */
    gnetif.flags |= NETIF_FLAG_LINK_UP;

    /* When the netif is fully configured this function must be called.*/
		
    netif_set_up(&gnetif);
//		netif_set_up(&gnetif2);
//		netif_set_up(&gnetif3);
#ifdef USE_DHCP
    DHCP_state = DHCP_START;
#else
#ifdef SERIAL_DEBUG
		printf("\n  Static IP address   \n");
		printf("IP: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
		printf("NETMASK: %d.%d.%d.%d\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
		printf("Gateway: %d.%d.%d.%d\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* SERIAL_DEBUG */
#endif /* USE_DHCP */
  }
  else
  {
    /*  When the netif link is down this function must be called.*/
    netif_set_down(&gnetif);
//		netif_set_down(&gnetif2);
//		netif_set_down(&gnetif3);
#ifdef USE_DHCP
    DHCP_state = DHCP_LINK_DOWN;
		
#endif /* USE_DHCP */
#ifdef SERIAL_DEBUG
		printf("\n  Network Cable is  \n");
		printf("    not connected   \n");
#endif /* SERIAL_DEBUG */
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ETH_link_callback);
//	netif_set_link_callback(&gnetif2, ETH_link_callback);
//	netif_set_link_callback(&gnetif3, ETH_link_callback);
}



/**
* @brief  LwIP_DHCP_Process_Handle
* @param  None
* @retval None
*/
void LwIP_DHCP_task(void * pvParameters)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
	OS_ERR  os_err;
#ifdef USE_DHCP	
	while(1)
	{
		switch (DHCP_state)
		{
		case DHCP_START:
			{
				DHCP_state = DHCP_WAIT_ADDRESS;
				dhcp_start(&gnetif);
				/* IP address should be set to 0 
					 every time we want to assign a new DHCP address */
				IPaddress = 0;
#ifdef SERIAL_DEBUG
				printf("\n     Looking for    \n");
				printf("     DHCP server    \n");
				printf("     please wait... \n");
#endif /* SERIAL_DEBUG */
			}
			break;

		case DHCP_WAIT_ADDRESS:
			{
				/* Read the new IP address */
				IPaddress = gnetif.ip_addr.addr;

				if (IPaddress!=0) 
				{
					DHCP_state = DHCP_ADDRESS_ASSIGNED;	

					/* Stop DHCP */
					dhcp_stop(&gnetif);

#ifdef SERIAL_DEBUG
					printf("\n  IP address assigned \n");
					printf("    by a DHCP server   \n");
					printf("IP: %d.%d.%d.%d\n",(uint8_t)(IPaddress),(uint8_t)(IPaddress >> 8), \
																 (uint8_t)(IPaddress >> 16),(uint8_t)(IPaddress >> 24));
					printf("NETMASK: %d.%d.%d.%d\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
					printf("Gateway: %d.%d.%d.%d\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
					LED1_ON;
#endif /* SERIAL_DEBUG */
					// 发出信号量
					OSSemPost(&semNetworkState,OS_OPT_POST_ALL|OS_OPT_POST_NO_SCHED,&os_err);
				} 
				else
				{
					/* DHCP timeout */
					if (gnetif.dhcp->tries > MAX_DHCP_TRIES)
					{
						DHCP_state = DHCP_TIMEOUT;

						/* Stop DHCP */
						dhcp_stop(&gnetif);

						/* Static address used */
						IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
						IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
						IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
						netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);

#ifdef SERIAL_DEBUG
						printf("\n    DHCP timeout    \n");
						printf("  Static IP address   \n");
						printf("IP: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
						printf("NETMASK: %d.%d.%d.%d\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
						printf("Gateway: %d.%d.%d.%d\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
						OSSemPost(&semNetworkState,OS_OPT_POST_ALL|OS_OPT_POST_NO_SCHED,&os_err);
						LED1_ON;
#endif /* SERIAL_DEBUG */
					}
				}
			}
			break;
		default: break;
		}
		OSTimeDlyHMSM( 0u, 0u, 0u, 250u,OS_OPT_TIME_HMSM_STRICT,&os_err);
	}
#else
		OSSemPost(&semNetworkState,OS_OPT_POST_ALL|OS_OPT_POST_NO_SCHED,&os_err);
#endif
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
uint32_t getIPAddress(void)
{
	return IPaddress;
}
//void ethernetif_input( void * pvParameters )
//{
//  struct pbuf *p;
//  OS_ERR os_err;
//  err_t err;
//  
//  /* move received packet into a new pbuf */
//  while (1)
//  {
//		SYS_ARCH_DECL_PROTECT(sr);
//		
//		SYS_ARCH_PROTECT(sr);
//		// 关键是s_pxNetIf
//    p = low_level_input(s_pxNetIf);
//		SYS_ARCH_UNPROTECT(sr);
//    if (p == NULL)	
//      continue;
//		err = s_pxNetIf->input(p, s_pxNetIf);
//		if (err != ERR_OK){
//			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//			pbuf_free(p);
//			p = NULL;
//		}
//    /*sleep 5 ms*/ 
//    OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_DLY, (OS_ERR *)&os_err);
//  }
//}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
