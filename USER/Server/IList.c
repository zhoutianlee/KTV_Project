
#include "IList.h"
#include "stdlib.h"

static	pListHandle		createHandle(uint16_t objectSize,pIList listCB);
static	pList 				createObject(pListHandle	handle);
static	char					appendObject(pListHandle handle,pList object);
static	char 					insertObject(pListHandle handle,pList object);
static	char 					deleteObject(pListHandle handle,pList object);
static	char 					removeObject(pListHandle handle,pList object);
static	char					removeNumber(pListHandle handle,uint16_t number);
static	uint16_t			getNumber	 (pListHandle handle,pList object);
static	uint16_t			getSize		 (pListHandle handle);
static	pList		 			getObject	 (pListHandle handle,uint16_t number);
static	pList				  getFirst		 (pListHandle handle);
static	pList				  getLast		 (pListHandle handle);

const IListFunc listFunc= {
	createHandle,
	createObject,
	appendObject,
	insertObject,
	deleteObject,
	removeObject,
	removeNumber,
	getNumber,
	getSize,
	getObject,
	getFirst,
	getLast,
};

static	pListHandle		createHandle(uint16_t objectSize,pIList listCB)
{
	pListHandle handle =NULL;
	if(listCB!=NULL&&listCB->malloc!=NULL){
		handle = (pListHandle)malloc(sizeof(ListHandle));
		if(handle != NULL){
			handle->count = 0;
			handle->current = NULL;
			handle->head 		 = NULL;
			handle->tail		= NULL;
			handle->listCB  = listCB;
			handle->objectSize	= objectSize;
		}
	}
	return handle;
}

static	pList 				createObject(pListHandle	handle)
{
	if(handle != NULL && handle->listCB->malloc!= NULL){
		return handle->listCB->malloc(handle->objectSize);
	}
	return NULL;
}

static	char					appendObject(pListHandle handle,pList object)
{
	if(handle !=NULL&& object!=NULL){
		object->pNext 			= NULL;
		if(handle->tail!= NULL){
			handle->tail->pNext = object;
			handle->tail				= object;
		}else{
			handle->head = object;
			handle->tail = object;
		}
		handle->count		++;
		return 1;
	}
	return 0;
}

static	char 					insertObject(pListHandle handle,pList object)
{
	if(handle !=NULL&& object!=NULL){
		
		object->pNext  = handle->head;
		if(handle->tail == NULL){
			handle->tail	= object;
		}
		handle->count		++;
		return 1;
	}
	return 0;
}

static	char 					deleteObject(pListHandle handle,pList object)
{
	pList plink,ptemp;
	if(handle != NULL&&object != NULL){
		plink = handle->head;
		ptemp = NULL;
		while(plink){
			if(plink == object){
				if(ptemp == NULL){
					if((handle->head = plink->pNext)== NULL){
						handle->tail   = NULL;
					}
					
				}else{
					if((ptemp ->pNext = plink->pNext) ==NULL){
						handle->tail = ptemp;
					}
				}
				handle->current = NULL;
				handle->count		--;
				handle->listCB->free(plink);
				return 1;
			}else{
				ptemp = plink;
				plink = ptemp->pNext;
			}
		}
	}
	return 0;
}

static	char 					removeObject(pListHandle handle,pList object)
{
	return deleteObject(handle,object);
}
static	char					removeNumber(pListHandle handle,uint16_t number)
{
	pList plink;
	if(handle != NULL){
		if(number <= handle->count){
				if(number >1){
					plink = handle->head;
					while(--number){
						plink = plink->pNext;
					}
					plink->pNext = plink->pNext->pNext;
					handle->count--;
					handle->listCB->free(plink);
					return 1;
				}else{
					if((plink = handle->head)!= NULL){
						if((handle->head = plink->pNext)==NULL){
							handle->tail = NULL;
						}
						handle->listCB->free(plink);
						handle->count--;
						return 1;
					}
					
				}
		}else{
			//do nothing
		}

	}
	return 0;
}
static	uint16_t			getNumber	 (pListHandle handle,pList object)
{
	pList plink;
	uint16_t i=0;
	if(handle != NULL){
		plink = handle->head;
		while(plink&&plink!= object){
			i++;
			plink = plink->pNext;
		}
		if(plink==NULL){
			return 0xffff;
		}else{
			return i;
		}
	}
	return 0xffff;
}

static	uint16_t			getSize		 (pListHandle handle)
{
	if(handle != NULL)return handle->count;
	return 0;
}

static	pList		 			getObject	 (pListHandle handle,uint16_t number)
{
	pList plink;
	if(handle != NULL){
		plink = handle->head;
		while(plink&&--number){
			plink = plink->pNext;
		}
		if(number){
			return plink;
		}else{
			return NULL;
		}
	}
	return NULL;
}

static	pList				  getFirst		 (pListHandle handle)
{
	if(handle != NULL){
		return handle->head;
	}
	return NULL;
}
static	pList				  getLast		 (pListHandle handle)
{
	if(handle != NULL){
		return handle->tail;
	}
	return NULL;
}

