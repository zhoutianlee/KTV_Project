#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_init1(struct netif *netif);
err_t ethernetif_init2(struct netif *netif);
#endif 
