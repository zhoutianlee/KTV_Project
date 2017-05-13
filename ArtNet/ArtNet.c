

#include "ArtNet.h"
#include "includes.h"
#include "server.h"
#include "udp.h"
#include "bsp_time.h"
#include "string.h"
#include "ArtMem.h"
#include "TimerServer.h"
#include "DMXOut.h"
#include "Model.h"
#include "Model_Normal.h"
#include "Model_KTV.h"
#include "Model_Techo.h"
#include "KeyBoard.h"												// 装载键盘驱动
#include "stdio.h"
#include "hal_time.h"
#include "bsp_sdram.h"
#define MODE_TYPE_DMXOUT		1
#define MAXCOUNT						20							// 每次最多发送20包数据

#define setTime1Event()		  taskEvent|= 0x01
#define setTime2Event()			taskEvent|= 0x02
#define setEventCalculate()	taskEvent|= 0x04
#define setKeyboardEvent()	taskEvent|= 0x10

#define checkTime1Event()		taskEvent&0x01
#define checkTime2Event()		taskEvent&0x02
#define checkEventCalculate()	taskEvent&0x04
#define checkKeyboardEvent()	taskEvent&0x10

#define clearTime1Event()		taskEvent&=~0x01
#define clearTime2Event()		taskEvent&=~0x02
#define clearEventCalculate() taskEvent&=~0x04
#define clearKeyboardEvent()  taskEvent&=~0x10

static  Int8		 artMemPool[20000]__attribute__((at(SDRAM_BANK_ADDR_S0)));//__attribute__((at(SDRAM_BANK_ADDR_S0)));
static  Int8     timeMemPool[2000]__attribute__((at(SDRAM_BANK_ADDR_S0 + sizeof(artMemPool))));
//static  ArtDmx   artDMXArray[100]__attribute__((at(SDRAM_BANK_ADDR_S3)));;

static  OS_TCB   artNetAppTCB;
static  CPU_STK  artNetAppSTK[APP_CFG_TASK_OBJ_STK_SIZE];
struct  udp_pcb* pudp;
static  u8       taskEvent;
static  OS_SEM	 semArtNetOperator,semDisp;
pHandle artMemHandle;
pHandle timeMemHandle;
//static  ArtPoll  artPoll;
static  ArtDmx	 artDmx;
static  int      fp,errCount;
const static char artPoll[14] = {0x41,0x72,0x74,0x2D,0x4E,0x65,0x74,0x00,0x00,0x20,0x00,0x0E,0x06,0x00};

static void runArtNet(void*parg);
static void artNetCB(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port);
static void runArtNetStatus(void*parameter);
static void time1CB(void*parameter);
static void time2CB(void*parameter);
static void time3CB(void*parameter);
static void keyListener(pKeyMsg keymsg);
static KeyMsg keyMsg;
static void sendArtNetDMX(struct    pbuf* p,struct  udp_pcb* pudp);

void uiTest(void);

