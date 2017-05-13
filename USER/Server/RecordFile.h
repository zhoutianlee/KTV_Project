


#ifndef __RECORDFILE__H
#define __RECORDFILE__H


#define RECORDFILE_MAXSIZE		1500										// 
#define RECORDFILE_NAME				"RecordFile.dat"				// ÎÄ¼þÃû³Æ

#include <includes.h>

typedef struct   RecordHeader {

	u16   len;

	u32   destIP;
	u32   srcIP;
	u16   destPort;
	u16   srcPort;

	u32   sec;
	u32   usec;
}RecordHeader,*pRecordHeader;

typedef struct RecordFile {

	RecordHeader header;
	u8           data[RECORDFILE_MAXSIZE];

}RecordFile,*pRecordFile;

void openRecordFile  (void);
void rewindRecordFile(void);
char readRecordFile  (pRecordFile p);
void closeRecordFile (void);

#endif

