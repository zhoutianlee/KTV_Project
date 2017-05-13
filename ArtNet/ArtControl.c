
#include "ArtControl.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define OLDTICK					10000													// oldTick用于重启设备
#define DISCONNECTTICK	10															// 用于断开设备

static pArtDevice phead= NULL;

char addArtDevice(pArtPollReply reply)
{
	pArtDevice pdevice;
	if(reply!=NULL ){
		if((pdevice=searchDeviceReply(reply))==0){
			pdevice = (pArtDevice)malloc(sizeof(ArtDevice));
			if(pdevice != NULL){
				memcpy(&pdevice->node,((char*)reply)+10,sizeof(ArtNode));// 内存拷贝
				pdevice->connected = 1;
				pdevice->oldTime = 0;
				pdevice->tick = 0;
				pdevice->count =0;
				pdevice->start = 0;
				pdevice->status = 0;
				pdevice->pNext = phead;
				phead = pdevice;

#if	ARTNET_DEBUG				
				printf("add device success\r\n");
				printf("device ip = %d:%d:%d:%d\r\n",reply->IPAddress[0],reply->IPAddress[1],reply->IPAddress[2],reply->IPAddress[3]);
				printf("device shortName = %s\r\n",pdevice->node.ShortName);
				printf("device longName = %s\r\n",pdevice->node.LongName);
#endif			
				
				return 1;
			}else{
#if	ARTNET_DEBUG		
				printf("device malloc error\r\n");
#endif				
				return 0;
			}
		}else{
			// 重新刷新数据
			pdevice->tick = 0;
#if	ARTNET_DEBUG		
			printf("decice flush %s\r\n",pdevice->node.ShortName);
#endif			
//			printf("decice flush %s\r\n",pdevice->node.ShortName);
		}
		return 0;
	}
	return 0;
}

char oldArtDevice(void)
{
	char flag = 0;
	pArtDevice ptemp,plink = phead;
	ptemp = NULL;
	while(plink){
		if(plink->tick++ > DISCONNECTTICK){
#if	ARTNET_DEBUG
				printf("IP = %d:%d:%d:%d\tdevice disconnected..\r\n",plink->node.IPAddress[0],
				plink->node.IPAddress[1],
				plink->node.IPAddress[2],
				plink->node.IPAddress[3]);
#endif	
			if(ptemp == NULL){
				phead = plink->pNext;
				free(plink);
				plink = phead;
			}else{
				ptemp->pNext = plink->pNext;
				free(plink);
				plink = ptemp->pNext;
			}
			
			continue;
		}else{
			plink->oldTime++;
			if(plink->oldTime > OLDTICK){
				plink->status = 0;
				plink->count = 0;
				plink->start = 0;
				plink->tick  = 0;
				plink->oldTime = 0;

#if	ARTNET_DEBUG
				printf("IP = %d:%d:%d:%d\tdevice restart..\r\n",plink->node.IPAddress[0],
				plink->node.IPAddress[1],
				plink->node.IPAddress[2],
				plink->node.IPAddress[3]);
#endif				
			}
		}
		ptemp = plink;
		plink = ptemp->pNext;
	}
	return flag;
}

pArtDevice getArtDeviceHead(void)
{
	return phead;
}

pArtDevice searchDeviceReply(pArtPollReply reply)
{
	pArtDevice plink = phead;
	
	while(plink!= NULL){
		if(plink->node.IPAddress[0] == reply->IPAddress[0]&&
			 plink->node.IPAddress[1] == reply->IPAddress[1]&&
			 plink->node.IPAddress[2] == reply->IPAddress[2]&&
			 plink->node.IPAddress[3] == reply->IPAddress[3]){
				return plink;
			 }
			 plink = plink->pNext;
	}
	return plink;
	
}

pArtDevice searchArtDevice(Int8*IPAddress)
{
	pArtDevice plink = phead;
	
	while(plink!= NULL){
		if(plink->node.IPAddress[0] == IPAddress[0]&&
			 plink->node.IPAddress[1] == IPAddress[1]&&
			 plink->node.IPAddress[2] == IPAddress[2]&&
			 plink->node.IPAddress[3] == IPAddress[3]){
				return plink;
			 }
			 plink = plink->pNext;
	}
	return plink;
}

