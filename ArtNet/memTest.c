
#include "Artmem.h"
#include "stdio.h"

static mu8 array1[10000];
static mu8 array2[3000];
static void*phandle1,*phandle2;
static void*p[20];
void   memTest(void)
{
	phandle1 = memFunc.initMem(sizeof(array1),array1);
	phandle2 = memFunc.initMem(sizeof(array2),array2);
	
	printf("block1 = 0x%x\t",(mu32)phandle1);
	printf("block1 = %x%%\n",memFunc.getUsedPercent(phandle1));
	printf("block2 = 0x%x\n",(mu32)phandle2);
	printf("block2 = %x%%\n",memFunc.getUsedPercent(phandle2));
	
	// 首个存储取测试
	p[0] = memFunc.getMem(phandle1,100);
	memFunc.putMem(phandle1,p[0]);
	printf("Test0:get and put block1 free size = %d\n",(int)memFunc.getFreeSize(phandle1));
	
	// 两个存取 先取先回
	p[0] = memFunc.getMem(phandle1,100);
	printf("Test1:fifo block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	p[1] = memFunc.getMem(phandle1,100);
	printf("Test1:fifo block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1,p[0]);
	printf("Test1:fifo block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1,p[1]);
	printf("Test1:fifo block1 free size = %d\n",(int)memFunc.getFreeSize(phandle1));
	
	// 两个存取，先取后回
	p[0] = memFunc.getMem(phandle2,100);
	printf("Test2:filo block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	p[1] = memFunc.getMem(phandle2,100);
	printf("Test2:filo block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	memFunc.putMem(phandle2,p[1]);
	printf("Test2:filo block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	memFunc.putMem(phandle2,p[0]);
	printf("Test2:filo block2 free size = %d\n",(int)memFunc.getFreeSize(phandle2));
	
	// 申请跟内存大小一样的内存块
	p[0] = memFunc.getMem(phandle1,memFunc.getFreeSize(phandle1));
	printf("Test3:req same block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1,p[0]);
	printf("Test3:req same block1 free size = %d\n",(int)memFunc.getFreeSize(phandle1));
	
	p[0] = memFunc.getMem(phandle2,memFunc.getFreeSize(phandle2));
	printf("Test4:req same block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	memFunc.putMem(phandle2,p[0]);
	printf("Test4:req same block2 free size = %d\n",(int)memFunc.getFreeSize(phandle2));
	// 申请大于自身内存的内存快
	p[0] = memFunc.getMem(phandle1,memFunc.getFreeSize(phandle1)+100);
	printf("Test5:req same block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1,p[0]);
	printf("Test5:req more block1 free size = %d\n",(int)memFunc.getFreeSize(phandle1));
	
	p[0] = memFunc.getMem(phandle1,memFunc.getFreeSize(phandle2)+100);
	printf("Test6:req more block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	memFunc.putMem(phandle2,p[0]);
	printf("Test6:req more block2 free size = %d\n",(int)memFunc.getFreeSize(phandle2));

	// 申请大于自身内存的内存快
	p[0] = memFunc.getMem(phandle1,5000);
	// 多次释放
	printf("Test7:req more block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1, p[0]);
	printf("Test7:req more block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));
	memFunc.putMem(phandle1, p[0]);
	printf("Test7:req more block1 free size = %d\n", (int)memFunc.getFreeSize(phandle1));

	p[0] = memFunc.getMem(phandle2, 100000);
	printf("Test8:req more block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
	memFunc.putMem(phandle2, p[0]);
	printf("Test9:req more block2 free size = %d\n", (int)memFunc.getFreeSize(phandle2));
}
