

#ifndef __ARTIPPROG__H
#define __ARTIPPROG__H

#include "ArtNetType.h"

typedef struct ArtIpProg{

	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	Int8  Filler1;
	Int8  Filler2;
	
	Int8  Command;
	Int8  Filler4;
	
	Int8  ProgIpHi;
	Int8  ProgIp2;
	Int8  ProgIp1;
	Int8  ProgIpLo;
	
	Int8  ProgSmHi;
	Int8  ProgSm2;
	Int8  ProgSm1;
	Int8  ProgSmLo;
	
	Int8  ProgPortHi;
	Int8  ProgPortLo;
	
	Int8  Spare[8];
}ArtIpProg,*pArtIpProg;


typedef struct ArtIpProgReply{
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	
	Int8  Filler1;
	Int8  Filler2;
	Int8  Filler3;
	Int8  Filler4;
	
	Int8  ProgIpHi;
	Int8  ProgIp2;
	Int8  ProgIp1;
	Int8  ProgIpLo;
	
	Int8  ProgSmHi;
	Int8  ProgSm2;
	Int8  ProgSm1;
	Int8  ProgSmLo;
	
	Int8  ProgPortHi;
	Int8  ProgPortLo;
	
	Int8  Status;				//	 bit6=1,DHCP enable
	
	Int8  Spare[7];
}ArtIpProgReply,*pArtIpProgReply;

char createArtIpProg(pArtNode node,pArtIpProg p);
char createArtIpProgReply(pArtNode node,pArtIpProgReply p);

#endif
