

#include "DmxOut.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

typedef struct TABLE{

	LIGHT_TYPE type;
	Int16			 nodeID;
}TABLE,*pTABLE;

#define ARTCONFIG_NUM					sizeof(artNetConfig)/sizeof(ArtNetConfig)
#define CompareIP(ip1,ip2)		(ip1[0]==ip2[0]&&ip1[1]==ip2[1]&&ip1[2]==ip2[2]&&ip1[3]==ip2[3])	
#define TABLESIZE							sizeof(table)/sizeof(TABLE)
	
static void  ChangeIPString(const char*ip,Int8* array);

static Int16 GetNodeData_0(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_1(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_2(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_3(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_4(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_31(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static void  calculateArray_1(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum);
static void  calculateArray_2(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum);
static void  calculateArray_3(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum);
static void  calculateArray_4(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum);

static pNodeList createLineConfig(pArtNetLineConfig pconfig);
static pNodeList createArrayConfig(pArtNetArrayConfig pconfig);

static const 		 TABLE table[6]={
	{LIGHT_TYPE_SOUNDWAVE,9},
	{LIGHT_TYPE_DIAMOND0,4},
	{LIGHT_TYPE_DIAMOND1,5},
	{LIGHT_TYPE_FLOOR,3},
	{LIGHT_TYPE_TOP,1},
	{LIGHT_TYPE_TV,2}
};

static const  ArtNetLineConfig artNetLineConfig[]={
	{1,"192.168.1.103",0,70,	DATATYPE_RBG,GetNodeData_0},
	{1,"192.168.1.103",1,70,	DATATYPE_RBG,GetNodeData_0},
	{1,"192.168.1.103",2,160,	DATATYPE_RBG,GetNodeData_0},//DATATYPE_RGB
	{1,"192.168.1.103",3,170,	DATATYPE_RBG,GetNodeData_0},//DATATYPE_RGB
	
	{1,"192.168.1.101",0,120,	DATATYPE_RBG,GetNodeData_0},
	{1,"192.168.1.101",1,120,	DATATYPE_RBG,GetNodeData_0},
	{1,"192.168.1.101",2,100,	DATATYPE_RBG,GetNodeData_0},
	{1,"192.168.1.101",3,100,	DATATYPE_RBG,GetNodeData_0},
	
	{2,"192.168.1.102",0,170,	DATATYPE_RBG,GetNodeData_0},//DATATYPE_RGB
	{2,"192.168.1.102",1,142,	DATATYPE_RBG,GetNodeData_0},//DATATYPE_RGB
	
	// 地板灯
	{3,"192.168.1.94",0,170,	DATATYPE_RBG,GetNodeData_0},
	{3,"192.168.1.94",1,170,	DATATYPE_RBG,GetNodeData_0},
	{3,"192.168.1.94",2,170,	DATATYPE_RBG,GetNodeData_0},
	{3,"192.168.1.94",3,170,	DATATYPE_RBG,GetNodeData_0},
	{3,"192.168.1.95",4,170,	DATATYPE_RBG,GetNodeData_0},
	{3,"192.168.1.95",5,170,	DATATYPE_RBG,GetNodeData_0},
	
	// 钻石墙
	{4,"192.168.1.100",0,160,	DATATYPE_RBG,GetNodeData_0},
	{4,"192.168.1.100",1,160,	DATATYPE_RBG,GetNodeData_0},
	{4,"192.168.1.100",2,90,	DATATYPE_RBG,GetNodeData_0},
	{4,"192.168.1.100",3,80,	DATATYPE_RBG,GetNodeData_0},
	{0xff}
};

static const  ArtNetArrayConfig artNetArrayConfig[]={
	{5,"192.168.1.110",0 ,36, DATATYPE_RGBW,24 ,6,GetNodeData_4,calculateArray_4,ARRAYMODE_4},
	{5,"192.168.1.111",0 ,36, DATATYPE_RGBW,24 ,6,GetNodeData_4,calculateArray_4,ARRAYMODE_4},
	{5,"192.168.1.112",0 ,36, DATATYPE_RGBW,24 ,6,GetNodeData_4,calculateArray_4,ARRAYMODE_4},
	{5,"192.168.1.113",0 ,36, DATATYPE_RGBW,24 ,6,GetNodeData_4,calculateArray_4,ARRAYMODE_4},
	
	{9,"192.168.1.92",0 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.92",1 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.92",2 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.92",3 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.93",4 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.93",5 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.93",6 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.93",7 ,130,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	
	{9,"192.168.1.96",0 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.96",1 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.96",2 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.96",3 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.97",4 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.97",5 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.97",6 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.97",7 ,130,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	
	{9,"192.168.1.98",0 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.98",1 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.98",2 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.98",3 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.99",4 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.99",5 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.99",6 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.99",7 ,130,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	
	{9,"192.168.1.90",0 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.90",1 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.90",2 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.90",3 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.91",4 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{9,"192.168.1.91",5 ,170,DATATYPE_GRB,80,60,GetNodeData_31,calculateArray_3,ARRAYMODE_3},
	{0xff}
};
static pArray 			phead;
static pNodeList 		plistHead  = NULL;
static pArray 			array[3];
static pPixelHead 	pheadArray[2];

void initArtNetConfig(void)
{
	pNodeList plink;
	pLineNode pnode;
	plistHead = createLineConfig((pArtNetLineConfig)artNetLineConfig);
	if(plistHead != NULL){
		plink = plistHead;
		while(plink->pNext!=NULL){plink=plink->pNext;}
		plink->pNext = createArrayConfig((pArtNetArrayConfig)artNetArrayConfig);

		if(plink->pNext){array[0] = plink->pNext->parray;plink = plink->pNext;}
		if(plink->pNext)array[1] = plink->pNext->parray;
	}
//	plistHead = createArrayConfig((pArtNetArrayConfig)artNetArrayConfig);
//	array[0] = plistHead->parray;
	// floor
	pheadArray[0] = createPixelHead(50,3);plink = getNodeListIndex(3);if(plink == NULL)return;
	pnode = &plink->pHead->lineNode;
	while(pnode){
		addPixelMember(pheadArray[0],pnode);
		pnode = pnode->pNext;
	}
	// diamond
	pheadArray[1] = createPixelHead(20,4);plink = getNodeListIndex(4);if(plink == NULL)return;
	pnode = &plink->pHead->lineNode;
	while(pnode){
		addPixelMember(pheadArray[1],pnode);
		pnode = pnode->pNext;
	}
	
	
//	addPixelMember(pheadArray[0],
}

void          setLightTypeEnable(LIGHT_TYPE type,char status)
{
	Int16 nodeID = 0xff;
	Int16 i;
	pNodeList plink;
	for(i=0;i<TABLESIZE;i++){
		if(table[i].type == type){
			nodeID = table[i].nodeID;
			break;
		}
	}
	
	if(nodeID!= 0XFF){
		plink = plistHead;
		while(plink){
			if(plink->nodeID == nodeID){plink->enable = status;break;}
			plink = plink ->pNext;
		}
	}
}

pPixelHead		getFloorHead(void)
{
	return pheadArray[0];
}

pPixelHead		getDiamondHead(void)
{
	return pheadArray[1];
}

pArray  			getArrayIndex(Int8 index)
{
	return array[index];
}
void          registerNodeList(pArtNode  node)
{
	pNodeList plist = plistHead;
	pUnionNode    punion;
	if(node != NULL ){
		while(plist){
			
			punion = plist->pHead;
			while(punion){
				if(CompareIP(punion->lineNode.msg.IPAddress,node->IPAddress)){
					punion->lineNode.msg.isExit = 1;
				}
				punion = (pUnionNode) punion ->lineNode.pNext;
			}

			plist = plist->pNext;
		}
	}
//	if(flag == 1)printf("device %s register success\r\n",node->ShortName);
//	else printf("device %s register fault\r\n",node->ShortName);
}

pArray				createArray(Int16 width,Int16 heigh,DATATYPE datatype)
{
	pArray plink;
	if(datatype == DATATYPE_RGBW){
		plink = (pArray)memFunc.getMem(artMemHandle,sizeof(Array)+(width+1)*(heigh+1)*sizeof(RGBW));
	}else{
		plink = (pArray)memFunc.getMem(artMemHandle,sizeof(Array)+(width+1)*(heigh+1)*sizeof(RGB));
	}
//	switch(datatype){
//		case DATATYPE_GRB:
//		case DATATYPE_RBG:
//		case DATATYPE_RGB:{
//			plink = (pArray)memFunc.getMem(artMemHandle,sizeof(Array)+(width+1)*(heigh+1)*sizeof(RGB));
//			
//		}break;
//		case DATATYPE_RGBW:{
//			plink = (pArray)memFunc.getMem(artMemHandle,sizeof(Array)+(width+1)*(heigh+1)*sizeof(RGBW));
//		}break;
//	}
	if(plink != NULL){
		plink->type = datatype;
		plink->array= (DataType*)((Int8*)plink)+sizeof(Array);

		plink->width = width;
		plink->heigh = heigh;
	}
	return plink;
}

pNodeList			getNodeListIndex(Int32 nodeID)
{
	pNodeList plink = plistHead;
	
	while(plink!= NULL&&plink->nodeID != nodeID){
		plink = plink->pNext;
	}
	return plink;
}

pNodeList     getNodeListHead(void)
{
	return plistHead;
}

pArray				getArrayHead(void)
{
	return phead;
}
pPixelHead		createPixelHead(Int16	width,Int32 nodeID)
{
	pPixelHead phead = (pPixelHead)memFunc.getMem(artMemHandle,sizeof(PixelHead));//malloc(sizeof(PixelHead));
	if (phead != NULL) {
		phead->nodeID = nodeID;
		phead->heigh = 0;
		phead->width = width;
		phead->pHead = NULL;
		phead->pTail = NULL;
		phead->dlt = 0;
	}
	return phead;
}
char				  addPixelMember(pPixelHead phead,pLineNode pline)
{
	pPixelLine line;
	Int16			 count;
	Int16      width;
	if (phead == NULL)return 0;
	line = (pPixelLine)memFunc.getMem(artMemHandle,sizeof(PixelLine));//malloc(sizeof(PixelLine));
	if (line == NULL)return 0;
	line->pline = pline;
	line->pNext = NULL;
  
	if (phead->pTail == NULL) {
		phead->pHead = line;
	}
	else {
		phead->pTail->pNext = line;
	}
	phead->pTail = line;
	width = pline->len + phead->dlt;
	count = 0;
	while (width >= phead->width) { count++;width -= phead->width; }
	phead->heigh += count;
	phead->dlt = width;
	return 1;
}
void 					getPixelLine(pPixelHead phead,Int16 heigh,pRGB pdata)
{
	Int8 flag = heigh & 0x01;
	pRGB pStart;
	pLineNode plink;
	pPixelLine ptemp;
	Int16 count,len;
	if (phead == NULL) return; 

	if (heigh & 0x01)pdata += phead->width-1;											// ?????
	ptemp = phead->pHead;
	count = 0;

	while (ptemp) {

		plink  = ptemp->pline;
		pStart = ((pRGB)plink->Data)-count;															  // ????
		count += plink->len;

		while (count >= phead->width) {
			
			if (heigh-- == 0) {
				// ??????
				if (count >= phead->width) {
					// ????????????
					count = phead->width;
					if(flag)while(count--)*pdata-- = *pStart++;
					else    while(count--)*pdata++ = *pStart++;
					return;
				}else {
					// ???????
					len = phead->width;
					if (flag) {
						while (len != 0) {
							len -= count;
							while (count--) *pdata-- = *pStart++;
							ptemp = ptemp->pNext;
							if (ptemp == NULL)return;
							plink = ptemp->pline;
							pStart = ((pRGB)plink->Data);								// ??????pStart
							if (plink->len > len)count = len;else count = plink->len;
						}
					}else {
						while (len != 0) {
							len -= count;
							while (count--) *pdata++ = *pStart++;
							ptemp = ptemp->pNext;
							if (ptemp == NULL)return;
							plink = ptemp->pline;
							pStart = ((pRGB)plink->Data);								// ??????pStart
							if (plink->len > len)count = len;else count = plink->len;
						}
					} // else
				}// else
			}//else ????
			count -= phead->width;
			pStart += phead->width;
		}// while????
		if (heigh == 0) {
			// 
			len = phead->width;
			if (flag) {
				while (len) {
					len -= count;
					while (count--) *pdata-- = *pStart++;
					ptemp = ptemp->pNext;
					if (ptemp == NULL)return;
					plink = ptemp->pline;
					pStart = ((pRGB)plink->Data);								// ??????pStart
					if (plink->len > len)count = len;else count = plink->len;
				}
			}else {
				while (len ) {
					len -= count;
					while (count--) *pdata++ = *pStart++;
					ptemp = ptemp->pNext;
					if (ptemp == NULL)return;
					plink = ptemp->pline;
					pStart = ((pRGB)plink->Data);								// ??????pStart
					if (plink->len > len)count = len;else count = plink->len;
				}
			} // else
			return;
		}
		ptemp = ptemp->pNext;
	}// ????,plink == NULL,??????
}
void  				setPixelLine(pPixelHead phead,Int16 heigh,pRGB pdata)
{
	Int8 flag = heigh & 0x01;
	pRGB pStart;
	pLineNode plink;
	pPixelLine ptemp;
	Int16 count,len;
	if (phead == NULL) return; 

	if (heigh & 0x01)pdata += phead->width-1;											// ?????
	ptemp = phead->pHead;
	count = 0;

	while (ptemp) {

		plink  = ptemp->pline;
		pStart = ((pRGB)plink->Data)-count;															  // ????
		count += plink->len;

		while (count >= phead->width) {
			
			if (heigh-- == 0) {
				// ??????
				if (count >= phead->width) {
					// ????????????
					count = phead->width;
					if(flag)while(count--)*pStart++ = *pdata-- ;
					else    while(count--)*pStart++ = *pdata++;
					return;
				}else {
					// ???????
					len = phead->width;
					if (flag) {
						while (len != 0) {
							len -= count;
							while (count--) *pStart++ = *pdata--;
							ptemp = ptemp->pNext;
							if (ptemp == NULL)return;
							plink = ptemp->pline;
							pStart = ((pRGB)plink->Data);								// ??????pStart
							if (plink->len > len)count = len;else count = plink->len;
						}
					}else {
						while (len != 0) {
							len -= count;
							while (count--) *pStart++ = *pdata++;
							ptemp = ptemp->pNext;
							if (ptemp == NULL)return;
							plink = ptemp->pline;
							pStart = ((pRGB)plink->Data);								// ??????pStart
							if (plink->len > len)count = len;else count = plink->len;
						}
					} // else
				}// else
			}//else ????
			count -= phead->width;
			pStart += phead->width;
		}// while????
		if (heigh == 0) {
			// 
			len = phead->width;
			if (flag) {
				while (len) {
					len -= count;
					while (count--) *pStart++ = *pdata--;
					ptemp = ptemp->pNext;
					if (ptemp == NULL)return;
					plink = ptemp->pline;
					pStart = ((pRGB)plink->Data);								// ??????pStart
					if (plink->len > len)count = len;else count = plink->len;
				}
			}else {
				while (len ) {
					len -= count;
					while (count--) *pStart++= *pdata++ ;
					ptemp = ptemp->pNext;
					if (ptemp == NULL)return;
					plink = ptemp->pline;
					pStart = ((pRGB)plink->Data);								// ??????pStart
					if (plink->len > len)count = len;else count = plink->len;
				}
			} // else
			return;
		}
		ptemp = ptemp->pNext;
	}// ????,plink == NULL,??????
}
static pNodeList createLineConfig(pArtNetLineConfig pconfig)
{
	pNodeList  phead;
	pLineNode  plink;
	Int8       nodeID = pconfig->nodeID;
	
	if(pconfig->nodeID== 0xff)return NULL;
	phead = (pNodeList)memFunc.getMem(artMemHandle,sizeof(NodeList));if(phead == NULL)return NULL;
	
	phead->mode   = ARTNETMODE_LINE;
	phead->parray = NULL;
	phead->pNext  = NULL;
	phead->pHead  = NULL;
	phead->pTail	= NULL;
	phead->enable = 1;
	phead->nodeID = nodeID;
	while(nodeID == pconfig->nodeID && pconfig ->nodeID != 0XFF){
		if(pconfig->type == DATATYPE_RGBW)plink = memFunc.getMem(artMemHandle,sizeof(LineNode)+(pconfig->length<<2));
		else plink = memFunc.getMem(artMemHandle,sizeof(LineNode)+pconfig->length*3);if(plink == NULL)return NULL;
		
		plink->Data = (DataType*)(((char*)plink)+sizeof(LineNode));
		plink->func = pconfig->getNodeData;
		plink->pNext= NULL;
		plink->len  = pconfig->length;
		plink->type = pconfig->type;
		
		plink->msg.Sequence = 1;
		plink->msg.Net			= 0;
		plink->msg.SubUni   = pconfig->subUni;
		plink->msg.isExit   = 0;
		ChangeIPString(pconfig->IPAddress,plink->msg.IPAddress);
		
		if(phead->pHead == NULL){
			phead ->pHead = (pUnionNode)plink;
			phead ->pTail = (pUnionNode)plink;
		}else{
			phead->pTail->lineNode.pNext = plink;
		}
		
		phead->pTail    = (pUnionNode)plink;
		
		pconfig ++;
	}
	phead->pNext = createLineConfig(pconfig);
	return phead;
}

static pNodeList createArrayConfig(pArtNetArrayConfig pconfig)
{
	pNodeList  phead;
	pArrayNode plink;
	Int8       nodeID = pconfig->nodeID;
	Int16  		 x,y;
	Int16      count;
	
	if(pconfig->nodeID == 0xff)return NULL;
	phead = (pNodeList)memFunc.getMem(artMemHandle,sizeof(NodeList));if(phead == NULL)return NULL;
	phead->parray = (pArray)memFunc.getMem(artMemHandle,sizeof(Array));if(phead->parray == NULL)return NULL;
	
	if(pconfig->type == DATATYPE_RGBW)phead ->parray->array = (DataType*)memFunc.getMem(artMemHandle,pconfig->width*pconfig->heigh<<2);
	else phead ->parray->array = (DataType*)memFunc.getMem(artMemHandle,pconfig->width*pconfig->heigh*3);
	phead->mode   = ARTNETMODE_ARRAY;
	phead->parray->heigh = pconfig->heigh;
	phead->parray->width = pconfig->width;
	phead->parray->type  = pconfig->type;
	phead->pNext				 = NULL;
	phead->pHead 				 = NULL;
	phead->pTail				 = NULL;
	phead->nodeID				 = nodeID;
	phead->enable				 = 1;
	count = 0;
	while(nodeID== pconfig->nodeID&&pconfig->nodeID!= 0XFF){
		plink = (pArrayNode)memFunc.getMem(artMemHandle,sizeof(ArrayNode));if(plink == NULL)return NULL;
		plink->func = pconfig->getNodeData;
		plink->type = pconfig->type;
		plink->pNext= NULL;
		plink->len					= pconfig->length;
		plink->msg.isExit = 0;
		plink->msg.Sequence = 0;
		plink->msg.SubUni   = pconfig->subUni;
		plink->msg.Net			= 0;
		
		ChangeIPString(pconfig->IPAddress,plink->msg.IPAddress);
		pconfig->calculate(phead->parray->width,phead->parray->heigh,&x,&y,count);
		count += pconfig->length;
		plink->width = x;
		plink->heigh = y;
		
//		printf("x = %d\ty = %d\n",x,y);
		if(plink->type == DATATYPE_RGBW) plink->Data  = (DataType*)((RGBW*)phead->parray->array +(phead->parray->width*y + x));
		else plink->Data  = (DataType*)((RGB*)phead->parray->array +(phead->parray->width*y + x));
		switch(pconfig->mode){
			case ARRAYMODE_1:
			if(y&0x01){
					if(pconfig->type == DATATYPE_RGBW)plink->Data --;
					else plink->Data = (DataType*)((RGB*)plink->Data-1);
			}break;
			case ARRAYMODE_2:break;
			case ARRAYMODE_3:
			if(x&0x01){
				if(pconfig->type == DATATYPE_RGBW)plink->Data -= phead->parray->width;
				else plink->Data = (DataType*)(((RGB*)plink->Data)-phead->parray->width);
			}
			break;
			case ARRAYMODE_4:break;
		}
		
		if(phead->pHead == NULL){
			phead->pHead = (pUnionNode)plink;
			phead->pTail = (pUnionNode)plink;
		}else{
			phead->pTail->arrayNode.pNext = plink;
		}
		phead->pTail = (pUnionNode)plink;
		pconfig++;
	}
	
	phead->pNext = createArrayConfig(pconfig);
	return phead;
}

static void  ChangeIPString(const char*ip,Int8* array)
{
	int i;
	Int8 sum;
	for(i=0;i<4;i++){
		sum = 0;
		while(*ip!='.'&&*ip!=0){
				sum = sum*10- '0'+*ip++ ;
		}
		ip++;
		*array++ = sum;
	}
}

static Int16 GetNodeData_0(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{ 
	Int16 length;
	Int8* pSrc;
	if(pNode->lineNode.type == DATATYPE_RGBW){
		length = pNode->lineNode.len<<2;	
	}else{
		length = pNode->lineNode.len*3;
	}
	
	len    = (len>length)?length:len;
	length = len;
	
	pSrc = (Int8*)pNode->lineNode.Data;
	switch(pNode->lineNode.type){
		case DATATYPE_GRB:{
			// 第一个数据跟第二个数据对调
			len /= 3;
			while(len--){
				*pDest ++ = *(pSrc+1);
				*pDest ++ = *pSrc;
				*pDest ++ = *(pSrc+2);
				
				pSrc     += 3;
			}
		}break;
		case DATATYPE_GBR:{
		 len /= 3;
			while(len--){
				*pDest ++ = *(pSrc+1);
				*pDest ++ = *(pSrc+2);
				*pDest ++ = *(pSrc);
				
				pSrc     += 3;
			}
		}break;
		case DATATYPE_RBG:{
			// 第二个数据跟第三个数据对调
			len /= 3;
			while(len--){
				*pDest ++ = *pSrc;
				*pDest ++ = *(pSrc+2);
				*pDest ++ = *(pSrc+1);
				
				pSrc     += 3;
			}
		}break;
		// 数据不用对调
		case DATATYPE_RGB:
		case DATATYPE_RGBW:{
			while(len--){
				*pDest++ = *pSrc++;
			}
		}break;
	}
	return length;
}

// length 均错误	
	
static Int16 GetNodeData_1(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{
	/*
				1  2  3  4
				8	 7	6	 5
				9 10 11 12
	 */
	char flag   = (((pArrayNode)pNode)->heigh)&0x01;
	Int16 width = ((pArrayNode)pNode)->width;
	Int8*pSrc   = (Int8*)((pArrayNode)pNode)->Data;
	Int16 length ;
	Int16 offset =0;
	Int16 start, end;
	Int16 i;
	start = width;
	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len    = length>>2;
		offset = ((array->width)<<2);
		while(len!=0){
			if (len > array->width)end = array->width;
			else end = len;
			len -= end - start;
			
			if(flag == 0){
				for(i=start;i<end;i++){
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
				}
				pSrc += offset - 4;
				flag = 1;
			}else{
				for(i=start;i<end;i++){
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					pSrc -=4;
				}
				pSrc += offset +4;
				flag  = 0;
			}
			start = 0;
		}
	}else{
		length = ((pArrayNode)pNode)->len*3;
		length = (len>length)?length:len;
		len    = length/3;
		offset = (array->width*3);
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				while(len!=0){
					if (len > array->width)end = array->width;
					else end = len;
					len -= end - start;
					
					if(flag == 0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							pSrc += 3;
						}
						pSrc += offset - 3;
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							pSrc -= 3;
						}
						pSrc += offset +3;
						flag  = 0;
					}
					start = 0;
				}
			}break;
			case DATATYPE_GBR:{
				while(len!=0){
					if (len > array->width)end = array->width;
					else end = len;
					len -= end - start;
					
					if(flag == 0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc);;
							pSrc += 3;
						}
						pSrc += offset - 3;
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc);
							pSrc -= 3;
						}
						pSrc += offset +3;
						flag  = 0;
					}
					start = 0;
				}
				
			}break;
			case DATATYPE_RBG:{
				while(len!=0){
					if (len > array->width)end = array->width;
					else end = len;
					len -= end - start;
					
					if(flag == 0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc+1);
							pSrc += 3;
						}
						pSrc += offset - 3;
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc+1);
							pSrc -= 3;
						}
						pSrc += offset +3;
						flag  = 0;
					}
					start = 0;
				}
				
			}break;
			case DATATYPE_RGB:{
				while(len!=0){
					if (len > array->width)end = array->width;
					else end = len;
					len -= end - start;
					
					if(flag == 0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							pSrc += 3;
						}
						pSrc += offset - 3;
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							pSrc -= 3;
						}
						pSrc += offset +3;
						flag  = 0;
					}
					start = 0;
				}
				
			}break;
				default:{}break;
		}
	}
	return length;
}

