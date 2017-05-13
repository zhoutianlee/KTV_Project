

#ifndef __ARTCONTROL__H
#define __ARTCONTROL__H

#include "ArtPoll.h"
#include "stdlib.h"
#include "ArtNetType.h"


typedef struct ArtDevice{

	struct ArtDevice*pNext;
	ArtNode 		 node;
	char         connected;															// ����״̬
	char         status:3;															// ״̬��־
	char         count:3;																// ���ڷ��Ϳ�����������
	char         start:1;																// ���ñ�־
	
	int          tick;																	// ������ֵ
	int          oldTime;																// �ϻ�����
	
}ArtDevice,*pArtDevice;

char addArtDevice(pArtPollReply reply);
char oldArtDevice(void);															// �ϻ��豸
pArtDevice searchArtDevice(Int8*IPAddress);
pArtDevice searchDeviceReply(pArtPollReply reply);
pArtDevice getArtDeviceHead(void);

#endif
