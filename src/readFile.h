#ifndef _READFILE_H_
#define _READFILE_H_
typedef struct charInfo{
	unsigned short value;
	int freq;
	struct charInfo *next;
}charInfo;

typedef struct fileInfo{
	int length;			/* size of bigbuffer */
	int counter;			/* number of characters in bigbuffer */
	int distinctChars;
}*fileInfo_t;
unsigned short *readfile(fileInfo_t file,FILE *infile, unsigned short *bigbuffer, int compresslevel, int *lastBytesNotCompresed, unsigned char *notCompressedBytes); 
charInfo *frequency(fileInfo_t file, unsigned short *bigbuffer, charInfo *charinfo1);
charInfo *addcharInfo(charInfo *info,unsigned short buffer, int *distinctChars);
void freecharInfo(charInfo *charinfo1);

#endif
