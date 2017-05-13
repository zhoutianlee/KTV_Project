/*
 本文件用于内存管理，测试通过
 */ 
#ifndef __ARTMEM__H
#define __ARTMEM__H
#ifdef __cplusplus
extern "C"{
#endif

// data type 
typedef unsigned char  mu8;
typedef unsigned short mu16;
typedef unsigned long  mu32;	
typedef struct Block {
	mu32  size : 24;//:15;
	mu32  used : 8;//:1;
	mu8  data[];
}Block, *pBlock;

typedef struct ListBlock {
	struct ListBlock*pNext;
	struct ListBlock*pPrev;
	Block  block;
}ListBlock, *pListBlock;

typedef struct ListBlockHandle {
	pListBlock pfree;
	pListBlock pused;

	mu8*			  pstart;
	mu8*			  pend;
	mu32        freeSize;
	mu32        sizes;
}ListBlockHandle, *pListBlockHandle;
typedef pListBlockHandle pHandle;
typedef struct MemFuncStruct{
	pHandle (*initMem)			 (int size,void*block);
	void*   (*getMem)				 (pHandle p,int size);
	void*   (*getClearMem)	 (pHandle p,int size);
	void    (*putMem)				 (pHandle p,void*ptr);
	
	mu32    (*getFreeSize)	 (pHandle p);
	mu32    (*getMemSize)		 (pHandle p);
	mu8     (*getUsedPercent)(pHandle p);
	mu8     (*getFreePercent)(pHandle p);
	
}MemFunc;


extern const MemFunc memFunc;									// 实例化对象

#ifdef __cplusplus	
}
#endif

#endif
