

#include "Server.h"
#include "stdio.h"
#include "api.h"
#include "udp.h"
#include "netif/etharp.h"
#include "bsp_time.h"
#include "TimerServer.h"

//typedef struct UDPPortStruct{

//	u16_t 	port;
//	NetUDPCallback callback;
//	struct udp_pcb* upd;
//	
//}UDPPort_t;

typedef enum   ArtNetOpCode{

	OpPoll      = 0x2000,
	OpPollReply = 0x2100,
	OpDmx       = 0x5000
}ArtNetOpCode;

typedef struct ArtPoll{
	char  ID[8];
	short OpCode;
	char  ProtVerHi;
	char  ProtVerLo;
	char  TalkToMe;
	char  Priority;
	
}ArtPoll_t,*pArtPoll_t;

typedef struct ArtNzs{

	char  ID[8];
	short OpCode;
	char  ProtVerHi;
	char  ProtVerLo;
	char  Sequence;
	char  StartCode;
	char  SubUni;
	char  Net;
	char  LengthHi;
	char  LengthLo;
	char  Data[512];
	
}ArtNzs_t,*pArtNzs_t;

typedef struct ArtPollReply{
	char  ID[8];
	short OpCode;
	char  IPAddress[4];
	char  Port[2];
	char  VersInfoH;
	char  VersInfoL;
	char  NetSwitch;
	char  SubSwitch;
	char  OemHi;
	char  Oem;
	char  UbeaVersion;
	char  Status1;
	char  EstaManLo;
	char  EstaManHi;
	char  ShortName[18];
	char  LongName[64];
	char  NodeReport[64];
	char  NumPortsHi;
	char  NumPortsLo;
	char  PortTypes[4];
	char  GoodInput[4];
	char  GoodOutput[4];
	char  SwIn[4];
	char  SwOut[4];
	char  SwVideo;
	char  SwMacro;
	char  SwRemote;
	char  Spare[3];
	char  Style;
	char  Mac[6];
	char  BindIp[4];
	char  BindIndex;
	char  Status2;
	char  Filler[26];
	
}ArtPollReply_t,*pArtPollReply_t;
OS_SEM 			semNetworkState;
OS_SEM			semFileState;
FATFS				fs;
const static char artPoll[14] = {0x41,0x72,0x74,0x2D,0x4E,0x65,0x74,0x00,0x00,0x20,0x00,0x0E,0x06,0x00};
const static char shortName[16] = "Unixe ArtNet Box";
const static char shortName1[16] = "Hello ArtNet Box";
const static char longName[64]  = "Unixe ArtNet Box Make by Feige 2017-03-03";
const static char longName1[64]  = "Hello ArtNet Box Make by Feige 2017-03-03";

