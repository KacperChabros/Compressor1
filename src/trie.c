#include "trie.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
trieNode *addNode()
{
	int i;
	trieNode *newNode = malloc( sizeof(*newNode));

	if(newNode != NULL)
	{
		newNode->isEndOfWord = false;
		newNode->symbol = 0;
		for(i = 0; i < 2; i++)
		{
			newNode->children[i] = NULL;
		}
	}
	return newNode;
}

void addWord(trieNode *root, char *code, unsigned short symbol)
{
	int length = strlen(code);
	int i;
	trieNode *iter = root; 

	for( i = 0; i < length; i++)
	{
		if( code[i] == '0' )
		{
			if( iter->children[0] == NULL )
				iter->children[0] = addNode();
			iter = iter->children[0];
		}
		else if( code[i] == '1' )
		{
			if( iter->children[1] == NULL )
				iter->children[1] = addNode();
			iter = iter->children[1];
		}
	}	
	
	iter->isEndOfWord = true;
	iter->symbol = symbol;
}

trieNode *lookForSymbol(trieNode *root, char *code)
{
	int i;
	int length = strlen(code);
	trieNode *iter = root;

	for( i = 0; i < length; i++)
	{
		if( code[i] == '0')
		{
			if( iter->children[0] == NULL)
			      return NULL;
			iter = iter->children[0];	      
		}
		else if( code[i] == '1')
		{
			if( iter->children[1] == NULL)
				return NULL;
			iter = iter->children[1];
		}
	}

	if( iter->isEndOfWord == true )
	{
		return iter;
	}
	else
	{
		return NULL;
	}
}

void freeTrie(trieNode *root)
{
	int i;
	if(root == NULL)
	{
		return;
	}
	else
	{
		for(i=0; i<2; i++)
			freeTrie(root->children[i]);
	}
	free(root);
}
