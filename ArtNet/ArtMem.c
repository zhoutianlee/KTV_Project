
#include "ArtMem.h"

#define  NULL 					(void*)0
#define  MEMHEAD_SIZE		12
#define  USED						1
#define  UNUSED					0




static pHandle initMem(int size,void*block);
static void*   getMem (pHandle p,int size);
static void*   getClearMem (pHandle p,int size);
static void    putMem (pHandle p,void*ptr);
static mu32    getFreeSize(pHandle p);
static mu32    getMemSize (pHandle p);
static mu8     getFreePercent(pHandle p);
static mu8     getUsedPercent(pHandle p);


const MemFunc memFunc = {
	initMem,
	getMem,
	getClearMem,
	putMem,
	
	getFreeSize,
	getMemSize,
	getFreePercent,
	getUsedPercent
};

static pHandle initMem(int size,void*block)
{
	pListBlockHandle handle = NULL;

	if(size > sizeof(ListBlockHandle)+MEMHEAD_SIZE){
		handle = (pListBlockHandle)block;
		handle->pfree 	 = (pListBlock)((mu8*)block+sizeof(ListBlockHandle));
		handle->pused 	 = NULL;
		handle->freeSize = size - sizeof(ListBlockHandle)-MEMHEAD_SIZE;
		handle->sizes    = size;
		handle->pstart   = block;
		handle->pend		 = ((mu8*)block)+size;
		// init listBlock
		handle->pfree->pNext 			 = NULL;
		handle->pfree->pPrev			 = NULL;
		handle->pfree->block.size  = handle->freeSize;
		handle->pfree->block.used  = UNUSED;
	}
	return (pHandle)handle;
}
static void*   getMem (pHandle p,int size)
{
	pListBlock pLink,ptemp;
	
	if(((pListBlockHandle)p)!= NULL){
		pLink = ((pListBlockHandle)p)->pfree;
		
		while(pLink!=NULL){
			if(pLink->block.size >= size){
				// 进行内存分配
				pLink->block.used = USED;
				if(pLink->block.size > (MEMHEAD_SIZE+size)){
					// 进行拆分
					ptemp =(pListBlock)(((mu8*)pLink)+MEMHEAD_SIZE+size);
					ptemp ->block.used = UNUSED;
					ptemp->block.size  = pLink->block.size - size-MEMHEAD_SIZE;
					// 处理指针指向
					ptemp->pPrev = pLink->pPrev;
					if(pLink->pPrev == NULL){
						((pListBlockHandle)p)->pfree = ptemp;
					}else{
						pLink->pPrev->pNext = ptemp;
					}
					
					ptemp->pNext = pLink->pNext;
					if(ptemp->pNext != NULL){
						ptemp->pNext->pPrev = ptemp;
					}
					// 处理系统内存大小
					((pListBlockHandle)p)->freeSize -= (size +MEMHEAD_SIZE);
					// 处理被分配的内存块
					pLink->block.size = size;
					
					return pLink->block.data;
				}else{
					
					if (pLink->pPrev == NULL) {
						((pListBlockHandle)p)->pfree = ((pListBlockHandle)p)->pfree->pNext;
						if (((pListBlockHandle)p)->pfree != NULL) {
							((pListBlockHandle)p)->pfree->pPrev = NULL;
						}
					}else{
						pLink->pPrev->pNext = pLink->pNext;
					}
					if(pLink->pNext != NULL){
						pLink->pNext->pPrev = pLink->pPrev;
					}
					((pListBlockHandle)p)->freeSize -= pLink->block.size;
					return pLink->block.data;
				}
			}
			pLink = pLink->pNext;
		}
	}
	return NULL;
}

static void*   getClearMem (pHandle p,int size)
{
	mu8 *ptr = (mu8*)getMem(p,size);
	int  i;
	if(ptr!= NULL){
		for(i=0;i<size;i++){
			ptr[i]=0;
		}
	}
	return ptr;
}
static void    putMem (pHandle p,void*ptr)
{
	pListBlock ptemp,plink;
	if(ptr!= NULL&&p!= NULL){
		ptemp = (pListBlock)((mu8*)ptr - MEMHEAD_SIZE);
		if((mu32)ptemp < (mu32)((pListBlockHandle)p)->pstart)return ;// error code
		if (ptemp->block.used == UNUSED)return;
		ptemp->block.used = UNUSED;					// 进行内存释放
		if(((mu32)((mu8*)ptr)+ptemp->block.size) <(mu32)(((pListBlockHandle)p)->pend)){
			plink = ((pListBlock)((mu8*)ptr+ptemp->block.size));
			if(plink->block.used == UNUSED){
				// 准备合并
				
				ptemp->pNext			 = plink->pNext;
				ptemp->pPrev		   = plink->pPrev;
				if(plink->pPrev!= NULL){
					plink->pPrev->pNext = ptemp;
				}else{
					((pListBlockHandle)p)->pfree = ptemp;
				}
				if(plink->pNext!= NULL){
					plink->pNext->pPrev = ptemp;
				}
				((pListBlockHandle)p)->freeSize += ptemp->block.size + MEMHEAD_SIZE;
				ptemp->block.size += plink->block.size+MEMHEAD_SIZE;
			}else{
				
				ptemp->pNext = ((pListBlockHandle)p)->pfree;
				ptemp->pPrev = NULL;
				if(((pListBlockHandle)p)->pfree!= NULL)((pListBlockHandle)p)->pfree->pPrev = ptemp;
				((pListBlockHandle)p)->pfree 			  = ptemp;
				((pListBlockHandle)p)->freeSize		 += ptemp->block.size;
			}
		}else{
			if(((pListBlockHandle)p)->pfree!= NULL)((pListBlockHandle)p)->pfree->pPrev = ptemp;
			((pListBlockHandle)p)->pfree 			  = ptemp;
			((pListBlockHandle)p)->freeSize		 += ptemp->block.size;
		}
	}
}

static mu32    getFreeSize(pHandle p)
{
	if(p!=NULL){
		return ((pListBlockHandle)p)->freeSize;
	}
	return 0;
}
static mu32    getMemSize (pHandle p)
{
	if(p!=NULL){
		return ((pListBlockHandle)p)->sizes;
	}
	return 0;
}

static	mu8     getFreePercent(pHandle p)
{
	if(p!=NULL){
		return (mu8)((mu32)((pListBlockHandle)p)->freeSize*100)/((pListBlockHandle)p)->sizes;
	}
	return 0;
}
static	mu8     getUsedPercent(pHandle p)
{
	return 100 - getFreePercent(p);
}