void initArtNet(void*parg)
{
	OS_ERR err;
	int i;
	artMemHandle  = memFunc.initMem(sizeof(artMemPool),artMemPool);
	timeMemHandle = memFunc.initMem(sizeof(timeMemPool),timeMemPool);
	initArtDmx(&artDmx);
	initArtNetConfig();
	initModel();																						// 初始化模式
	initKeyBoard();																					// 初始化键盘驱动
//	getModel()->setUp();
//	setModelType(MODELTYPE_NORMAL,0);													// ktv模式
//	setModelType(MODELTYPE_KTV,2);													// ktv模式
	if(artMemHandle == NULL||timeMemHandle == NULL){
		printf("mem init create fall\r\n");
		
	}else{
		printf("artMem init create success\r\n");
	}
	OSTaskCreate(&artNetAppTCB,                              	/* Create the start task                                */
								"artNetApp",
								runArtNet,
								0u,
								APP_CFG_TASK_ARTNETAPP_PRIO,								/* App priority	*/
								&artNetAppSTK[0u],
								artNetAppSTK[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							  &err);
	
//	for(i=0;i<100;i++){
//		initArtDmx(&artDMXArray[i]);
//	}
}

static void runArtNet(void*parg)
{
	OS_ERR 						err;
	err_t  						merr;
	
	static ip_addr_t 	addr;
	TimeHandle* 			time1;
	TimeHandle* 			time2;

	struct    pbuf*   pbuf;
//	setModelType(MODELTYPE_DIANYIN,4);
//	setModelType(MODELTYPE_NORMAL,4);	
	setModelType(MODELTYPE_KTV,1);													// ktv模式
	getTimerHandle()->initTimer(100);
	printf("run ArtNet\r\n");
	OSSemPend(&semNetworkState,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	
//	uiTest();
	pudp = udp_new();																		// 获取udp链接
	merr = udp_bind(pudp,IP_ADDR_ANY,6454);      		 		// 端口绑定
	
	if(merr != ERR_OK){
		printf("udp bind error\r\n");
		while(1);
	}
	udp_recv(pudp,artNetCB,NULL);												// 设置端口回调
	
	addr.addr = getIPAddress();
	addr.addr|= 0xff<<24;																// 设置广播地址
	
	errCount = fp = 0;
	appendTimerServer(3000,time1CB,NULL,1);
	appendTimerServer(1000/30,time2CB,NULL,1);//ARTNET_FREQFLASH
//	appendTimerServer(1000,time3CB,NULL,1);
	appendTimerServer(1000/20,time3CB,NULL,1);					// 用于计算
	OSSemCreate(&semArtNetOperator,"artNetSem",0,&err);
	OSSemCreate(&semDisp,"semDisp",0,&err);

	if(err != OS_ERR_NONE ||time1 == NULL||time2== NULL){
		printf("os semcreate error\r\n");
		while(1);
	}
	taskEvent = 0;																			// 任务事件标志
	setKeyListener(keyListener);
	while(1){
	
		// 等待任务呗唤醒
		setmsTick(0);
		OSSemPend(&semArtNetOperator,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		if(checkTime1Event()){
			
			clearTime1Event();
			pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtPoll),PBUF_ROM);
			addr.addr = getIPAddress();
			addr.addr|= 0xff<<24;																
			pbuf->payload = (char*)&artPoll;
			if(udp_sendto(pudp,pbuf,&addr,6454) != ERR_OK){
				printf("error udp msg\r\n");
			}
			oldArtDevice();																	// 刷新设备
			
			pbuf_free(pbuf);
			// 进行设备控制

		}
		if(checkTime2Event()){
			//printf("time2 callback id \r\n");
			clearTime2Event();
//			
//			plink = getArtDeviceHead();
//			pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtDmx),PBUF_ROM);
			pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtDmx),PBUF_ROM);
			if(pbuf==NULL)return;
			sendArtNetDMX(pbuf,pudp);												// 发送ArtNet数据包
			fp ++;
//			while(plink!=NULL){
//				
//				if(plink->start ==1){
//					createArtDmx(&plink->node,&artDmx,512,artDmx.Data);
//					if(toggle ==0){
//						toggle = 1;
//						for(i = 0;i<512;i++){artDmx.Data[i] = 0xff;}
//					}else{
//						toggle = 0;
//						for(i = 0;i<512;i++){artDmx.Data[i] = 0x00;}
//					}
//					pbuf->len = pbuf->tot_len = sizeof(ArtDmx);
//					pbuf->payload = &artDmx;

//					addr.addr = getIPAddress();
//					addr.addr|= 0xff<<24;//可采用广播的形式
//					for(i = 0;i<4;i++){
//						artDmx.SubUni = plink->node.SwOut[i];
//						if(udp_sendto(pudp,pbuf,&addr,0x1936)!= ERR_OK){//(ip_addr_t*)plink->node.IPAddress,
//							printf("dmx send error\r\n");
//						}
//					}
//				}
//				plink = plink->pNext;
//			}
			pbuf_free(pbuf);
//			setEventCalculate();
		}
		if(checkEventCalculate()){
			clearEventCalculate();
			// 
			//  此处用于回掉
			runModel(getNodeListHead(),NULL);
			//getModel()->run(getNodeListHead(),NULL);
		}
		if(checkKeyboardEvent()){
			clearKeyboardEvent();
			
			// 
			
			printf("len = %d\tmsg1=%x msg2=%x msg3=%x msg4=%x msg5=%x\n",keyMsg.len,keyMsg.parameter[0],keyMsg.parameter[1],keyMsg.parameter[2],keyMsg.parameter[3],keyMsg.parameter[4]);
			setModelType(keyMsg.parameter[0],keyMsg.parameter[1]);
			setCurrentData(MODELDATA_COLOR,keyMsg.parameter[2]);
			setCurrentData(MODELDATA_DIMMER,keyMsg.parameter[3]);
//			setModelType((keyMsg.parameter[0]-1)&0xf,(keyMsg.parameter[0]>>4)&0x0f);
		}

	}
	
}
	