static char artPollReply[239] = {
	0x41,0x72,0x74,0x2D,0x4E,0x65,0x74,0x00,0x00,0x21,0xC0,0xA8,0x01,0x67,0x36,0x19,0x04,0x4E,0x00,0x00,
	0x04,0xC4,0x00,0x00,0x41,0x49,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x42,0x6F,0x78,0x00,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x4C,0x65,0x64,0x20,0x41,0x72,0x74,0x6E,
	0x65,0x74,0x5F,0x73,0x41,0x43,0x4E,0x20,0x42,0x6F,0x78,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x4C,0x65,0x64,0x20,
	0x42,0x6F,0x78,0x20,0x69,0x73,0x20,0x4F,0x4B,0x20,0x31,0x31,0x36,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x04,0x80,0x80,0x80,0x80,0x00,0x00,
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xA7,0x36,0xE6,0x63,0xF3,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char artPollReply1[239]={
	0x41,0x72,0x74,0x2D,0x4E,0x65,0x74,0x00,0x00,0x21,0xC0,0xA8,0x01,0x67,0x36,0x19,0x04,0x4E,0x00,0x00,
	0x04,0xC4,0x00,0x00,0x41,0x49,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x42,0x6F,0x78,0x00,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x4C,0x65,0x64,0x20,0x41,0x72,0x74,0x6E,
	0x65,0x74,0x5F,0x73,0x41,0x43,0x4E,0x20,0x42,0x6F,0x78,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,0x61,0x68,0x61,0x6E,0x27,0x73,0x20,0x4C,0x65,0x64,0x20,
	0x42,0x6F,0x78,0x20,0x69,0x73,0x20,0x4F,0x4B,0x20,0x31,0x31,0x36,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x04,0x80,0x80,0x80,0x80,0x00,0x00,
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xA7,0x36,0xE6,0x63,0xF3,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
struct udp_pcb* udpConnect;
	
static NetUDPCallback udpCallback;
static void initArtNetBox(void);
static  OS_TCB   ServerTCB;
static  CPU_STK  ServerSTK[APP_CFG_TASK_OBJ_STK_SIZE];
	
static void semServer(void);
static void fileServer(void);
static void initUDPNetwork(void);
static void runServer(void*p_arg);
static void myCallbackTest(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port);

void   initServer(void*p_arg)
{
	OS_ERR err;
	
	initListTimerServer();
	semServer();
//	fileServer();
	OSTaskCreate(&ServerTCB,                              	/* Create the start task                                */
								"fileApp",
								runServer,
								0u,
								APP_CFG_TASK_SERVER_PRIO,									/* App priority	*/
								&ServerSTK[0u],
								ServerSTK[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							  &err);
	if(err!= OS_ERR_NONE){
		printf("create initServer App error\r\n");
	}
}

// 正式启动的任务
static void runServer(void*p_arg)
{
	OS_ERR err;
	
	printf("run Server\r\n");
	initUDPNetwork();
	printf("end Server initation\r\n");
	while(1){
		
		OSTimeDlyHMSM(0u, 0u, 0u, 1u,
									OS_OPT_TIME_HMSM_STRICT,
									&err);
		runTimerServer();															// 运行定时任务管理
		sortTimerServer();														// 整理内存
		// 此处用于发送ArtPoll
		//closeTime(NULL);															// 定时清理内存
	}
}

static void semServer(void)
{
	OS_ERR  err;
	OSSemCreate(&semNetworkState,"networkState",0,&err);
	if(err == OS_ERR_NONE){
		printf("network sem  creates success\r\n");
	}
	OSSemCreate(&semFileState,"fileState",0,&err);
	if(err == OS_ERR_NONE){
		printf("file sem creates success\r\n");
	}
}

static void fileServer(void)
{
	FRESULT res_sd; 
	OS_ERR	err;
	// 文件系统服务
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
			
			printf("《《格式化失败。》》\r\n");
		}
	}
  else if(res_sd!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
		OSSemPost(&semFileState,OS_OPT_POST_ALL|OS_OPT_POST_NO_SCHED,&err);
		if(err == OS_ERR_NONE){
			printf("文件系统已就绪\r\n");
		}
  }
}

static void initUDPNetwork(void)
{
	OS_ERR err;
	err_t  merr;
	ip_addr_t addr;
	pArtPollReply_t pReply = (pArtPollReply_t)artPollReply;
	
	
	printf("init UDP Network\r\n");
	
	OSSemPend(&semNetworkState,0,OS_OPT_PEND_BLOCKING,NULL,&err);
//	initArtNetBox();
//	printf("is ready to run udp server\r\n");
//	udpConnect = udp_new();//(NETCONN_UDP);
//	if(udpConnect == NULL){
//		printf("udp对象创建失败\r\n");
//	}
//	
//	merr = udp_bind(udpConnect,IP_ADDR_ANY,5000);
//	if(merr == ERR_OK){
//		printf("udp端口绑定成功\r\n");
//	}
//	//udp_recv(udpConnect,udpCallback,NULL);
//	udp_recv(udpConnect,myCallbackTest,NULL);
	
	//setUDPCallback(0x1936,myCallbackTest);
}

void   setUDPListener(int port,NetUDPCallback func)
{
	udpCallback = func;
//	if(port == 100){
//		udpCallback = func;
//	}
}

// artNet 数据回调
static void myCallbackTest(void*arg,struct udp_pcb*upcb,struct pbuf*p,struct ip_addr*addr,u16_t port)
{
	uint32_t ipaddr;
	struct pbuf* pBuf;
	unsigned char ipArray[4];
	if(udpCallback != NULL)udpCallback(arg,upcb,p,addr,port);
	pArtPoll_t      pPoll  = (pArtPoll_t)artPoll;
	pArtPoll_t      pSrc   = (pArtPoll_t)p->payload;
	pArtPollReply_t pReply = (pArtPollReply_t)artPollReply;
	pArtNzs_t       pArtNzs;
	char msg[] = "this is a text!\r\n";

	ipaddr = addr->addr;
	printf("udp recieve,ip=%d.%d.%d.%d\rport is=%d\r\n",(uint8_t)(ipaddr),(uint8_t)(ipaddr >> 8), \
																					(uint8_t)(ipaddr >> 16),(uint8_t)(ipaddr >> 24),port);
	
	switch(pSrc->OpCode){
	
		case OpDmx:{
			pArtNzs = (pArtNzs_t)p->payload;
			printf("udp dmx data recieve,size = %d\tSubUni =%d\tNet = %d\tlen = %d\r\n",p->tot_len,pArtNzs->SubUni,pArtNzs->Net,pArtNzs->LengthLo+(pArtNzs->LengthHi<<8));
			
		}break;
		case OpPoll:{
			
			pBuf = pbuf_alloc(PBUF_RAW,sizeof(artPollReply),PBUF_ROM);
			pBuf->payload = pReply;
			udp_sendto(upcb,pBuf,addr,port);
			pReply->IPAddress[0] = 192;
			pReply->IPAddress[1] = 168;
			pReply->IPAddress[2] = 1;
			pReply->IPAddress[3] = 105;
			udp_sendto(upcb,pBuf,addr,port);
			pReply->IPAddress[0] = 192;
			pReply->IPAddress[1] = 168;
			pReply->IPAddress[2] = 1;
			pReply->IPAddress[3] = 106;
			udp_sendto(upcb,pBuf,addr,port);
			pReply->IPAddress[0] = 192;
			pReply->IPAddress[1] = 168;
			pReply->IPAddress[2] = 1;
			pReply->IPAddress[3] = 107;
			udp_sendto(upcb,pBuf,addr,port);
			pReply->IPAddress[0] = 192;
			pReply->IPAddress[1] = 168;
			pReply->IPAddress[2] = 1;
			pReply->IPAddress[3] = 108;
			udp_sendto(upcb,pBuf,addr,port);
//			pBuf->payload = artPollReply1;
//			udp_sendto(upcb,pBuf,addr,port);
			pbuf_free(pBuf);
		}break;
		case OpPollReply:{
			// 其他盒子的信息可以忽略
		}break;
	}
	if(p!=NULL)pbuf_free(p);
	if(p!=NULL)
	{
		udp_sendto(upcb,p,addr,port);
		udp_sendto(upcb,pBuf,addr,port);
		pbuf_free(p);
	}else{
		udp_sendto(upcb,pBuf,addr,port);
	}
	
	pbuf_free(pBuf);
}
// 初始化ArtNet盒子
static void initArtNetBox(void)
{
	int i;
	char*p = (char*)0x1fff7a10;
	pArtPollReply_t pReply = (pArtPollReply_t)artPollReply;
	pReply->IPAddress[0] = IPADDR_ANY&0XFF;
	pReply->IPAddress[1] = IPADDR_ANY>>8;
	pReply->IPAddress[2] = IPADDR_ANY>>16;
	pReply->IPAddress[3] = IPADDR_ANY>>24;
	
	for(i=0;i<sizeof(shortName)/sizeof(char);i++){
		pReply->ShortName[i] = shortName[i];
	}
	
	for(i=0;i<sizeof(longName)/sizeof(char);i++){
		pReply->LongName[i] = longName[i];
	}
	
	pReply->SwOut[3] = 9;
	
	for(i=0;i<6;i++){
		pReply->Mac[i] = *p++;
	}
	
	pReply = (pArtPollReply_t)artPollReply1;
	
	pReply->IPAddress[0] = 192;
	pReply->IPAddress[1] = 168;
	pReply->IPAddress[2] = 1;
	pReply->IPAddress[3] = 105;
	//etharp_add_static_entry((ip_addr_t*)pReply->IPAddress,(struct eth_addr * )pReply->Mac);
	pReply->IPAddress[0] = 192;
	pReply->IPAddress[1] = 168;
	pReply->IPAddress[2] = 1;
	pReply->IPAddress[3] = 106;
	//etharp_add_static_entry((ip_addr_t*)pReply->IPAddress,(struct eth_addr * )pReply->Mac);
	for(i=0;i<sizeof(shortName)/sizeof(char);i++){
		pReply->ShortName[i] = shortName1[i];
	}
	
	for(i=0;i<sizeof(longName)/sizeof(char);i++){
		pReply->LongName[i] = longName1[i];
	}
	
	pReply->SwOut[3] = 9;
	p = (char*)0x1fff7a10;
	for(i=0;i<6;i++){
		pReply->Mac[i] = *p++;
	}
	
}

struct udp_pcb*getUDPArtNet(void)
{
	return udpConnect;
}
