#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "readFile.h"


int cmp(const void *a, const void *b)
{
	node *na = *(node **)a;
	node *nb = *(node **)b;
	if(na->freq < nb->freq)
		return -1;
	else if(na->freq == nb->freq)
	{
		if(na->value < nb->value)
			return -1;
		else if(na->value == nb->value)
			return 0;
		else return 1;

		return 0;
	}
	else
		return 1;
}

int rcmp(const void *a, const void *b)
{
	return -cmp(a,b);
}

char *revstr(char *string, int length)
{
	if(!string || ! *string)
		return string;
	char tmp;
	length-=1;
	int i, j=0;
	for(i = length; i>length/2; i--)
	{
		tmp = string[i];
		string[i] = string[j];
		string[j++] = tmp;
	}
	return string;
}

priorityQ *enqueue(priorityQ *pQ1, node *newNode, int *queuedItems) 	/*add to priority queue*/
{
	if(pQ1 == NULL)
	{
		pQ1 = malloc(sizeof(pQ1));
		pQ1->Qnode = newNode;
		pQ1->next = NULL;
	}else{
		priorityQ *tmp = malloc(sizeof(*tmp));
		tmp->Qnode = newNode;
		tmp->next = NULL;

		priorityQ *i;
		if( pQ1->Qnode->freq >= tmp->Qnode->freq )
		{
			tmp->next = pQ1;
			(*queuedItems)++;
			return tmp;
		}
		for(i=pQ1; i->next != NULL; i=i->next)
		{
			if( i->next->Qnode->freq >= tmp->Qnode->freq)
			{
				tmp->next = i->next;
				i->next = tmp;
				break;
			}
		}
		i->next = tmp;
	}
	(*queuedItems)++;
	return pQ1;
}

priorityQ *pop(priorityQ *pQ1, int *queuedItems)
{
	if( *queuedItems > 1 )
	{
		priorityQ *tmp1 = pQ1;				/*pop 2 front elements from the queue*/
		priorityQ *tmp2 = pQ1->next;
	
		priorityQ *newHead = pQ1->next->next;

		tmp1->next = NULL;
		tmp2->next = NULL;
		
		node *newNode = malloc(sizeof(*newNode));	/*create a new node with the sum of frequency*/
		newNode->value = -1;
		newNode->whichChildAmI = 'N';
		newNode->freq = tmp1->Qnode->freq + tmp2->Qnode->freq;
		newNode->leftChild = tmp1->Qnode;
		newNode->rightChild = tmp2->Qnode;
		newNode->parent = NULL;

		tmp1->Qnode->whichChildAmI = '0';		/*assign popped nodes as children*/
		tmp1->Qnode->parent = newNode;

		tmp2->Qnode->whichChildAmI = '1';
		tmp2->Qnode->parent = newNode;

		(*queuedItems)-=2;
		
		free(tmp1);
		free(tmp2);

		newHead = enqueue(newHead, newNode, queuedItems);
		return newHead;
	}else{
		fprintf(stderr, "There is only root item in the queue!\n");
		return pQ1;
	}

}

dictionary *addToDictionary(dictionary *dict, node *leaf)
{
	node *iterNode;
	int codeLength = 0;
	int j=0;
	char *currCode = malloc(sizeof(*currCode));
	for(iterNode = leaf; iterNode->parent != NULL; iterNode=iterNode->parent)	/*creating a code for the symbol on leaf*/
	{
		codeLength++;
		currCode = realloc(currCode, codeLength+1);
		*(currCode + codeLength - 1)  = iterNode->whichChildAmI;
	}
	currCode[codeLength] = '\0';
	currCode = revstr(currCode, codeLength);

	if(dict == NULL)
	{
		dict = malloc(sizeof(dict));
		dict->symbol = leaf->value;
		dict->bitLength = codeLength;
		dict->code = malloc( codeLength * sizeof(dict->code));
		strcpy(dict->code, currCode);
		dict->next = NULL;
	}else{
		dictionary *tmp = malloc(sizeof(*tmp));
		tmp->symbol = leaf->value;
		tmp->bitLength = codeLength;
		tmp->code = malloc(codeLength * sizeof(tmp->code));
		strcpy(tmp->code, currCode);
		tmp->next = NULL;

		dictionary *i;
		for(i=dict; i->next != NULL; i=i->next)
			;
		i->next = tmp;
	}
	free(currCode);
	return dict;
}

void freeTree(node *currNode)
{
	if(currNode == NULL)
	{
		return;
	}else{
		freeTree(currNode->leftChild);

		freeTree(currNode->rightChild);

		free(currNode);
	}
}

void freeDict(dictionary *dict1)
{
	dictionary *i;
	dictionary *tmp;
	if(dict1 == NULL)
	{
		return;
	}
	else if(dict1->next == NULL)
	{
		free(dict1->code);
		free(dict1);
	}
	else
	{
		tmp = dict1;
		for(i=dict1->next; i->next != NULL; i=i->next)
		{
			free(tmp->code);
			free(tmp);
			tmp = i;
		}
		free(i->code);
		free(i);
	}	
}

dictionary *makeDictionary(fileInfo_t file1, charInfo *charInfo1)
{
	int i, j;
	int queuedItems = 0;
	charInfo *iterCharInfo = charInfo1;
	node *nodes[file1->distinctChars];
	
	for(i=0; i<file1->distinctChars || iterCharInfo != NULL; i++)	/*adding to the struct of nodes*/
	{
		node *tmp = malloc(sizeof(*tmp));
		tmp->value = iterCharInfo->value;
		tmp->freq = iterCharInfo->freq;
		tmp->whichChildAmI = 'N';
		tmp->leftChild = NULL;
		tmp->rightChild = NULL;
		tmp->parent = NULL;
		nodes[i] = tmp;
		iterCharInfo = iterCharInfo->next;		
	}
	qsort(nodes, file1->distinctChars, sizeof(node*), cmp);		/*sorting the nodes in ascending order*/
	
	
	priorityQ *prior1 = NULL;
	for(i=0; i<file1->distinctChars; i++)
	{
		prior1 = enqueue(prior1, nodes[i], &queuedItems);
	}
	while(queuedItems > 1)						/*making a tree*/
	{
		prior1 = pop(prior1, &queuedItems);
	}
	qsort(nodes, file1->distinctChars, sizeof(node*), rcmp);
	
	
	dictionary *dict1 = NULL;
	
	for(i=0; i<file1->distinctChars; i++)				/*creating a dictionary*/
	{
		dict1 = addToDictionary(dict1, nodes[i]);
	}
	freeTree(prior1->Qnode);
	free(prior1);
	return dict1;
}
