

#include "bsp_led.h"

#include "fileApp.h"
#include "stdio.h"
#include "RecordFile.h"
#include "server.h"
//#include "api_lib.h"
#include "bsp_time.h"
#include "hal_time.h"

#define DATA_MAX_SIZE			1000
#define RECORD_NUM				2

static  OS_TCB                        fileAppTCB;
static  CPU_STK                       fileAppSTK[APP_CFG_TASK_OBJ_STK_SIZE];

static  void runFileApp(void*parameter);
static  void runTime(void);
static  void timeUp(int id);
static  void runTimeTest(void);

static  RecordFile										record[RECORD_NUM];
static  pRecordFile                   pread;
static  pRecordFile                   pwrite;

static  struct udp_pcb* pcb;
static  struct pbuf*pbuf;
static  struct pbuf BUFFER;
static  TimeHandle timeHandle;
static  OS_SEM			semFileOperator;
void initFileApp(void*p_arg)
{
	OS_ERR err;
	OSTaskCreate(&fileAppTCB,                              	/* Create the start task                                */
								"fileApp",
								runFileApp,
								0u,
								APP_CFG_TASK_FILEAPP_PRIO,								/* App priority	*/
								&fileAppSTK[0u],
								fileAppSTK[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							  &err);
	
	if(err!= OS_ERR_NONE){
		printf("create File App error\r\n");
	}
	pbuf = &BUFFER;
	pbuf->ref = 1;
	pbuf->type = PBUF_ROM;
	pbuf->flags = 0;
	pbuf->next = NULL;

//	timeHandle = createTime(1,0,getTimeTick(0,100,500),timeUp);
//	getTimerHandle()->startTimeSec(1,0,runTimeTest);
	pwrite = &record[0];
}
static  char runFlag = 0;
static  void runFileApp(void*parameter)
{
	int count =1;
	int sumSize = 0;
	OS_ERR err;
	int tick = 0;
	struct ip_addr*dest_ip;
	pcb= getUDPArtNet();
	printf("fileApp is running\r\n");
	OSSemCreate(&semFileOperator,"fileOperator",0,&err);
	if(err!= OS_ERR_NONE){
		printf("file operator sem create fault\r\n");
		while(1);
	}
	OSSemPend(&semNetworkState,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	// 先建立UDP通信说需要的pbuf
	// 读取文件信息,链接时间发送器
	// 获取文件数据属性，并通过Udp包封装发送出去。
	
	openRecordFile();
//	// 读取第一个数据包
//	runFlag = 1;
//	getTimerHandle()->startTimerSec(1,0,runTimeTest);
//	while(runFlag){
//		if(readRecordFile(pwrite) ==0){
//			tick ++;
//		}else{
//			rewindRecordFile();
//		}
//	}
//	printf("file test :file readTick = %d\r\n",tick);
//	tick = 0;
//	readRecordFile(pwrite);
//	runFlag = 1;
//	pbuf = pbuf_alloc(PBUF_RAW,pwrite->header.len,PBUF_ROM);if(pbuf==NULL)return ;
//	//getTimerHandle()->startTimerSec(1,0,runTimeTest);
//	while(runFlag){
//		pwrite->header.destIP = htonl(pwrite->header.destIP);
//		pbuf->payload = pwrite->data;
//		pbuf->len = pbuf->tot_len = 520;//pwrite->header.len;
//		if(udp_sendto(pcb,pbuf,(ip_addr_t*)&pwrite->header.destIP,pwrite->header.destPort)!=ERR_OK){
//			printf("sent error tick = %d\r\n",tick);
//		}else
//		tick++;
//	}

	pbuf = pbuf_alloc(PBUF_RAW,pwrite->header.len,PBUF_ROM);if(pbuf==NULL)return ;
//	printf("internet test: netcount = %d\r\n",tick);
	if(readRecordFile(pwrite)== 0){
////		pwrite->header.destIP 
		//pwrite->header.destIP = htonl(pwrite->header.destIP);
		pbuf->payload = pwrite->data;
	//pwrite->header.destPort = htons(pwrite->header.destPort);
			// 在此处发送udp报文,需要转换字节序
		udp_sendto(pcb,pbuf,(ip_addr_t*)&pwrite->header.destIP,pwrite->header.destPort);
		//pbuf_free(pbuf);
	}
//	startTime(timeHandle);
	
	// 形成时间帧
	while(1){
		pwrite++;
		if(pwrite> &record[RECORD_NUM-1]){
			pwrite = &record[0];
		}       
		if(readRecordFile(pwrite)== 0&&((pwrite->header.srcIP>>24)&0xff)==108){
			
			//printf("tick = %d\r\n",tick++);
			if(getTimerHandle()->startTimerSec(pwrite->header.sec,pwrite->header.usec,runTime) ==1){
				OSSemPend(&semFileOperator,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			}
//		  modifyTime(timeHandle,getTimeTick(pwrite->header.sec,0,pwrite->header.usec));
			pbuf->len = pbuf->tot_len = pwrite->header.len;
			//pbuf = pbuf_alloc(PBUF_RAW,pwrite->header.len,PBUF_ROM);if(pbuf==NULL)return ;
		  pbuf->payload = pwrite->data;
			// 在此处发送udp报文
			pbuf->payload = pwrite->data;
			//pbuf->len     = pwrite->header.len;
			//pwrite->header.destIP = htonl(pwrite->header.destIP);
		  //pwrite->header.destPort = htons(pwrite->header.destPort);
			if(udp_sendto(pcb,pbuf,(ip_addr_t*)&pwrite->header.destIP,pwrite->header.destPort) != ERR_OK){
				printf("data send error\r\n");
			}
			//pbuf_free(pbuf);
		}else if(((pwrite->header.srcIP>>24)&0xff)!=107){
			if(getTimerHandle()->startTimerSec(pwrite->header.sec,pwrite->header.usec,runTime) ==1){
				OSSemPend(&semFileOperator,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			}
			printf("srcIP!= 192.168.1.107\r\n");
		}else{
			printf("rewindRecordFile count is%d\r\n",count++);
			rewindRecordFile();continue;
		}
		
	}
	closeRecordFile();
	pbuf_free(pbuf);
//	pbuf_free(pbuf);
}

// 时间播放器,用于产生时间序列
static void timeUp(int id)
{
	OS_ERR err;
	// 需要Modify time
	//OSSemPost(&semFileOperator,OS_OPT_POST_ALL|OS_OPT_POST_NO_SCHED,&err);
}

static  void runTime(void)
{
	OS_ERR err;
	//LED2_TOGGLE;
	// 需要Modify time
	//OSSemPost(&semFileOperator,OS_OPT_POST_ALL,&err);
}
static void runTimeTest()
{
	runFlag = 0;
}

