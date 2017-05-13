
#include "bsp_sound.h"
#include <inttypes.h>
#include "bsp_wm8978.h"

#include "includes.h"
#include "bsp_sdram.h"
/* RECBUFFER_SIZE����������������С��ʵ��ÿ��������ռ��RAM�ռ��ֽ���Ϊ��RECBUFFER_SIZE*2
 * ����ѡ��I2S_AudioFreq_16k������ʹ��RECBUFFER_SIZEΪ1024*4���Ϳɱ�֤¼��������Ч��������
 * ���ѡ���������߲�������Ҫ�ʵ�������RECBUFFER_SIZE��С�����統ѡ����I2S_AudioFreq_44k
 * ������RECBUFFER_SIZEΪ1024*8
 * ���⣬SD������Ķ�д�ٶ�Ҳ��һ��������Ӱ��¼��Ч��
 */

/* ¼����״̬ */
enum
{
	STA_IDLE = 0,	/* ����״̬ */
	STA_RECORDING,	/* ¼��״̬ */
	STA_PLAYING,	/* ����״̬ */
	STA_ERR,			/*  error  */
};

typedef struct
{
	uint8_t ucInput;			/* ����Դ��0:MIC, 1:������ */
	uint8_t ucFmtIdx;			/* ��Ƶ��ʽ����׼��λ��������Ƶ�� */
	uint8_t ucVolume;			/* ��ǰ�������� */
	uint8_t ucGain;			  /* ��ǰ���� */	
	uint8_t ucStatus;			/* ¼����״̬��0��ʾ������1��ʾ¼���У�2��ʾ������ */	
}REC_TYPE;	

/* WAV�ļ�ͷ��ʽ */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*/
	uint32_t	size_8;						/* ���¸���ַ��ʼ���ļ�β�����ֽ���	*/
	uint32_t	wave;							/* = "WAVE" 0x45564157*/
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*/
	uint32_t	fmtSize;					/* ��һ���ṹ��Ĵ�С(һ��Ϊ16) */
	uint16_t	wFormatTag;				/* ���뷽ʽ,һ��Ϊ1	*/
	uint16_t	wChannels;				/* ͨ������������Ϊ1��������Ϊ2 */
	uint32_t	dwSamplesPerSec;	/* ������ */
	uint32_t	dwAvgBytesPerSec;	/* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) */
	uint16_t	wBlockAlign;			/* ÿ���������ֽ���(=����������/8*ͨ����) */
	uint16_t	wBitsPerSample;		/* ����������(ÿ��������Ҫ��bit��) */
																			  
	uint32_t	data;							/* = "data" 0x61746164*/
	uint32_t	datasize;					/* �����ݳ��� */
} WavHead;	


/* ��Ƶ��ʽ�л��б�(�����Զ���) */
#define FMT_COUNT				6		/* ��Ƶ��ʽ����Ԫ�ظ��� */
#define RECBUFFER_SIZE  256

void MusicPlayer_I2S_DMA_TX_Callback(void);
void Recorder_I2S_DMA_RX_Callback(void);
static  void runSound(void*parameter);
static void StartRecord(void);

static REC_TYPE Recorder;          /* ¼���豸 */

static char    enable;
static uint8_t bufflag=0;          /* ���ݻ�����ѡ���־ */
signed short buffer0[RECBUFFER_SIZE];//__attribute__((at(SDRAM_BANK_ADDR_S3 )));;  /* ���ݻ�����1 ��ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */
signed short buffer1[RECBUFFER_SIZE];//__attribute__((at(SDRAM_BANK_ADDR_S3 + sizeof(buffer0))));  /* ���ݻ�����2 ��ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */

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
	Recorder.ucStatus=STA_IDLE;    /* ��ʼ����Ϊ����״̬  */
	Recorder.ucInput=1;            /* 0ȱʡMIC����  */
	Recorder.ucFmtIdx=0;           /* ȱʡ������I2S��׼��16bit���ݳ��ȣ�8K������  */
	Recorder.ucVolume=63;          /* ȱʡ��������  */
	Recorder.ucGain=0;           /* ȱʡ��·�������� */
	
	// �ڴ˴���Ҫ����һ���������ڴ���DMA�ж�����
	if(wm8978_Init() == 0){
		printf("��ⲻ��wm8978оƬ\r\n");
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
	// �������ڻ��ѵ�ǰ����
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
	// ����������������
	// һ�����жϲ����������������
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
			// ��ֹ�豸
			I2Sxext_Recorde_Stop();
			I2S_Play_Stop();
			I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
			wm8978_Reset();	/* ��λWM8978����λ״̬ */
		}
		if(event &0x02){
			event &= ~0x02;
			// �����豸
			StartRecord();
		}
		if(event &0x04){
			event &= ~0x04;
			// DMA�жϣ���Ҫ��������
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
			// y9ongyu¼����
			event&=~0x08;
		}
	}
	// �����ź���
}

/* DMA��������жϻص����� */
/* ¼�������Ѿ��������һ������������Ҫ�л���������
   ͬʱ���԰������Ļ���������д�뵽�ļ��� */
void Recorder_I2S_DMA_RX_Callback(void)
{
	OS_ERR err;
	if(enable== STA_RECORDING)
	{	
		if(I2Sxext_RX_DMA_STREAM->CR&(1<<19)) //��ǰʹ��Memory1����
		{
			bufflag=0;
			runCB(buffer0,256);
		}
		else                                 //��ǰʹ��Memory0����
		{
			bufflag=1;
			runCB(buffer1,256);
		}
	}
}

/* DMA��������жϻص����� */
/* �����������Ѿ�������ɣ���Ҫ�л��������������»��������ݲ��� 
   ͬʱ��ȡWAV�ļ�������䵽�Ѳ�������  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	OS_ERR err;
	if(enable == STA_PLAYING)
	{
		if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //��ǰʹ��Memory1����
		{
			bufflag=0;                       //���Խ����ݶ�ȡ��������0
		}
		else                               //��ǰʹ��Memory0����
		{
			bufflag=1;                       //���Խ����ݶ�ȡ��������1
		}
	}
	event |= 0x08;
	OSSemPost(&semSound,OS_OPT_POST_ALL,&err);
}
static void StartRecord(void)
{
	OS_ERR err;

	OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err);
	I2S_Stop();			/* ֹͣI2S¼���ͷ��� */
	wm8978_Reset();		/* ��λWM8978����λ״̬ */

	Recorder.ucStatus = STA_RECORDING;		/* ¼��״̬ */
		
	/* ���ڷ���������������ͬ���� */
	wm8978_SetOUT1Volume(Recorder.ucVolume);

	/* ����WM8978оƬ������Ϊ�����룬���Ϊ����LINE_ON| */
	wm8978_CfgAudioPath( MIC_LEFT_ON | ADC_ON,  EAR_RIGHT_ON);//wm8978_CfgAudioPath(LINE_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_SetLineGain(Recorder.ucGain);
		
	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2Sx_TX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);
	DMA_ITConfig(I2Sx_TX_DMA_STREAM,DMA_IT_TC,DISABLE);//������������ж�
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_RX_DMA_Init((const uint16_t*)buffer0,(const uint16_t*)buffer1,RECBUFFER_SIZE);
  	
	I2S_Play_Start();
	I2Sxext_Recorde_Start();
}


