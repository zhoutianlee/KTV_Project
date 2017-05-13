
#ifndef __KEYBOARD__H
#define __KEYBOARD__H

#include "bsp_debug_usart.h"

#define KEYMAX				5

typedef struct KeyMsg{
	int  len;
	char parameter[KEYMAX];
}KeyMsg,*pKeyMsg;

typedef void (*AppKeyListener)(pKeyMsg msg);

void initKeyBoard(void);
void setKeyListener(AppKeyListener listener);

#endif
