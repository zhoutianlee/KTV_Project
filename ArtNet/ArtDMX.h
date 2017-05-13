
#ifndef __ARTDMX__H
#define __ARTDMX__H

#include "ArtNetType.h"

typedef struct ArtDmx{
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	
	Int8  Sequence;
	Int8  Physical;
	
	Int8  SubUni;
	Int8 	Net;
	
	Int8  LengthHi;
	Int8	Length;
	
	Int8	Data[512];
	
 }ArtDmx,*pArtDmx;
 

 typedef struct ArtNzs{
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	
	Int8  Sequence;
	Int8  StartCode;
	
	Int8  SubUni;
	Int8 	Net;
	
	Int8  LengthHi;
	Int8	Length;
	
	Int8	Data[512];
 }ArtNzs,*pArtNzs;
 
char createArtDmx(pArtNode node,pArtDmx pdmx,Int16 len,Int8*pDmx512);
void initArtDmx(pArtDmx pdmx);
 
#endif