static Int16 GetNodeData_2(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{	/*
				1  2  3  4
				5  6  7  8 
				9  10 11 12
	 */
	Int8*pSrc   = (Int8*)((pArrayNode)pNode)->Data;;
	Int16 length ;

	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len    = length;
		
		while(len--){
			*pDest++ = *pSrc++;
		}
		
	}else{
		length = ((pArrayNode)pNode)->len*3;
		length = (len>length)?length:len;
		len    = length/3;
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				while(len--){
					*pDest++ = *(pSrc +1);
					*pDest++ = *(pSrc);
					*pDest++ = *(pSrc+2);
					pSrc += 3;
				}
			}break;
			case DATATYPE_GBR:{
				while(len--){
					*pDest++ = *(pSrc +1);
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc);
					pSrc += 3;
				}
			}break;
			case DATATYPE_RBG:{
				while(len--){
					*pDest++ = *(pSrc )++;
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc);
					pSrc += 2;
				}
			}break;
			case DATATYPE_RGB:{
				while(len--){
					*pDest++ = *(pSrc )++;
					*pDest++ = *(pSrc)++;
					*pDest++ = *(pSrc)++;
				}
			}break;
			default:{}break;
		}
	}
	return length;
}

static Int16 GetNodeData_31(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{
	Int16 i;
	Int8* pSrc;
	Int16 offset;
	Int16 start, end, count;
	char  flag;
	Int16 length;
	if(array == NULL|| pNode == NULL)return 0;
	pSrc = (Int8*)pNode ->arrayNode.Data;
	flag = pNode->arrayNode.width & 0x01;
	
	if (flag == 1)start = array->heigh - pNode->arrayNode.heigh;
	else start = pNode ->arrayNode.heigh;
	
	count = pNode->arrayNode.len;
	
	switch(array->type){
		case DATATYPE_GBR:{
			offset = array->width*3;
			length = count*3;
			while (count != 0) {
				if (count > array->heigh)end = array->heigh;
				else end = count;
				count = count-end + start;
				if (flag == 0) {

					for (i = start;i < end;i++) {

						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc);
						pSrc += offset;//array->width;
					}
					pSrc  =pSrc - offset +3;//array->width+1;
					flag = 1;
				}
				else {

					for (i = start;i < end;i++) {
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc);
						pSrc -= offset;//array->width;
					}
					pSrc = pSrc + offset+3;//array->width +1;
					flag = 0;
				}
				start = 0;
			}
		}break;
		case DATATYPE_GRB:{
			offset = array->width*3;
			length = count*3;
			while (count != 0) {
				if (count > array->heigh)end = array->heigh;
				else end = count;
				count = count- end + start;
				if (flag == 0) {

					for (i = start;i < end;i++) {

						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						pSrc += offset;//array->width;
					} 
					pSrc  =pSrc - offset +3;//array->width+1;
					flag = 1;
				}
				else {

					for (i = start;i < end;i++) {
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						pSrc -= offset;//array->width;
					}
					pSrc = pSrc + offset+3;//array->width +1;
					flag = 0;
				}
				start = 0;
			}
		}break;
		case DATATYPE_RBG:{
			offset = array->width*3;
			length = count*3;
			while (count != 0) {
				if (count > array->heigh)end = array->heigh;
				else end = count;
				count -= end - start;
				if (flag == 0) {

					for (i = start;i < end;i++) {

						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc+1);
						pSrc += offset;//array->width;
					}
					pSrc  =pSrc - offset +3;//array->width+1;
					flag = 1;
				}
				else {

					for (i = start;i < end;i++) {
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc+1);
						pSrc -= offset;//array->width;
					}
					pSrc = pSrc + offset+3;//array->width +1;
					flag = 0;
				}
				start = 0;
			}
		}break;
		case DATATYPE_RGB:{
			offset = array->width*3;
			length = count*3;
			while (count != 0) {
				if (count > array->heigh)end = array->heigh;
				else end = count;
				count -= end - start;
				if (flag == 0) {

					for (i = start;i < end;i++) {

						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						pSrc += offset;//array->width;
					}
					pSrc  =pSrc - offset +3;//array->width+1;
					flag = 1;
				}
				else {

					for (i = start;i < end;i++) {
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						pSrc -= offset;//array->width;
					}
					pSrc = pSrc + offset+3;//array->width +1;
					flag = 0;
				}
				start = 0;
			}
		}break;
		case DATATYPE_RGBW:{
			offset = array->width<<2;
			length = count<<1;
			while (count != 0) {
				if (count > array->heigh)end = array->heigh;
				else end = count;
				count -= end - start;
				if (flag == 0) {

					for (i = start;i < end;i++) {

						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc+3);
						pSrc += offset;//array->width;
					}
					pSrc  =pSrc - offset +4;//array->width+1;
					flag = 1;
				}
				else {

					for (i = start;i < end;i++) {
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc+3);
						pSrc -= offset;//array->width;
					}
					pSrc = pSrc + offset+4;//array->width +1;
					flag = 0;
				}
				start = 0;
			}
		}break;
	}
	return (length>len)?len:length;
}
static Int16 GetNodeData_3(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{
	/*
				1  6  7  12
				2  5  8  11
				3  4  9  10
	 */
	
	char flag   = (((pArrayNode)pNode)->width)&0x01;
	Int8*pSrc   = (Int8*)((pArrayNode)pNode)->Data;//(Int8*)array->array;
	Int16 length ;
	Int16 start,end;
	Int16 i;
	Int16 offset;
	if(flag == 1)start = array->heigh - pNode->arrayNode.heigh;
	else start = ((pArrayNode)pNode)->heigh;
	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len    = length>>2;
		offset = array->width<<2;
		while(len!=0){
			
			if(len >array->heigh)end = array->heigh;
			else end = len;
			len =len -( end-start);
			
			if(flag ==0){
				for(i=start;i<end;i++){
					*pDest++ = *pSrc;
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc+3);	
					pSrc += offset;
				}
				pSrc = pSrc - offset+sizeof(RGBW);
				flag = 1;
			}else{
				for(i=start;i<end;i++){
					*pDest++ = *pSrc;
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc+3);	
					pSrc -= array->width;
				}
				pSrc = pSrc + array->width +sizeof(RGBW);
				flag = 0;
			}
			start =0;
		}

	}else{
		length  = ((pArrayNode)pNode)->len*3;
		length  = (len>length)?length:len;
		
		len     = length/3;
		offset  = array->width*3;
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				
				while(len!=0){
			
					if(len >array->heigh)end = array->heigh;
					else end = len;
					len =len -( end-start);
					
					if(flag ==0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							pSrc += offset;
						}
						pSrc = pSrc - offset+sizeof(RGB);
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc);
							*pDest++ = *(pSrc+2);
							pSrc -= offset;
						}
						pSrc = pSrc + offset +sizeof(RGB);
						flag = 0;
					}
					start =0;
				}
			}break;
			case DATATYPE_GBR:{
				while(len!=0){
			
					if(len >array->heigh)end = array->heigh;
					else end = len;
					len =len -( end-start);
					
					if(flag ==0){
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc);
							pSrc += offset;
						}
						pSrc = pSrc - offset+sizeof(RGB);
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc);
							pSrc -= offset;
						}
						pSrc = pSrc + offset +sizeof(RGB);
						flag = 0;
					}
					start =0;
				}
				
			}break;
			case DATATYPE_RBG:{
				while(len!=0){
			
					if(len >array->heigh)end = array->heigh;
					else end = len;
					len =len -( end-start);
					
					if(flag ==0){
						for(i=start;i<end;i++){
							*pDest++ = *pSrc;
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc+1);

							pSrc += offset;
						}
						pSrc = pSrc - offset+sizeof(RGB);
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *pSrc;
							*pDest++ = *(pSrc+2);
							*pDest++ = *(pSrc+1);

							pSrc -= offset;
						}
						pSrc = pSrc + offset +sizeof(RGB);
						flag = 0;
					}
					start =0;
				}

			}break;
			case DATATYPE_RGB:{
				while(len!=0){
			
					if(len >array->heigh)end = array->heigh;
					else end = len;
					len =len -( end-start);
					
					if(flag ==0){
						for(i=start;i<end;i++){
							*pDest++ = *pSrc;
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);

							pSrc += offset;
						}
						pSrc = pSrc - offset+sizeof(RGB);
						flag = 1;
					}else{
						for(i=start;i<end;i++){
							*pDest++ = *pSrc;
							*pDest++ = *(pSrc+1);
							*pDest++ = *(pSrc+2);


							pSrc -= offset;
						}
						pSrc = pSrc + offset +sizeof(RGB);
						flag = 0;
					}
					start =0;
				}
				
			}break;
			default:{}break;
		}
		
		
	}
	return length;
}
static Int16 GetNodeData_4(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{
	/*
				1  4  7  10
				2  5  8  11
				3  6  9  12
	 */
	Int16 heigh = ((pArrayNode)pNode)->heigh;
	Int8*pSrc   = (Int8*)(Int8*)((pArrayNode)pNode)->Data;//array->array;
	Int8*pSave  = pSrc;
	Int16 length = len;
	Int16 offset0;
	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		
		offset0 = ((array->width)<<2) - 4;
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len = length>>2;
		while(len--){
			*pDest++ = *pSrc++;
			*pDest++ = *pSrc++;
			*pDest++ = *pSrc++;
			*pDest++ = *pSrc++;
			if(heigh ++>= array->heigh-1){
				heigh  = 0;
				// 重置pSrc
				pSrc   = pSave +4;
				pSave  = pSrc;
			}else{
				pSrc += offset0 ;
			}
			
		}
		
	}else{
		
		offset0 = ((array->width)*3) - 3;
		length = ((pArrayNode)pNode)->len*3;
		length = (len>length)?length:len;
		len = length/3;
		
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				while(len--){
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc);
					*pDest++ = *(pSrc+2);
					pSrc    += 3;
					if(heigh ++>= array->heigh-1){
						heigh  = 0;
						// 重置pSrc
						pSrc   = pSave +3;
						pSave  = pSrc;
					}else{
						pSrc += offset0;
					}
					
				}
			}break;
			case DATATYPE_GBR:{
				while(len--){
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc);
					pSrc    += 3;
					if(heigh ++>= array->heigh-1){
						heigh  = 0;
						// 重置pSrc
						pSrc   = pSave +3;
						pSave  = pSrc;
					}else{
						pSrc += offset0;
					}
					
				}
			}break;
			case DATATYPE_RBG:{
				while(len--){
					*pDest++ = *pSrc++;
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc);
					pSrc    += 2;
					if(heigh ++>= array->heigh-1){
						heigh  = 0;
						// 重置pSrc
						pSrc   = pSave +3;
						pSave  = pSrc;
					}else{
						pSrc += offset0;
					}
					
				}
			}break;
			case DATATYPE_RGB:{
				while(len--){
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					if(heigh ++>= array->heigh-1){
						heigh  = 0;
						// 重置pSrc
						pSrc   = pSave +3;
						pSave  = pSrc;
					}else{
						pSrc += offset0;
					}
					
				}
			}break;
			default:{}break;
		}
		
	}
	return length;
}

