#ifndef _TRIE_H_
#define _TIRE_H_
#include <stdbool.h>

typedef struct trieNode{
	struct trieNode *children[2];
	bool isEndOfWord;
	unsigned short symbol;
} trieNode;

trieNode *addNode();

void addWord(trieNode *root, char *code, unsigned short symbol);

trieNode *lookForSymbol(trieNode *root, char *code);

void freeTrie(trieNode *root);

#endif
