
#ifndef __ARTNET__H
#define __ARTNET__H

#include "includes.h"
#include "ArtNetType.h"
#include "ArtPoll.h"
#include "NetConf.h"
#include "ArtControl.h"
#include "ArtIpProg.h"
#include "ArtRDM.h"
#include "ArtDmx.h"



typedef union ArtNetPackages{
	
	ArtTodRequest 	artTodRequest;
	ArtTodData			artTodData;
	ArtTodControl 	artTodControl;
	
	ArtIpProg				artIpProg;
	ArtIpProgReply 	artIpProgReply;
	
	ArtPoll					artPoll;
	ArtPollReply		artPollReply;
	
	ArtDmx					artDmx;
	ArtNzs					artNzs;
	
	u8              buf[ARTNET_BUFMAX];
}ArtNetPackages;

typedef struct ArtNet{

	u8  ID[8];
	u16 OpCodes;
	ArtNetPackages packages;
}ArtNet,*pArtNet;



void initArtNet(void*parg);

#endif