static void calculateArray_1(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum)
{
	/*
				1  2  3  4
				8	 7	6	 5
				9 10 11 12
	 */
	Int16 size = sum;
	*x = 0;
	*y = 0;
	while (1) {
		
		*x = size;
		if (size <= width) return;else *y = *y + 1;
		size -= width;

	}
	
}
static void calculateArray_2(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum)
{
	/*
				1  2  3  4
				5  6  7  8 
				9  10 11 12
	 */
	Int16 size = sum;
	*x = 0;
	*y = 0;
	while (1) {
		
		*x = size;
		if (size <= width) return;else *y = *y + 1;
		size -= width;

	}
}
static void calculateArray_3(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum)
{
	/*
				1  6  7  12
				2  5  8  11
				3  4  9  10
	 */
	Int16 size = sum;
	*x = 0;
	*y = 0;
	while (1) {

		*y = size;
		if (size < heigh) {
			if (*x & 0x01) *y = heigh - *y;
			return;
		}
		else { *x += 1; }
//		if (size == heigh&&*x & 0x01) { *x += 1;*y = 0;return; }
//		if (size <= heigh) return;else *x += 1;
		size -= heigh;

	}
}
static void calculateArray_4(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 sum)
{
	Int16 size = sum;
	*x = 0;
	*y = 0;
	while (1) {
		
		*y = size;
		if (size < heigh) return;else *x = *x + 1;
		size -= heigh;
	}
}