static void artNetCB(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port)
{
	// 获得ARTNET POLL REPLAY等数据应答
//	printf("udp recieve,ip=%d.%d.%d.%d\tport is=%d\r\n",(uint8_t)(addr->addr),(uint8_t)(addr->addr >> 8), \
//																					(uint8_t)(addr->addr >> 16),(uint8_t)(addr->addr >> 24),port);



	if(((pArtPoll)p->payload)->OpCode== OpPollReply){
//		printf("get something\r\n");
		if(addArtDevice((pArtPollReply)p->payload) ==1){
			
			
			// 如果设备添加成功，则需要运行状态机
//			runArtNetStatus(searchDeviceReply((pArtPollReply)p->payload)->node.IPAddress);
//			pBuf = pbuf_alloc(PBUF_RAW,sizeof(ArtPoll),PBUF_ROM);
//			if(pBuf != NULL){
//			// 需要发送ArtTodReq
//			// 创建数据报文
//				createArtTodRequest(&searchDeviceReply((pArtPollReply)p->payload)->node,&request);
//				pBuf->payload = (char*)&request;
//				if(udp_send(upcb,pBuf)!= ERR_OK){
//					printf("udpSend artNetCB call error\r\n");
//				}
//				pbuf_free(pBuf);
//			}
			runArtNetStatus(searchDeviceReply((pArtPollReply)p->payload)->node.IPAddress);
		}
		//runArtNetStatus(searchDeviceReply((pArtPollReply)p->payload)->node.IPAddress);
	}
	
	pbuf_free(p);
}

static void time1CB(void*parameter)
{
	OS_ERR err;
	// 用于形成固定时间的ＡＲＴＮＥＴ　ＰＯＬＬ数据包
	setTime1Event();
	OSSemPost(&semArtNetOperator,OS_OPT_POST_ALL,&err);
}

static void time2CB(void*parameter)
{
	OS_ERR err;
	//　用于形成固定的帧速
	setTime2Event();
	OSSemPost(&semArtNetOperator,OS_OPT_POST_ALL,&err);
}

static void time3CB(void*parameter)
{
	OS_ERR err;
//	printf("fp = %d \t errCount = %d\r\n",fp,errCount);
	setEventCalculate();
//	setTime3Event();
	OSSemPost(&semArtNetOperator,OS_OPT_POST_ALL,&err);
}	

static void time4CB(void)
{
	OS_ERR err;
	OSSemPost(&semDisp,OS_OPT_POST_ALL,&err);
}
static void keyListener(pKeyMsg pkeymsg)
{
	OS_ERR err;
	keyMsg = *pkeymsg;
	setKeyboardEvent();
	OSSemPost(&semArtNetOperator,OS_OPT_POST_ALL,&err);
}

