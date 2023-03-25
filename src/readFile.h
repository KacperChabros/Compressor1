#ifndef _READFILE_H_
#define _READFILE_H_
typedef struct charInfo{	/*ZMIANY*/
	unsigned short value;
	int freq;
	struct charInfo *next;
}charInfo;

typedef struct fileInfo{
	int length;			/* size of bigbuffer */
	int counter;			/* number of characters in bigbuffer */
	int distinctChars;
	/*struct charInfo *character;*/
	/*unsigned char *bigbuffer;*/
}*fileInfo_t;
/*ZMIANY*/
unsigned short *readfile(fileInfo_t file,FILE *infile, unsigned short *bigbuffer, int compresslevel, int *lastBytesNotCompresed, unsigned char *notCompressedBytes); 
/*unsigned char *doubleSize(unsigned char *bigbuffer,int *length);*/
charInfo *frequency(fileInfo_t file, unsigned short *bigbuffer, charInfo *charinfo1);	/*ZMIANY*/
charInfo *addcharInfo(charInfo *info,unsigned short buffer, int *distinctChars);		/*ZMIANY*/
void freecharInfo(charInfo *charinfo1);

#endif
