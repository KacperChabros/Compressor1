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
dictionary *findCode(dictionary *dict,unsigned char symbol);
void binWrite(dictionary *dict, unsigned char *bigbuffer ,FILE *outfile,int counter,int compressLevel, bool cypher,unsigned char checksum);
#endif