static void runArtNetStatus(void*parameter)
{
	pArtDevice  		node;
  ArtTodRequest	 	request;
  ArtIpProg				ipProg;
  ArtTodControl   artTodControl;
	struct    pbuf* pbuf;

	if(parameter!=NULL){
		node = searchArtDevice((Int8*)parameter);

		if(node!=NULL&&node->start==0&&node->connected ==1){
#if	ARTNET_DEBUG
		
//			printf("\nnode status =%d\r\n",node->status);
//			if(node->status == 4){
//				printf("node status is come\r\n");
//			}
#endif
			switch(node->status){
				case 0:{
					// 刚加入网络，需要发送OpTodRequest信息，但先延迟约50ms
					appendTimerServer(50,runArtNetStatus,parameter,0);
//					time = createTimeMSec(0,parameter,50,runArtNetStatus);
					node->status ++;
//					if(time!=NULL){
//						startTime(time);
//					}else{
//#if	ARTNET_DEBUG
//						printf("time create error234\r\n");
//#endif						
//					}
				}break;
				case 1:{
					// 发送OpTodRequest指令
					pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtTodRequest),PBUF_ROM);
					createArtTodRequest(&node->node,&request);
					if(pbuf!=NULL){
						pbuf->payload = &request;
						if(udp_sendto(pudp,pbuf,(ip_addr_t*)parameter,0x1936)!=ERR_OK){
#if	ARTNET_DEBUG
							printf("udp send to error line = 245\r\n");
#endif							
						}
					}else{
#if	ARTNET_DEBUG
							printf("pbuf createError = 252\r\n");
						return;
#endif										
					}
					
					pbuf_free(pbuf);
					appendTimerServer(1,runArtNetStatus,parameter,0);
//					time = createTime(0,parameter,1,runArtNetStatus);
//					if(time!= NULL){
//						startTime(time);
//					}else{
//#if	ARTNET_DEBUG
//						printf("run ArtNet status error line=256\r\n");
//#endif						
//					}
					node->status++;
				}break;
				case 2:{
					// 发送OpIpProg命令
					pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtIpProg),PBUF_ROM);
					if(pbuf!=NULL){
						
						createArtIpProg(&node->node,&ipProg);
						pbuf->payload = &ipProg;
						if(udp_sendto(pudp,pbuf,(ip_addr_t*)parameter,0x1936)!=ERR_OK){
#if	ARTNET_DEBUG
							printf("udp send to error line = 277\r\n");
#endif							
						}
					}else{
#if	ARTNET_DEBUG
							printf("pbuf createError = 281\r\n");
						return;
#endif												
					}
					pbuf_free(pbuf);
					
					node->status++;
					// 正常会受到下一个应答，为保证状态机运行正常，需要用定时器约束
					appendTimerServer(3,runArtNetStatus,parameter,0);
//					time = createTimeMSec(0,parameter,3,runArtNetStatus);
//					if(time!=NULL){
//						startTime(time);
//					}else{
//#if	ARTNET_DEBUG
//						printf("time create error 342\r\n");
//#endif								
//					}
				}break;
				case 3:{
					// 准备进入下一状态
					appendTimerServer(170,runArtNetStatus,parameter,0);
//					time = createTimeMSec(0,parameter,170,runArtNetStatus);
//					if(time!=NULL){
//						startTime(time);
//					}else{
//#if	ARTNET_DEBUG
//						printf("time create error 297\r\n");
//#endif								
//					}
					node->status++;
				}break;
				case 4:
				case 5:
				case 6:
				case 7:{
					// 发送控制命令
					pbuf = pbuf_alloc(PBUF_RAW,sizeof(ArtIpProg),PBUF_ROM);
					
					if(pbuf!=NULL){
						createArtTodControl(&node->node,&artTodControl,ArtTodCmd_AtcFlush,node->status-4);
						pbuf->payload = &artTodControl;
						if(udp_sendto(pudp,pbuf,(ip_addr_t*)parameter,0x1936)!=ERR_OK){
#if	ARTNET_DEBUG
							printf("udp send to error line = 314\r\n");
#endif							
						}
					}else{
#if	ARTNET_DEBUG
							printf("pbuf createError = 281\r\n");
						return;
#endif												
					}
					pbuf_free(pbuf);
					appendTimerServer(1,runArtNetStatus,parameter,0);
//					time = createTime(0,parameter,1,runArtNetStatus);
//					if(time!=NULL){
//						startTime(time);
//					}else{
//#if	ARTNET_DEBUG
//						printf("time create error 332\r\n");
//#endif								
//					}
					if(node->status == 7){
						node->start = 1;
						node->status = 0;
						registerNodeList(&node->node);
						// 将结束这个状态机
					}else{
						node->status++;
					}
				}break;
				default:{
					node->status = 0;
					
				}
				
			}
		}
	}
}


