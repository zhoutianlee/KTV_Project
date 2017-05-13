

#ifndef __ILIST__H
#define __ILIST__H

#include "includes.h"

typedef struct List{
	struct List* pNext;
}List,*pList;

typedef struct IList{
	void*   (*malloc) (uint16_t size);
	void		(*free)		(void*);
}IList,*pIList;

typedef struct ListHandle{
	
	pIList				listCB;
	
	pList					head;
	pList 			 	tail;
	pList					current;
	
	uint16_t		  objectSize;
	uint16_t			count;
}ListHandle,*pListHandle;

typedef struct IListFunc{
	
	pListHandle		(*createHandle)(uint16_t objectSize,pIList listCB);
	pList 				(*createObject)(pListHandle	handle);
	char					(*appendObject)(pListHandle handle,pList object);
	char 					(*insertObject)(pListHandle handle,pList object);
	char 					(*deleteObject)(pListHandle handle,pList object);
	char 					(*removeObject)(pListHandle handle,pList object);
	char					(*removeNumber)(pListHandle handle,uint16_t number);
	uint16_t			(*getNumber)	 (pListHandle handle,pList object);
	uint16_t			(*getSize)		 (pListHandle handle);
	pList		 			(*getObject)	 (pListHandle handle,uint16_t number);
	pList				  (*getFirst)		 (pListHandle handle);
	pList				  (*getLast)		 (pListHandle handle);
	
}IListFunc,*pIListFunc;

extern const IListFunc listFunc;

#endif
