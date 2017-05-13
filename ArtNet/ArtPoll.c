

#include "ArtPoll.h"
#include "string.h"
#include "ArtNetType.h"

static const char* IDConst = "Art-Net";

pArtPoll 			createArtPoll(char*destArray)
{
	pArtPoll p = (pArtPoll)destArray;
	strcpy((char*)p->ID,IDConst);
	p->OpCode  = OpPoll;
	p->ProtVerHi = 0X00;
	p->ProtVerLo = 0x0e;
	p->Priority  = 0x00;
	return p;
}

pArtPollReply createArtPollReply(char*destArray)
{
	pArtPollReply p = (pArtPollReply)destArray;
	strcpy((char*)p->ID,IDConst);
	p->OpCode = OpPollReply;
	return p;
}
