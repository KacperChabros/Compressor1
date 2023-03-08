#ifndef _READFILE_H_
#define _READFILE_H_
typedef struct charInfo{
	unsigned char value;
	int freq;
	struct charInfo *next;
}charInfo;

typedef struct fileInfo{
	int length;
	int counter;
	int distinctChars;
	/*struct charInfo *character;*/
	/*unsigned char *bigbuffer;*/
}*fileInfo_t;
unsigned char *readfile(fileInfo_t file,FILE *infile, unsigned char *bigbuffer);
/*unsigned char *doubleSize(unsigned char *bigbuffer,int *length);*/
charInfo *frequency(fileInfo_t file, unsigned char *bigbuffer, charInfo *charinfo1);
charInfo *addcharInfo(charInfo *info,unsigned char buffer, int *distinctChars);
void freecharInfo(charInfo *charinfo1);

#endif
