

#include "ArtDmx.h"
#include "stdio.h"
#include "string.h"

char createArtDmx(pArtNode node,pArtDmx pdmx,Int16 len,Int8*pDmx512)
{
	int i;
	if(node!=NULL&&pdmx!=NULL){
		memset(pdmx,0x00,sizeof(ArtDmx));
		strcpy((char*)pdmx->ID,ARTNET_STRING);
		pdmx->OpCode = OpDmx;
		pdmx->ProtVerHi = 0x00;//node->VersInfoH;
		pdmx->ProtVerLo	= 0x0e;//node->VersInfoL;
		
		pdmx->LengthHi = (len>>8)&0xff;
		pdmx->Length	 = len;
		
		pdmx->Physical = 0;
		
		for(i=0;i<len;i++){
			pdmx->Data[i] = pDmx512[i];
		}
		return 1;
	}
	return 0;
}

void initArtDmx(pArtDmx pdmx)
{
	if(pdmx !=NULL){
		pdmx->OpCode = OpDmx;
		pdmx->ProtVerHi = 0x00;//node->VersInfoH;
		pdmx->ProtVerLo	= 0x0e;//node->VersInfoL;
		
		pdmx->Physical = 0;
		strcpy((char*)pdmx->ID,ARTNET_STRING);
	}
}
