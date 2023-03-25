#ifndef _COMPRESS_H_
#define _COMPRESS_H_
#include <stdbool.h>
struct values{
	char A;
	char B;
};
typedef union data{
	short buf; 
	struct values Val;
	/*struct values{
		char A;
		char B;
	};*/
}data_t;
dictionary *findCode(dictionary *dict,unsigned short symbol);

void binWrite(dictionary *dict, unsigned short *bigbuffer ,FILE *outfile,int counter, int compressLevel, bool cypher, unsigned char checksum, int lastBytesNotCompressed, unsigned char *notCompressedBytes,bool info);
#endif
