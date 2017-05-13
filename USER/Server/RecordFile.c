
#include "RecordFile.h"
#include "ff.h"
#include "stdio.h"

static FIL fp;
static FRESULT result = FR_EXIST;
static RecordFile record;
static RecordFile mrecord;
static void initMyRecord(void);
static void copyRecord(pRecordFile psrc,pRecordFile pdest);
static void fileTest(void)
{
	int i;
	UINT len;
	int  count;
	initMyRecord();
	result = f_open(&fp,"mytest1.txt",FA_WRITE);
	if(result!= FR_OK){
		printf("file test fault\r\n");
		return;
	}
	record.header.destIP = 0x88888888;
	record.header.srcIP  = 0x22222222;
	record.header.destPort = 0x3333;
	record.header.srcPort = 0x4444;
	record.header.sec = 0;
	record.header.usec = 0;
	for(i = 0; i < 1500;i++){
		record.data[i] = i;
	}
	count =0;
	for(i = 0; i< 1000;i++,count++){
		record.header.len = i+10;
		record.header.usec = 0xaa;
		record.header.sec = 0xbb;
		result =f_write(&fp,&record.header,sizeof(RecordHeader),&len);if(result != FR_OK){break;}
		result =f_write(&fp,record.data,record.header.len,(UINT*)&len);if(result != FR_OK){break;}
	}
	printf("file write count is %d\r\n",count);
	f_close(&fp);
	result = f_open(&fp,"mytest1.txt",FA_READ);
	if(result != FR_OK){
		printf("file test fault read\r\n");
		return;
	}
	count = 0;
	while(1){
		
		result =f_read(&fp,&record.header,sizeof(RecordHeader),&len);
		if(result != FR_OK||len!= sizeof(RecordHeader)||record.header.len>1500){break;}
		result = f_read(&fp,&record.data,record.header.len,&len);
		if(result != FR_OK||len!= record.header.len){break;}
		count++;
	}
	printf("file read count is %d\r\n",count);
	result = FR_EXIST;
	
}
void openRecordFile  (void)
{
	printf("==========================================================\r\n");
	fileTest();
	printf("==========================================================\r\n");
	if(result == FR_OK)return;
		if((result=f_open(&fp,RECORDFILE_NAME,FA_READ))!= FR_OK){
			printf("file open fault\r\n");
			return;
		}else{
			printf("file open success\r\n");
		}
}
void rewindRecordFile(void)
{
	if(result == FR_OK){
		f_rewind(&fp);
	}
}

char readRecordFile  (pRecordFile p)
{
	UINT len;
	FRESULT res;
//	copyRecord(&mrecord,p);return 0;
	if(result == FR_OK&& p!=NULL){
		res =f_read(&fp,(char*)&p->header,sizeof(RecordHeader),&len);
		if(len!= sizeof(RecordHeader)){
			return 1;
			printf("len is not equal sizoef(RecordHeader)\r\n");
			
		}
		if(res != FR_OK){
			return res;
		}
		if(p->header.len > 1500){
			return FR_EXIST;
		}
		res =f_read(&fp,(char*)p->data,p->header.len,&len);
		if(len != p->header.len){
			return 1;
			printf("len is not equal p->header.len\r\n");
		}
		return res;
	}
	return 1;
}

void closeRecordFile (void)
{
	if(result == FR_OK){
		f_close(&fp);
		result = FR_EXIST;
		printf("文件已关闭\r\n");
	}
}
const static char artPoll[14] = {0x41,0x72,0x74,0x2D,0x4E,0x65,0x74,0x00,0x00,0x20,0x00,0x0E,0x06,0x00};
static void initMyRecord(void)
{
	int i;
	mrecord.header.destIP = 0xc0a8016b;
	mrecord.header.destPort = 0x1936;
	mrecord.header.len      = 14;
	mrecord.header.sec      = 0;
	mrecord.header.usec     = 5000;
	mrecord.header.srcIP    = 0xc0a80165;
	mrecord.header.srcPort = 0x1936;
	for(i=0;i<14;i++){
		mrecord.data[i] = artPoll[i];
	}
}

static void copyRecord(pRecordFile psrc,pRecordFile pdest)
{
	int i;
	pdest->header.destIP = psrc->header.destIP;
	pdest->header.destPort = psrc->header.destPort;
	pdest->header.len      = psrc->header.len;
	pdest->header.sec      = psrc->header.sec;
	pdest->header.usec     = psrc->header.usec;
	pdest->header.srcIP    = psrc->header.srcIP;
	pdest->header.srcPort = psrc->header.srcPort;
	for(i=0;i<psrc->header.len;i++){
		pdest->data[i] = psrc->data[i];
	}
}