//void          addNodeList(pArtNode  node)
//{
//	int i;
//	if(node!= NULL){
//		for(i=0;i<ARTCONFIG_NUM;i++){
//			if(strcmp(artNetConfig->longName,(const char*)node->ShortName) ==0){
//				if()
//			}
//		}
//	}
//}

//typedef struct ArrayCtrl{
//	Int8			handle;
//	struct    ArrayCtrl*pNext;
//	Array			array;
//}ArrayCtrl,*pArrayCtrl;

//static pArrayCtrl phead =NULL;
//static pDmxList   pdmxHead=NULL;

//Int8  		createArray(Int8*Name,Int16 Row,Int16 Column)
//{
//	pArrayCtrl	pctrl;
//	if(Name != NULL){
//		pctrl = (pArrayCtrl)malloc(sizeof(ArrayCtrl)+Row*Column*Depth);
//		if(pctrl==NULL){
//#if DMXOUT_DEBUG
//		printf("pctrl create error File %s %d\r\n",__FILE__,__LINE__);
//#endif			
//			return 0;
//		}
//		strlcpy((char*)pctrl->array.Name,(const char*)Name,DMXOUT_NAMESIZE);
//		pctrl->array.point.width= Row;
//		pctrl->array.point.heigh = Column;
//		pctrl->array.array	 = (Int32***)((Int8*)pctrl +sizeof(ArrayCtrl));
//		if(phead==NULL){
//			pctrl->handle = 1;
//		}else{
//			pctrl->handle = phead->handle+1;
//		}
//		pctrl->pNext = phead;
//		phead = pctrl;
//		
//		return pctrl->handle;
//	}
//	return 0;
//}
//pArray		getArray   (Int8 handle)
//{
//	pArrayCtrl plink = phead;
//	while(plink!=NULL){
//		if(plink->handle == handle){
//			return &plink->array;
//		}
//	}
//	return NULL;
//}

