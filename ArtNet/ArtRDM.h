

#ifndef __ARTRDM__H
#define __ARTRDM__H

#include "ArtNetType.h"

typedef enum	ArtTodCmd{
	ArtTodCmd_AtcNone = 0x0,
	ArtTodCmd_TodFull  = 0x0,
	ArtTodCmd_AtcFlush = 0x01
}ArtTodCmd;

typedef struct ArtTodRequest{

	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	Int8  Filler1;
	Int8  Filler2;
	
	Int8	Spare1;
	Int8  Spare2;
	Int8  Spare3;
	Int8	Spare4;
	Int8  Spare5;
	Int8  Spare6;
	Int8 	Spare7;
	
	Int8	Net;
	Int8	Command;
	Int8	AddCount;
	Int8  Address[32];
}ArtTodRequest,*pArtTodRequest;

typedef struct ArtTodData{
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	Int8  RdmVer;
	Int8  Port;
	
	Int8	Spare1;
	Int8  Spare2;
	Int8  Spare3;
	Int8	Spare4;
	Int8  Spare5;
	Int8  Spare6;
	Int8 	BindIndex;
	
	Int8	Net;
	Int8	Command;
	Int8  Address;
	Int8	UidTotalHi;
	Int8  UidTotalLo;
	Int8  BlockCount;
	Int8	UidCount;
	Int8	ToD[6];
}ArtTodData,*pArtTodData;

typedef struct ArtTodControl{
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	Int8  Filler1;
	Int8  Filler2;
	
	Int8	Spare1;
	Int8  Spare2;
	Int8  Spare3;
	Int8	Spare4;
	Int8  Spare5;
	Int8  Spare6;
	Int8 	Spare7;
	
	Int8	Net;
	Int8	Command;
	Int8  Address;
}ArtTodControl,*pArtTodControl;

char createArtTodRequest(pArtNode node,pArtTodRequest preq);
char createArtTodControl(pArtNode node,pArtTodControl pctl,ArtTodCmd cmd,char address);

#endif
