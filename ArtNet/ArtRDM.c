

#include "ArtRDM.h"
#include "stdio.h"
#include "string.h"

char createArtTodRequest(pArtNode node,pArtTodRequest preq)
{
	int i;
	if(node!=NULL&&preq!=NULL){
		memset(preq,0x00,sizeof(ArtTodRequest));
		strcpy((char*)preq->ID,ARTNET_STRING);
		preq->OpCode = OpTodReques;
		preq->ProtVerHi = 0x00;//node->VersInfoH;
		preq->ProtVerLo	= 0x0e;//node->VersInfoL;
		preq->Command		= ArtTodCmd_TodFull;
		
		preq->AddCount = ARTNET_MAX_PORTS;
		for(i=0;i<ARTNET_MAX_PORTS;i++){
			preq->Address[i] = node->SwOut[i];
		}
		return 1;
	}
	return 0;
}

char createArtTodControl(pArtNode node,pArtTodControl pctl,ArtTodCmd cmd,char address)
{
	int i;
	if(node != NULL&&pctl!=NULL){
		memset(pctl,0x00,sizeof(ArtTodControl));
		strcpy((char*)pctl->ID,ARTNET_STRING);
		pctl->OpCode = OpTodReques;
		pctl->ProtVerHi = 0x00;//node->VersInfoH;
		pctl->ProtVerLo	= 0x0e;//node->VersInfoL;
		pctl->Command		= cmd;
		
		for(i=0;i<ARTNET_MAX_PORTS;i++){
			pctl->Address = address;
		}
		return 1;
	}
	return 0;
}

