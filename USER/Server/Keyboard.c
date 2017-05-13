

#include "Keyboard.h"

static void keyboardListener(unsigned char data);
static char status;
static AppKeyListener pListener;

static unsigned char  len;
static KeyMsg   msg;
static char     index;
void initKeyBoard(void)
{
	setUsartListener(keyboardListener);
	status =0;
}

void setKeyListener(AppKeyListener listener)
{
	pListener = listener;
}
static void keyboardListener(unsigned char data)
{
	switch(status){
		case 0:if(data == 0xa5)status++;break;
		case 1:if(data == 0xa5)status++;break;
		case 2:len = data;msg.len = len;index =0;if(len<=KEYMAX&&len >0)status ++;else status =0;break;
		case 3:msg.parameter[index++] = data;
		if(--len ==0){status =0;if(pListener)pListener(&msg);}
		break;
		default:status = 0;
	}
	
}
