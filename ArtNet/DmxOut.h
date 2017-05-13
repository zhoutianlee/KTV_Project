
#ifndef __DMXOUT__H
#define __DMXOUT__H

#include "ArtNetType.h"

typedef struct DMXList*pDMXList;

#define  DMXOUT_NAMESIZE	18
#define  DMXOUT_DEBUG			1

typedef enum   LIGHT_TYPE{
	LIGHT_TYPE_SOUNDWAVE = 0,
	LIGHT_TYPE_DIAMOND0  = 1,
	LIGHT_TYPE_DIAMOND1  = 2,
	LIGHT_TYPE_FLOOR		 = 3,
	LIGHT_TYPE_TOP			 = 4,
	LIGHT_TYPE_TV				 = 5,
}LIGHT_TYPE;

typedef union  UnionNode*pUnionNode;
typedef struct Array*    pArray;
typedef Int16  (*GetNodeData)(pArray array,pUnionNode pNode,Int16 len,Int8*pDest);
typedef void   (*CalculateArray)(Int16 width,Int16 heigh,Int16* x,Int16* y,Int16 sum);
typedef struct NodeMsg{
	
	const char*NodeName;
	Int8 			IPAddress[4];															// ip��ַ
	Int8			Net;
	Int8			SubUni;
	Int8      Sequence;
  Int8      isExit;		  															// ���ڱ���Ƿ��Ѿ����Ǽǹ�
	
}NodeMsg;

typedef struct LineNode{
	struct LineNode*pNext;
	NodeMsg   	msg;
	
	
	GetNodeData func;																		// ������ʱ�����ָ�����ĸ�����
	Int16  	  	len;
	DATATYPE  	type;
	
	Int16				spare0;
	Int16				spare1;
	void*				spare3;
	
	DataType* 	Data;
}LineNode,*pLineNode;

typedef struct ArrayNode{
	struct ArrayNode*pNext;
	
	NodeMsg  	 	msg;
	
	
	GetNodeData func;																		// ������ʱ�����ָ�����ĸ�����
	Int16       len;
	DATATYPE		type;																		// �����ʱ��ͬ����RGB��RGBW����
	
	Int16     	width;
	Int16 			heigh;
	
	
//	DataType* 	Src;																		// ��������ָ��ͬһ����ʼ��ַ
	DataType*	  Data;																		// Src + offset; offset�Ǽ��������
	
}ArrayNode,*pArrayNode;

typedef union  UnionNode{														  // ���ڴ����������
	
	LineNode  	lineNode;
	ArrayNode 	arrayNode;
}UnionNode,*pUnionNode;

// ÿһ��IP��Ҫר�Ŵ���һ��
typedef struct NodeList{
	
	struct 		NodeList*pNext;
	Int32				nodeID;
	Int32				enable;									// 
	ARTNETMODE  mode;
	UnionNode*	pHead;
	UnionNode*	pTail;
	
	pArray		  parray;
}NodeList,*pNodeList;

typedef struct ArtNetLineConfig{
	Int8				nodeID;								// �Ƿ�Ϊ�µ�һ���ڵ�,ID��ͬ����Ϊͬһ�����͵Ľڵ�,���ںϲ���ΪArray��������
	char* 			IPAddress;						// 
	
	Int8  			subUni;
	Int16   		length;
	DATATYPE  	type;									// DATATYPE_RGB OR OTHRER
	GetNodeData getNodeData; 
}ArtNetLineConfig,*pArtNetLineConfig;

typedef struct ArtNetArrayConfig{
	Int8				nodeID;								// �Ƿ�Ϊ�µ�һ���ڵ�,ID��ͬ����Ϊͬһ�����͵Ľڵ�,���ںϲ���ΪArray��������
	char* 			IPAddress;						// 
	
	Int8  			subUni;
	Int16   		length;
	DATATYPE  	type;									// DATATYPE_RGB OR OTHRER
	
	Int16   		width;
	Int16   		heigh;
	
	GetNodeData getNodeData; 
	CalculateArray calculate;
	ARRAYMODE   mode;
}ArtNetArrayConfig,*pArtNetArrayConfig;

//typedef struct ArtNetConfig{
//	Int8				nodeID;								// �Ƿ�Ϊ�µ�һ���ڵ�,ID��ͬ����Ϊͬһ�����͵Ľڵ�,���ںϲ���ΪArray��������
//	char* 			IPAddress;						// 
//	char*       shortName;						// �̵�ַ
//	
//	ARTNETMODE 	mode;									// Belong to ARTNETMODE_ARRAY OR ARTNETMODE_LINE
//	Int8  			subUni;
//	Int16   		length;
//	DATATYPE  	type;									// DATATYPE_RGB OR OTHRER
//	
//	Int16   		width;
//	Int16   		heigh;
//	
//	ARRAYMODE 	arrayMode;						
//	GetNodeData getNodeData; 
//	
//}ArtNetConfig,*pArtNetConfig;

//typedef struct ArtNetConfig{				// �����б�
//	
//	char*  			longName;
//	ARTNETMODE 	mode;									// Belong to ARTNETMODE_ARRAY OR ARTNETMODE_LINE
//	Int8  			subUni;
//	Int16   		length;
//	DATATYPE  	type;									// DATATYPE_RGB OR OTHRER
//	
//	Int16   		width;
//	Int16   		heigh;
//	
//	ARRAYMODE 	arrayMode;						// 
//	Int16       xStart;
//	Int16       yStart;
//	GetNodeData  getNodeData;
//}ArtNetConfig,*pArtNetConfig;

typedef struct Array{								// �������ڼ��������ռ�

	Int16				width;
	Int16				heigh;
	DATATYPE		type;
	
	DataType*   array;								// һά����Ҳ���Է��ʶ�ά�����
}Array,*pArray;

typedef struct PixelLine{
	struct PixelLine*pNext;
	pLineNode 			 pline;
}PixelLine,*pPixelLine;

typedef struct PixelHead{
	Int32       nodeID;
	Int16 			width;
	Int16				heigh;
	Int16				dlt;
	pPixelLine	pHead;
	pPixelLine	pTail;
}PixelHead,*pPixelHead;

void   				initArtNetConfig(void);
void          registerNodeList(pArtNode  node);												// �ڷ����豸��ʱ����Ҫ���еǼ�
void          setLightTypeEnable(LIGHT_TYPE type,char status);							// 1��ʾʹ�ܣ�0��ʾ��ֹ

pNodeList     getNodeListHead(void);
pNodeList			getNodeListIndex(Int32 nodeID);

pArray				createArray(Int16 width,Int16 height,DATATYPE datatype);// �������ڲ���
pArray  			getArrayIndex(Int8 index);

pPixelHead		createPixelHead(Int16	width,Int32 nodeID);
char				  addPixelMember(pPixelHead phead,pLineNode pline);
void 					getPixelLine(pPixelHead phead,Int16 heigh,pRGB pdata);
void  				setPixelLine(pPixelHead phead,Int16 heigh,pRGB pdata);
pPixelHead		getFloorHead(void);
pPixelHead		getDiamondHead(void);

#endif