#if MODE_TYPE_DMXOUT == 0
static void sendArtNetDMX(struct pbuf* p,struct udp_pcb* pudp)
{
	pNodeList  plist = getNodeListHead();
	pUnionNode punion;
	Int16      len;
	ip_addr_t  addr;
	static Int16			 Currentcount =0;
	Int16			 count=0;
	Int8			 reStart =0;
//	createArtDmx(&plink->node,&artDmx,512,artDmx.Data);
	p->payload = &artDmx;
	p->len = p->tot_len = sizeof(artDmx);
	// 第一步，先寻找会Current的数据位置
	while(plist){
		punion = plist->pHead;
		while(punion){
			if(count == Currentcount)goto ARTNET_DMXOUT_NEXT;
			count ++;
			punion = (pUnionNode)punion->arrayNode.pNext;
		}
		plist = plist->pNext;
	}
	
ARTNET_DMXOUT_NEXT:
	if(Currentcount != count){
		Currentcount =0;
		count =0;
		plist = getNodeListHead();
	}
	count =0;
	// 分时显示
ARTNET_DMXOUT_START:	
	while(plist){
		if(plist->enable != 1){
			plist = plist->pNext;
			if(++count >= MAXCOUNT) goto ARTNET_DMXOUT_END;
			else continue;
		}
		punion = plist->pHead;
		
		while(punion){
			if(punion->lineNode.msg.isExit !=1){
					punion = (pUnionNode)punion->arrayNode.pNext;
					if(++count >= MAXCOUNT) goto ARTNET_DMXOUT_END;
					continue;
			}
			len = punion->arrayNode.func(plist->parray,punion,512,(Int8*)artDmx.Data);				// 数据拷贝
//			p->len = p->tot_len = len + 18;																									// 用于传输数据头
			
			addr.addr = (punion->arrayNode.msg.IPAddress[3]<<24)|(punion->arrayNode.msg.IPAddress[2]<<16)|(punion->arrayNode.msg.IPAddress[1]<<8)|(punion->arrayNode.msg.IPAddress[0]);
			artDmx.Length   = len&0xff;
			artDmx.LengthHi = len>>8;
			artDmx.Net  		= punion->arrayNode.msg.Net;
			artDmx.Sequence = punion->arrayNode.msg.Sequence++;
			artDmx.SubUni   = punion->arrayNode.msg.SubUni;
			
			if(punion->lineNode.msg.Sequence ==0){punion->lineNode.msg.Sequence++;}
			if(udp_sendto(pudp,p,&addr,0x1936)!=ERR_OK)errCount++;
			
			punion = (pUnionNode)punion->arrayNode.pNext;
			
			if(++count >= MAXCOUNT)goto ARTNET_DMXOUT_END;
		}
		plist = plist->pNext;
	}
	if(plist == NULL){
//		plist = getNodeListHead();
		Currentcount =0;
//		if(reStart++ ==0)goto ARTNET_DMXOUT_START;
		goto ARTNET_DMXOUT_EXIT;
	}
ARTNET_DMXOUT_END:
	Currentcount += MAXCOUNT;
ARTNET_DMXOUT_EXIT:
	printf("count = %d\n",count);
	count =0;
}
#else
static void sendArtNetDMX(struct pbuf* p,struct udp_pcb* pudp)
{
	pNodeList  plist = getNodeListHead();
	pUnionNode punion=NULL ;
	Int16      len;
	ip_addr_t  addr;
	OS_ERR err;
//	createArtDmx(&plink->node,&artDmx,512,artDmx.Data);
	Int16      i;
	struct pbuf* pbuf;
	Int16 count=0;
	Int8*			 psrc,*pdest;
	p->payload = &artDmx;
//	p->len = p->tot_len = sizeof(artDmx);

		while(plist){
			
			if(plist->enable != 1){plist = plist->pNext;continue;}																// 如果当前组件被禁止，这扫描下一个组件
			
			punion = plist->pHead;
			while(punion){
					
				if(punion->lineNode.msg.isExit !=1)
				{punion = (pUnionNode)punion->arrayNode.pNext;continue;}
				
				len = punion->arrayNode.func(plist->parray,punion,512,(Int8*)artDmx.Data);				// 数据拷贝
//				len = punion->arrayNode.func(plist->parray,punion,512,(Int8*)artDMXArray[count].Data);
				p->len = p->tot_len = len + 18;																									// 用于传输数据头
				
				addr.addr = (punion->arrayNode.msg.IPAddress[3]<<24)|(punion->arrayNode.msg.IPAddress[2]<<16)|(punion->arrayNode.msg.IPAddress[1]<<8)|(punion->arrayNode.msg.IPAddress[0]);
				artDmx.Length   = len&0xff;
				artDmx.LengthHi = len>>8;
				artDmx.Net  		= punion->arrayNode.msg.Net;
				artDmx.Sequence = 0;//punion->arrayNode.msg.Sequence++;
				artDmx.SubUni   = punion->arrayNode.msg.SubUni;

				if(punion->lineNode.msg.Sequence ==0){punion->lineNode.msg.Sequence++;}
				if(udp_sendto(pudp,p,&addr,0x1936)!=ERR_OK)errCount++;
//				pbuf_free(pbuf);
				punion = (pUnionNode)punion->arrayNode.pNext;
//				if(++count > 20)return;
//				printf("wait for mintues\r\n");
//				if(++count >20)
//				OSTimeDlyHMSM(0u, 0u, 0u, 1u,
//									OS_OPT_TIME_HMSM_STRICT,
//									&err);

//				OSSemPend(&semDisp,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			}
			plist = plist->pNext;
		}

}
#endif
