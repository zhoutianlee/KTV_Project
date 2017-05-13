

#ifndef __ARTPOLL__H
#define __ARTPOLL__H
#include "ArtNetType.h"



typedef struct ArtPoll{
	
	Int8  ID[8];
	Int16 OpCode;
	Int8  ProtVerHi;
	Int8  ProtVerLo;
	Int8  TalkToMe;
	Int8  Priority;
}ArtPoll,*pArtPoll;

typedef struct ArtPollReply{

	Int8  ID[8];
	Int16 OpCode;
	Int8  IPAddress[4];
	Int16 Port;
	Int8  VersInfoH;
	Int8  VersInfoL;
	Int8  NetSwitch;
	Int8  SubSwitch;
	Int8  OemHi;
	Int8  Oem;
	Int8  UbeaVersion;
	Int8  Status1;
	Int8  EstaManLo;
	Int8  EstaManHi;
	Int8  ShortName[18];
	Int8  LongName[64];
	Int8  NodeReport[64];
	Int8  NumPortsHi;
	Int8  NumPortsLo;
	Int8  PortTypes [4];
	Int8  GoodInput[4];
	Int8  GoodOutput[4];
	Int8  SwIn[4];
	Int8  SwOut[4];
	Int8  SwVideo;
	Int8  SwMacro;
	Int8  SwRemote;
	Int8  Spare[3];
	Int8  Style;
	Int8  MAC[6];
	Int8  BindIp[4];
	Int8  BindIndex;
	Int8  Status2;
	Int8  Filler[26];
}ArtPollReply,*pArtPollReply;

pArtPoll 			createArtPoll(char*destArray);
pArtPollReply createArtPollReply(char*destArray);


#endif