//pArray	getArrayByName(Int8*name)
//{
//	pArrayCtrl plink = phead;
//	if(name!= NULL){
//		while(plink){
//			if(strcmp((const char*)plink->array.Name,(const char*)name) == 0){
//				return &plink->array;
//			}
//		}
//	}
//	return NULL;
//}

//pDmxOut   createDmxOutByHandle(Int8 Ip[4],Int8 SubUni,Int8 Net,
//											 Int8* Name,Int8 Row,Int8 Column,Int8 handle)
//{
//	pArray array = getArray(handle);
//	pDmxList	plist;
//	if(array ==NULL){
//		return NULL;
//	}
//	plist = (pDmxList)malloc(sizeof(DmxList));
//	if(plist!= NULL){
//		plist->object.Ip[0] = Ip[0];
//		plist->object.Ip[1] = Ip[1];
//		plist->object.Ip[2] = Ip[2];
//		plist->object.Ip[3] = Ip[3];
//		plist->object.Net  	= Net;
//		plist->object.SubUni= SubUni;
//		plist->object.Sequence	= 1;
//		plist->object.array.point.width = Row;
//		plist->object.array.point.heigh = Column;
//		plist->object.array.array		= (Int32***)array->array;
//		plist->pNext = pdmxHead;
//		pdmxHead = plist;
//		return &pdmxHead->object;
//	}
//	return NULL;
//}

//pDmxOut		createDmxOutByName(Int8 Ip[4],Int8 SubUni,Int8 Net,
//											 Int8* Name,Int8 Row,Int8 Column,Int8*arrayName)
//{
//	pArray array = getArrayByName(Name);
//	pDmxList	plist;
//	if(array ==NULL){
//		return NULL;
//	}
//	plist = (pDmxList)malloc(sizeof(DmxList));
//	if(plist!= NULL){
//		plist->object.Ip[0] = Ip[0];
//		plist->object.Ip[1] = Ip[1];
//		plist->object.Ip[2] = Ip[2];
//		plist->object.Ip[3] = Ip[3];
//		plist->object.Net  	= Net;
//		plist->object.SubUni= SubUni;
//		plist->object.Sequence	= 1;
//		plist->object.array.point.width = Row;
//		plist->object.array.point.heigh = Column;
//		plist->object.array.array		= (Int32***)array->array;
//		plist->pNext = pdmxHead;
//		pdmxHead = plist;
//		return &pdmxHead->object;
//	}
//	return NULL;
//}

//pDmxList  getDmxListHead(void)
//{
//	return pdmxHead;
//}

