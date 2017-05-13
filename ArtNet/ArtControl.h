

#ifndef __ARTCONTROL__H
#define __ARTCONTROL__H

#include "ArtPoll.h"
#include "stdlib.h"
#include "ArtNetType.h"


typedef struct ArtDevice{

	struct ArtDevice*pNext;
	ArtNode 		 node;
	char         connected;															// 链接状态
	char         status:3;															// 状态标志
	char         count:3;																// 用于发送控制命令的序号
	char         start:1;																// 可用标志
	
	int          tick;																	// 在线数值
	int          oldTime;																// 老化计数
	
}ArtDevice,*pArtDevice;

char addArtDevice(pArtPollReply reply);
char oldArtDevice(void);															// 老化设备
pArtDevice searchArtDevice(Int8*IPAddress);
pArtDevice searchDeviceReply(pArtPollReply reply);
pArtDevice getArtDeviceHead(void);

#endif
