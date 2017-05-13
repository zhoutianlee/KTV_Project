

#include "DmxOut.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define ARTCONFIG_NUM					sizeof(artNetConfig)/sizeof(ArtNetConfig)
#define CompareIP(ip1,ip2)		(ip1[0]==ip2[0]&&ip1[1]==ip2[1]&&ip1[2]==ip2[2]&&ip1[3]==ip2[3])	

static void  ChangeIPString(const char*ip,Int8* array);

static Int16 GetNodeData_0(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_1(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_2(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_3(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static Int16 GetNodeData_4(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
static void  calculateArray_1(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count);
static void  calculateArray_2(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count);
static void  calculateArray_3(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count);
static void  calculateArray_4(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count);

static const  ArtNetConfig artNetConfig[] = {
	// 天花板
	{1,"192.168.1.103","TopLine1",			ARTNETMODE_LINE,0,70,		DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{1,"192.168.1.103","TopLine1",			ARTNETMODE_LINE,1,70,		DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{1,"192.168.1.103","TopLine1",			ARTNETMODE_LINE,2,160,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},//DATATYPE_RGB
	{1,"192.168.1.103","TopLine1",			ARTNETMODE_LINE,3,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},//DATATYPE_RGB
	
	{1,"192.168.1.101","TopLine2",			ARTNETMODE_LINE,0,120,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{1,"192.168.1.101","TopLine2",			ARTNETMODE_LINE,1,120,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{1,"192.168.1.101","TopLine2",			ARTNETMODE_LINE,2,100,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{1,"192.168.1.101","TopLine2",			ARTNETMODE_LINE,3,100,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	
	{2,"192.168.1.102","TVLine1",				ARTNETMODE_LINE,0,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},//DATATYPE_RGB
	{2,"192.168.1.102","TVLine1",				ARTNETMODE_LINE,1,142,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},//DATATYPE_RGB
	
	// 地板灯
	{3,"192.168.1.94","FloorLine1",			ARTNETMODE_LINE,0,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{3,"192.168.1.94","FloorLine1",			ARTNETMODE_LINE,1,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{3,"192.168.1.94","FloorLine1",			ARTNETMODE_LINE,2,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{3,"192.168.1.94","FloorLine1",    	ARTNETMODE_LINE,3,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{3,"192.168.1.95","FloorLine1",     ARTNETMODE_LINE,4,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{3,"192.168.1.95","FloorLine1",     ARTNETMODE_LINE,5,170,	DATATYPE_RBG,0 ,0 ,ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	
	// 钻石墙
	{4,"192.168.1.100","DiamondLine1",  ARTNETMODE_LINE,0,170,	DATATYPE_RBG,0 ,0, ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{4,"192.168.1.100","DiamondLine1",  ARTNETMODE_LINE,1,170,	DATATYPE_RBG,0 ,0, ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{4,"192.168.1.100","DiamondLine1",  ARTNETMODE_LINE,2,170,	DATATYPE_RBG,0 ,0, ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	{4,"192.168.1.100","DiamondLine1",  ARTNETMODE_LINE,3,170,	DATATYPE_RBG,0 ,0, ARRAYMODE_1,0 ,0 ,GetNodeData_0},
	
	{5,"192.168.1.110","DiamondArray1", ARTNETMODE_ARRAY,0 ,36, DATATYPE_RGBW,6 ,6,ARRAYMODE_1,0 ,0 ,GetNodeData_4},
	{6,"192.168.1.111","DiamondArray1", ARTNETMODE_ARRAY,0 ,36, DATATYPE_RGBW,6 ,6,ARRAYMODE_1,0 ,0 ,GetNodeData_4},
	{7,"192.168.1.112","DiamondArray1", ARTNETMODE_ARRAY,0 ,36, DATATYPE_RGBW,6 ,6,ARRAYMODE_1,0 ,0 ,GetNodeData_4},
	{8,"192.168.1.113","DiamondArray1", ARTNETMODE_ARRAY,0 ,36, DATATYPE_RGBW,6 ,6,ARRAYMODE_1,0 ,0 ,GetNodeData_4},

//	{9,"192.168.1.92","SoundWaveArray1",ARTNETMODE_ARRAY,0 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,0 ,0 ,GetNodeData_3},
//	{9,"192.168.1.92","SoundWaveArray1",ARTNETMODE_ARRAY,1 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,2 ,50,GetNodeData_3},
//	{9,"192.168.1.92","SoundWaveArray1",ARTNETMODE_ARRAY,2 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,5 ,20,GetNodeData_3},
//	{9,"192.168.1.92","SoundWaveArray1",ARTNETMODE_ARRAY,3 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,8 ,30,GetNodeData_3},
//	{9,"192.168.1.93","SoundWaveArray1",ARTNETMODE_ARRAY,4 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,11,40,GetNodeData_3},
//	{9,"192.168.1.93","SoundWaveArray1",ARTNETMODE_ARRAY,5 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,14,10,GetNodeData_3},
//	{9,"192.168.1.93","SoundWaveArray1",ARTNETMODE_ARRAY,6 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,17,60,GetNodeData_3},
//	{9,"192.168.1.93","SoundWaveArray1",ARTNETMODE_ARRAY,7 ,130,DATATYPE_GRB,80,60,ARRAYMODE_3,19,10,GetNodeData_3},
//	
//	{9,"192.168.1.96","SoundWaveArray1",ARTNETMODE_ARRAY,0 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,20,0 ,GetNodeData_3},
//	{9,"192.168.1.96","SoundWaveArray1",ARTNETMODE_ARRAY,1 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,22,50,GetNodeData_3},
//	{9,"192.168.1.96","SoundWaveArray1",ARTNETMODE_ARRAY,2 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,25,20,GetNodeData_3},
//	{9,"192.168.1.96","SoundWaveArray1",ARTNETMODE_ARRAY,3 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,28,30,GetNodeData_3},
//	{9,"192.168.1.97","SoundWaveArray1",ARTNETMODE_ARRAY,4 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,31,40,GetNodeData_3},
//	{9,"192.168.1.97","SoundWaveArray1",ARTNETMODE_ARRAY,5 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,34,10,GetNodeData_3},
//	{9,"192.168.1.97","SoundWaveArray1",ARTNETMODE_ARRAY,6 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,37,60,GetNodeData_3},
//	{9,"192.168.1.97","SoundWaveArray1",ARTNETMODE_ARRAY,7 ,130,DATATYPE_GRB,80,60,ARRAYMODE_3,39,10,GetNodeData_3},
//	
//	{9,"192.168.1.98","SoundWaveArray1",ARTNETMODE_ARRAY,0 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,40,0 ,GetNodeData_3},
//	{9,"192.168.1.98","SoundWaveArray1",ARTNETMODE_ARRAY,1 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,42,50,GetNodeData_3},
//	{9,"192.168.1.98","SoundWaveArray1",ARTNETMODE_ARRAY,2 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,45,20,GetNodeData_3},
//	{9,"192.168.1.98","SoundWaveArray1",ARTNETMODE_ARRAY,3 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,48,30,GetNodeData_3},
//	{9,"192.168.1.99","SoundWaveArray1",ARTNETMODE_ARRAY,4 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,51,40,GetNodeData_3},
//	{9,"192.168.1.99","SoundWaveArray1",ARTNETMODE_ARRAY,5 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,54,10,GetNodeData_3},
//	{9,"192.168.1.99","SoundWaveArray1",ARTNETMODE_ARRAY,6 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,57,60,GetNodeData_3},
//	{9,"192.168.1.99","SoundWaveArray1",ARTNETMODE_ARRAY,7 ,130,DATATYPE_GRB,80,60,ARRAYMODE_3,59,10,GetNodeData_3},
//	
//	{9,"192.168.1.90","SoundWaveArray1",ARTNETMODE_ARRAY,0 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,60,0 ,GetNodeData_3},
//	{9,"192.168.1.90","SoundWaveArray1",ARTNETMODE_ARRAY,1 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,62,50,GetNodeData_3},
//	{9,"192.168.1.90","SoundWaveArray1",ARTNETMODE_ARRAY,2 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,65,20,GetNodeData_3},
//	{9,"192.168.1.90","SoundWaveArray1",ARTNETMODE_ARRAY,3 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,68,30,GetNodeData_3},
//	{9,"192.168.1.91","SoundWaveArray1",ARTNETMODE_ARRAY,4 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,71,40,GetNodeData_3},
//	{9,"192.168.1.91","SoundWaveArray1",ARTNETMODE_ARRAY,5 ,170,DATATYPE_GRB,80,60,ARRAYMODE_3,74,10,GetNodeData_3},


};


static pArray phead;
static pNodeList plistHead  = NULL;
static pArray array[10];

void initArtNetConfig(void)
{
	Int8      nodeID = 0;
	pNodeList pnode= NULL,plist = plistHead;
	pArtNetConfig pconfig = (pArtNetConfig)&artNetConfig[0];
	int       i=0;
	int       countArray = 0;
	DataType*pSave;

	pUnionNode    punion;
	
	while(i++< ARTCONFIG_NUM){
		// 都要创建punion对象
		punion = memFunc.getMem(artMemHandle,sizeof(UnionNode));if(punion == NULL){printf("punion create error\r\n");return;}
		punion->lineNode.msg.SubUni 		  = pconfig->subUni;
		punion->lineNode.msg.Sequence 	  = 1;
		punion->lineNode.msg.NodeName   	= pconfig->shortName;
		punion->lineNode.msg.isExit     	= 0;
		
		ChangeIPString(pconfig->IPAddress,punion->lineNode.msg.IPAddress);					// 转换IP地址

		punion->lineNode.pNext					  = NULL;
		punion ->lineNode.len 						= pconfig->length;
		punion ->lineNode.type 						= pconfig->type;
		punion->lineNode.func							= pconfig->getNodeData;
		// 判断是否需要创建PNodeList对象只需要看是否pconfig ->nodeID==NodeID;
		if(pconfig->mode == ARTNETMODE_LINE){
			
			if(pconfig->type != DATATYPE_RGBW)	punion->lineNode.Data	= (DataType*)memFunc.getMem(artMemHandle,sizeof(RGBW)*pconfig->length); 
			else punion->lineNode.Data	= (DataType*)memFunc.getMem(artMemHandle,sizeof(RGB)*pconfig->length); 
			
			if(punion->lineNode.Data == NULL){printf("punion pData create error\r\n");return;}
		}else{
			punion->arrayNode.heigh					= pconfig->yStart;
			punion->arrayNode.width					= pconfig->xStart;
			
			// data部分没有初始化
		}
		if(pconfig ->nodeID != nodeID){
			
			nodeID = pconfig ->nodeID;
			// 需要创建NodeList对象
			pnode = memFunc.getMem(artMemHandle,sizeof(NodeList));if(pnode == NULL){printf("pnode create error\r\n");return;}
			// 在第一次创建的时候，如果是Array对象，这需要创建内存
			if(pconfig->mode == ARTNETMODE_ARRAY){
				if(pconfig->type == DATATYPE_RGBW)
				{pSave = (DataType*)memFunc.getMem(artMemHandle,sizeof(RGBW)*(pconfig->width+1)*(pconfig->heigh+1));}
				else
				{pSave = (DataType*)memFunc.getMem(artMemHandle,sizeof(RGB)*(pconfig->width+1)*(pconfig->heigh+1));}
				
				punion->arrayNode.Data = pSave;
				if(punion->arrayNode.Data  == NULL){printf("pSave create error\r\n");return;}

				pnode->parray = (pArray)memFunc.getMem(artMemHandle,sizeof(Array));
				if(pnode->parray == NULL){printf("parray create error\r\n");return;}
				
				pnode->parray->array = pSave;
				pnode->parray->heigh = pconfig->heigh;
				pnode->parray->width = pconfig->width;
				pnode->parray->type  = pconfig->type;
				array[countArray++]  = pnode->parray;
			}else{
				// 如果是Line，每个都需要在创建punion对象的时候，创建pData内存堆
				// do nothing
			}
			// 初始化内存节点
			pnode->pHead = punion;
			pnode->pTail = punion;
			pnode->pNext = NULL;
			pnode->mode	 = pconfig->mode;
			if(plist != NULL) plist->pNext = pnode;
			else plistHead = pnode;
			
			plist = pnode;
		}else{
			// 因为链表都是在头部
			pnode->pTail->lineNode.pNext = (pLineNode) punion;
			pnode->pTail = punion;
			// 计算Array 模式 的偏移地址
			if(pnode->mode == ARTNETMODE_ARRAY){
				if(punion->arrayNode.type == DATATYPE_RGBW)
					punion->arrayNode.Data = (DataType*)((RGBW*)pnode->parray->array+(pnode->parray->width)*punion->arrayNode.heigh + punion->arrayNode.width);
				else
					punion->arrayNode.Data = (DataType*)((RGB*)pnode->parray->array +(pnode->parray->width)*punion->arrayNode.heigh + punion->arrayNode.width);
			}
		}
		pconfig++;
	}
	
}
pArray  			getArrayIndex(Int8 index)
{
	return array[index];
}
void          registerNodeList(pArtNode  node)
{
	pNodeList plist = plistHead;
	char      flag = 0;
	pUnionNode    punion;
	if(node != NULL ){
		while(plist){
			
			punion = plist->pHead;
			while(punion){
				if(CompareIP(punion->lineNode.msg.IPAddress,node->IPAddress)){
					punion->lineNode.msg.isExit = 1;
					flag = 1;
				}
				punion = (pUnionNode) punion ->lineNode.pNext;
			}

			plist = plist->pNext;
		}
	}
//	if(flag == 1)printf("device %s register success\r\n",node->ShortName);
//	else printf("device %s register fault\r\n",node->ShortName);
}

pArtNetConfig getArtNetConfig(int*size)
{
	*size  = sizeof(artNetConfig)/sizeof(ArtNetConfig);
	return (pArtNetConfig)&artNetConfig;
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

pNodeList     getNodeListHead(void)
{
	return plistHead;
}

pArray				getArrayHead(void)
{
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
	char flag   = (((pArrayNode)pNode)->width)&0x01;
	Int16 width = ((pArrayNode)pNode)->width;
	Int8*pSrc   = (Int8*)((pArrayNode)pNode)->Data;
	Int16 length ;
	Int16 offset =0;
	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len    = length>>2;
		offset = ((array->width)<<2);
		while(len--){
			if(flag ==0){
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				if(width++>= array->width-1){
					flag = 1;
					pSrc+= offset;
				}
			}else{
				pSrc    -= 4;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				if(width--<=0){
					flag = 0;
					pSrc+= offset;
				}
			}
		}
	}else{
		length = ((pArrayNode)pNode)->len*3;
		length = (len>length)?length:len;
		len    = length/3;
		offset = (array->width);
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				while(len--){
					if(flag ==0){
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						pSrc += 3;
						if(width++>= array->width-1){
							flag = 1;
							pSrc+= offset;
						}
					}else{
						pSrc 		-= 3;
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc);
						*pDest++ = *(pSrc+2);
						pSrc += 3;
						if(width--<=0){
							flag = 0;
							pSrc+= offset;
						}
					}
				}
			}break;
			case DATATYPE_GBR:{
				while(len--){
					if(flag ==0){
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc);
						pSrc += 3;
						if(width++>= array->width-1){
							flag = 1;
							pSrc+= offset;
						}
					}else{
						pSrc 		-= 3;
						*pDest++ = *(pSrc+1);
						*pDest++ = *(pSrc+2);
						*pDest++ = *(pSrc);
						pSrc += 3;
						if(width--<=0){
							flag = 0;
							pSrc+= offset;
						}
					}
				}
			}break;
			case DATATYPE_RBG:{
				while(len--){
					if(flag ==0){
						*pDest++ = *pSrc++;
						*pDest++ = *(pSrc+1);
						*pDest++ = *pSrc++;
						pSrc ++;
						if(width++>= array->width-1){
							flag = 1;
							pSrc+= offset;
						}
					}else{
						pSrc 		-= 3;
						*pDest++ = *pSrc++;
						*pDest++ = *(pSrc+1);
						*pDest++ = *pSrc++;
						pSrc ++;
						if(width--<=0){
							flag = 0;
							pSrc+= offset;
						}
					}
				}
			}break;
			case DATATYPE_RGB:{
				while(len--){
					if(flag ==0){
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						if(width++>= array->width-1){
							flag = 1;
							pSrc+= offset;
						}
					}else{
						pSrc 		-= 3;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						if(width--<=0){
							flag = 0;
							pSrc+= offset;
						}
					}
				}
			}break;
				default:{}break;
		}
		while(len--){
			if(flag ==0){
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				if(width++>= array->width-1){
					flag = 1;
					pSrc+= offset;
				}
			}else{
				pSrc 		-= 3;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				*pDest++ = *pSrc++;
				if(width--<=0){
					flag = 0;
					pSrc+= offset;
				}
			}
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

static Int16 GetNodeData_3(pArray array,pUnionNode pNode,Int16 len,Int8*pDest)
{
	/*
				1  6  7  12
				2  5  8  11
				3  4  9  10
	 */
	char flag   = (((pArrayNode)pNode)->width)&0x01;
	Int16 heigh = ((pArrayNode)pNode)->heigh;
	Int8*pSrc   = (Int8*)((pArrayNode)pNode)->Data;//(Int8*)array->array;
	Int16 length ;
	Int16 offset0,offset1;
	if(((pArrayNode)pNode)->type == DATATYPE_RGBW){
		length = ((pArrayNode)pNode)->len<<2;
		length = (len>length)?length:len;
		len    = length>>2;
		offset0 = ((array->width)<<2);
		while(len--){
			
			*pDest++ = *pSrc;
			*pDest++ = *(pSrc+1);
			*pDest++ = *(pSrc+2);
			*pDest++ = *(pSrc+3);
			pSrc +=4;
			if(flag ==0){
				
				if(++heigh>=array->heigh){
					flag     = 1;
					pSrc    += 4;
				}else{
					pSrc    += offset0;
				}
			}else{
				
				if(--heigh==0){
					flag     = 0;
					pSrc    += 4;
				}else{
					pSrc    -= offset0;
				}
			}
		}
		
	}else{
		offset0 = ((array->width)*3);
		length  = ((pArrayNode)pNode)->len*3;
		length  = (len>length)?length:len;
		
		len     = length/3;
		switch(((pArrayNode)pNode)->type)
		{
			case DATATYPE_GRB:{
				while(len--){
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc);
					*pDest++ = *(pSrc+2);
					if(flag ==0){
				
							if(++heigh>=array->heigh){
								flag = 1;
								pSrc+= 3;
							}else{
								pSrc+= offset0;
							}
						}else{
							
							if(--heigh==0){
								flag = 0;
								pSrc+= 3;
							}else{
								pSrc-= offset0;
							}
						}
					}
			}break;
			case DATATYPE_GBR:{
				while(len--){
					
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc);
					if(flag ==0){
				
							if(++heigh>=array->heigh){
								flag = 1;
								pSrc+= 3;
							}else{
								pSrc+= offset0;
							}
						}else{
							
							if(--heigh==0){
								flag = 0;
								pSrc+= 3;
							}else{
								pSrc-= offset0;
							}
						}
					}
						
				
			}break;
			case DATATYPE_RBG:{
				while(len--){
					*pDest++ = *pSrc;
					*pDest++ = *(pSrc+2);
					*pDest++ = *(pSrc+1);
					if(flag ==0){
				
							if(++heigh>=array->heigh){
								flag = 1;
								pSrc+= 3;
							}else{
								pSrc+= offset0;
							}
						}else{
							
							if(--heigh==0){
								flag = 0;
								pSrc+= 3;
							}else{
								pSrc-= offset0;
							}
						}
//					if(flag ==0){
//							*pDest++ = *pSrc;
//							*pDest++ = *(pSrc+2);
//							*pDest++ = *(pSrc+1);
//							
//							if(heigh++>=array->heigh-1){
//								flag = 1;
//							}else{
//								pSrc    += offset0;
//							}
//						}else{
//							*pDest++ = *pSrc;
//							*pDest++ = *(pSrc+2);
//							*pDest++ = *(pSrc+1);
//							
//							if(heigh--<=0){
//								flag     = 0;
//							}else{
//								pSrc    -= offset1;
//							}
//						}
					}
			}break;
			case DATATYPE_RGB:{
				while(len--){
					*pDest++ = *pSrc;
					*pDest++ = *(pSrc+1);
					*pDest++ = *(pSrc+2);
					
					if(flag ==0){
				
							if(++heigh>=array->heigh){
								flag = 1;
								pSrc+= 3;
							}else{
								pSrc+= offset0;
							}
						}else{
							
							if(--heigh==0){
								flag = 0;
								pSrc+= 3;
							}else{
								pSrc-= offset0;
							}
						}
//					if(flag ==0){
//							*pDest++ = *pSrc;
//							*pDest++ = *(pSrc+1);
//							*pDest++ = *(pSrc+2);
//							if(heigh++>=array->heigh-1){
//								flag = 1;
//							}else{
//								pSrc    += offset0;
//							}
//						}else{
//							*pDest++ = *pSrc;
//							*pDest++ = *(pSrc+1);
//							*pDest++ = *(pSrc+2);
//							if(heigh--<=0){
//								flag     = 0;
//							}else{
//								pSrc    -= offset1;
//							}
//						}
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

static void calculateArray_1(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count)
{
	/*
				1  2  3  4
				8	 7	6	 5
				9 10 11 12
	 */
	Int16 size = count *len;
	Int16 i;
	*x = 0;
	*y = 0;
	while (1) {
		
		*x = size;
		if (size <= width) return;else *y = *y + 1;
		size -= width;

	}
	
}
static void calculateArray_2(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count)
{
	/*
				1  2  3  4
				5  6  7  8 
				9  10 11 12
	 */
	Int16 size = count *len;
	Int16 i;
	*x = 0;
	*y = 0;
	while (1) {
		
		*x = size;
		if (size <= width) return;else *y = *y + 1;
		size -= width;

	}
}
static void calculateArray_3(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count)
{
	Int16 size = count *len;
	Int16 i;
	*x = 0;
	*y = 0;
	while (1) {
		
		*y = size;
		if (size <= heigh) return;else *y = *y + 1;
		size -= heigh;

	}
}
static void calculateArray_4(Int16 width,Int16 heigh,Int16*x,Int16*y,Int16 len,Int16 count)
{
	Int16 size = count *len;
	Int16 i;
	*x = 0;
	*y = 0;
	while (1) {
		
		*y = size;
		if (size <= heigh) return;else *y = *y + 1;
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

