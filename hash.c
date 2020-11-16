#include <stdlib.h>
#include "list.h"



#include <stdio.h>

int hashCode(int key) {
   return key % 53;
}

struct list *search(int key, struct list *hashArray[106], int flag){
	int pos;
	struct list *cur;

	pos= hashCode(key);
	if(flag){
		pos+=53;
	}

	cur=hashArray[pos];
	while(cur!=NULL){				// Anatrexei tin lista
		if(cur->key==key){
			return cur;				// An to brei epistrefei ton kombo tis listas ston opoio brethike
		}
		cur= cur->next;
	}
	return NULL;					// Alliws epistrefei NULL
}

void insert(int key, char *ref, struct list *hashArray[106], int flag){
	int pos, i;
	struct list *cur;

	pos= hashCode(key);
	if(flag){
		pos+=53;
	}

	cur=hashArray[pos];
	if(hashArray[pos]==NULL){							// An einai i prwti eisagwgi stin lista
		hashArray[pos]= malloc(sizeof(struct list));	// Kanei malloc kai kanei tin eisagwgi
		hashArray[pos]->key=key;
		hashArray[pos]->next=NULL;
		for(i=0; i<11; i++){
			hashArray[pos]->ref[i]=ref[i];
		}
	}else{
		while(cur->next!=NULL){							// Alliws trexei mexri to telos tis listas
			cur= cur->next;
		}
		cur->next= malloc(sizeof(struct list));			// Opou kanei to malloc kai tin eisagwgi
		cur->next->key=key;
		cur->next->next=NULL;
		for(i=0; i<11; i++){
			cur->next->ref[i]=ref[i];
		}
	}
}

int delete(struct list *hashArray[106], int flag, int *w){
	int pos, i, count;
	struct list *cur, *next;

	count=0;
	pos=0;
	if(flag){
		pos=53;
	}
	for(i=0; i<53; i++){
		cur=hashArray[i+pos];
		if(hashArray[i+pos]!=NULL){
			while(cur!=NULL){			// Mexri na ftasei sto telos tis listas
				next=cur->next;
				if(cur->ref[9]=='W'){	// An i ref[9] eixe tin endeixi W auxanei ton metriti twn writes
					*w=*w+1;
				}
				free(cur);				// Sbinei tin eggrafi
				cur=next;
				count++;				// Auxanei ton metriti twn kombwn poy esbise
			}
		}
	}
	return count;						// Epistrefei posous kombous esbise
}
