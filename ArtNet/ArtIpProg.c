
#include "ArtIpProg.h"
#include "stdio.h"
#include "string.h"

char createArtIpProg(pArtNode node,pArtIpProg p)
{
	if(node!= NULL&&p!=NULL){
		memset(p,0x00,sizeof(ArtIpProg));
		strcpy((char*)p->ID,ARTNET_STRING);
		p->OpCode     = OpIpProg;
		p->ProtVerHi  = 0x00;//node->VersInfoH;
		p->ProtVerLo	= 0x0e;//node->VersInfoL;
		
		return 1;
	}
	return 0;
}

char createArtIpProgReply(pArtNode node,pArtIpProgReply p)
{
	if(node!= NULL&&p!=NULL){
		memset(p,0x00,sizeof(ArtIpProgReply));
		strcpy((char*)p->ID,ARTNET_STRING);
		p->OpCode     = OpIpProgReply;
		p->ProtVerHi  = 0x00;//node->VersInfoH;
		p->ProtVerLo	= 0x0e;//node->VersInfoL;
		
		return 1;
	}
	return 0;
}

