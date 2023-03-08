#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_
#include "readFile.h"

typedef struct node{
	int value;
	char whichChildAmI; 			/*defines whether a node is a left child '0' of its parent or right child '1'. 'N' is set by default*/
	int freq;
	struct node *leftChild;
	struct node *rightChild;
	struct node *parent;
} node;

typedef struct priorityQ{
	struct node *Qnode;
	struct priorityQ *next;
} priorityQ;

typedef struct dictionary{
	unsigned char symbol;
	char *code;
	struct dictionary *next;
} dictionary;



dictionary *makeDictionary(fileInfo_t file1, charInfo *charInfo1);

#endif
