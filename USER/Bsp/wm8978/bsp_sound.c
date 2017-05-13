
#include "bsp_sound.h"
#include <inttypes.h>
#include "bsp_wm8978.h"

#include "includes.h"
#include "bsp_sdram.h"
/* RECBUFFER_SIZE决定两个缓存区大小，实际每个缓存区占用RAM空间字节数为：RECBUFFER_SIZE*2
 * 对于选择I2S_AudioFreq_16k采样率使用RECBUFFER_SIZE为1024*4，就可保证录音及放音效果流畅，
 * 如果选择其他更高采样率需要适当调整用RECBUFFER_SIZE大小，比如当选择择I2S_AudioFreq_44k
 * 是设置RECBUFFER_SIZE为1024*8
 * 另外，SD卡本身的读写速度也在一定性能上影响录音效果
 */

/* 录音机状态 */
enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_RECORDING,	/* 录音状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_ERR,			/*  error  */
};

typedef struct
{
	uint8_t ucInput;			/* 输入源：0:MIC, 1:线输入 */
	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucGain;			  /* 当前增益 */	
	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */	
}REC_TYPE;	

/* WAV文件头格式 */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*/
	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数	*/
	uint32_t	wave;							/* = "WAVE" 0x45564157*/
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*/
	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16) */
	uint16_t	wFormatTag;				/* 编码方式,一般为1	*/
	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2 */
	uint32_t	dwSamplesPerSec;	/* 采样率 */
	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) */
	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数) */
	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) */
																			  
	uint32_t	data;							/* = "data" 0x61746164*/
	uint32_t	datasize;					/* 纯数据长度 */
} WavHead;	


/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT				6		/* 音频格式数组元素个数 */
#define RECBUFFER_SIZE  256

void MusicPlayer_I2S_DMA_TX_Callback(void);
void Recorder_I2S_DMA_RX_Callback(void);
static  void runSound(void*parameter);
static void StartRecord(void);

static REC_TYPE Recorder;          /* 录音设备 */

static char    enable;
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */
signed short buffer0[RECBUFFER_SIZE];//__attribute__((at(SDRAM_BANK_ADDR_S3 )));;  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
signed short buffer1[RECBUFFER_SIZE];//__attribute__((at(SDRAM_BANK_ADDR_S3 + sizeof(buffer0))));  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */

const uint16_t 	 recplaybuf[4]={0X0000,0X0000};
static  OS_TCB   soundTCB;
static  CPU_STK  soundSTK[APP_CFG_TASK_OBJ_STK_SIZE];
static  OS_SEM	 semSound;
static  void		 (*runCB)(int16_t*pdata,int16_t len);
static  uint16_t event;

uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};
void 		initSound(void)
{
	Recorder.ucStatus=STA_IDLE;    /* 开始设置为空闲状态  */
	Recorder.ucInput=1;            /* 0缺省MIC输入  */
	Recorder.ucFmtIdx=0;           /* 缺省飞利浦I2S标准，16bit数据长度，8K采样率  */
	Recorder.ucVolume=63;          /* 缺省耳机音量  */
	Recorder.ucGain=0;           /* 缺省线路输入增益 */
	
	// 在此处需要创建一个任务，用于传递DMA中断数据
	if(wm8978_Init() == 0){
		printf("检测不到wm8978芯片\r\n");
		return;
	}
	OS_ERR err;
	OSTaskCreate(&soundTCB,                              	/* Create the start task                                */
								"fileApp",
								runSound,
								0u,
								7,								/* App priority	*/
								&soundSTK[0u],
								soundSTK[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							  &err);
	runCB = NULL;
	enable = 0;
	event  = 0;
}

void    setSound(char bitEnable)
{
	OS_ERR err;
	// 可以用于唤醒当前任务
	if(bitEnable != enable){
		if(bitEnable ==0){
			// disable
			event |= 1;
			
		}else{
			// enable
			event |= 2;
		}
		enable = bitEnable;
		OSSemPost(&semSound,OS_OPT_POST_ALL,&err);
	}
}

void    setSoundCB(void(*cb)(int16_t *,int16_t len))
{
	runCB = cb;
}

static  void runSound(void*parameter)
{
	// 用于运行声音任务
	// 一旦有中断产生，则进行任务唤醒
	OS_ERR err;
	int    i;
	int16_t* pbuf;
	OSSemCreate(&semSound,"semSound",0,&err);
	if(err!=OS_ERR_NONE){printf("sound create error\n");return;}
	I2S_Stop();
	I2S_GPIO_Config();
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2S_DMA_TX_Callback=MusicPlayer_I2S_DMA_TX_Callback;
	I2S_Play_Stop();
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_Recorde_Stop();
	while(1){
		
		OSSemPend(&semSound,1000,OS_OPT_PEND_BLOCKING,NULL,&err);

		if(event &0x01){
			event &=~0x01;
			// 禁止设备
			I2Sxext_Recorde_Stop();
			I2S_Play_Stop();
			I2S_Stop();		/* 停止I2S录音和放音 */
			wm8978_Reset();	/* 复位WM8978到复位状态 */
		}
		if(event &0x02){
			event &= ~0x02;
			// 是能设备
			StartRecord();
		}
		if(event &0x04){
			event &= ~0x04;
			// DMA中断，需要发送数据
			if(bufflag == 0){pbuf = buffer0;}
			else {pbuf = buffer1;}
			if(runCB!= NULL){
				runCB(pbuf,256);
//				for(i=0;i<8;i++){
//					runCB(pbuf,1024);
//					pbuf += 1024;
//				}
			}
		}
		if(event&0x08){
			// y9ongyu录音的
			event&=~0x08;
		}
	}
	// 创建信号量
}

/* DMA接收完成中断回调函数 */
/* 录音数据已经填充满了一个缓冲区，需要切换缓冲区，
   同时可以把已满的缓冲区内容写入到文件中 */
void Recorder_I2S_DMA_RX_Callback(void)
{
	OS_ERR err;
	if(enable== STA_RECORDING)
	{	
		if(I2Sxext_RX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
		{
			bufflag=0;
			runCB(buffer0,256);
		}
		else                                 //当前使用Memory0数据
		{
			bufflag=1;
			runCB(buffer1,256);
		}
	}
}

/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	OS_ERR err;
	if(enable == STA_PLAYING)
	{
		if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
		{
			bufflag=0;                       //可以将数据读取到缓冲区0
		}
		else                               //当前使用Memory0数据
		{
			bufflag=1;                       //可以将数据读取到缓冲区1
		}
	}
	event |= 0x08;
	OSSemPost(&semSound,OS_OPT_POST_ALL,&err);
}
static void StartRecord(void)
{
	OS_ERR err;

	OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
	I2S_Stop();			/* 停止I2S录音和放音 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */

	Recorder.ucStatus = STA_RECORDING;		/* 录音状态 */
		
	/* 调节放音音量，左右相同音量 */
	wm8978_SetOUT1Volume(Recorder.ucVolume);

	/* 配置WM8978芯片，输入为线输入，输出为耳机LINE_ON| */
	wm8978_CfgAudioPath( MIC_LEFT_ON | ADC_ON,  EAR_RIGHT_ON);//wm8978_CfgAudioPath(LINE_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_SetLineGain(Recorder.ucGain);
		
	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2Sx_TX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);
	DMA_ITConfig(I2Sx_TX_DMA_STREAM,DMA_IT_TC,DISABLE);//开启传输完成中断
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_RX_DMA_Init((const uint16_t*)buffer0,(const uint16_t*)buffer1,RECBUFFER_SIZE);
  	
	I2S_Play_Start();
	I2Sxext_Recorde_Start();
}


