#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h> 
#include <sys/stat.h>
#include <string.h>
#include "Semun.h"
#include "list.h"
#include "hash.h"

static int set_semvalue(int, int);
static void del_semvalue(int);
static int down(int);
static int up(int);
int get_semval(int);
static int sem_id;
int power(int, int);

int main(int argv, char *argc[]){
	int i, k, q, max_fr, max, stat_val, shm_id, j, hex, z, pf_count1, pf_count2, total_pf, fr, ret, w, cur_fr, hits;
	pid_t pid, ch_id;
	FILE *fp1, *fp2;
	char ch, *ref, *shm;
	struct list *l, *hashTable[106];

	if(argv!=5){
		printf("Eprepe na dwsetai 4 orismata\n");		// k, q, max_fr, max
		return 0;
	}
	k= atoi(argc[1]);
	q= atoi(argc[2]);
	max_fr= atoi(argc[3]);
	max= atoi(argc[4]);

	printf("k = %d\tq = %d\tmax_fr =%d\tmax = %d\n", k, q, max_fr, max);

	if((sem_id = semget((key_t) 43345, 4, 0666 | IPC_CREAT))==-1){fprintf(stderr,"Fail in creating semaphores\n");}   // Dimiourgw tous semaforous
	set_semvalue(0,0);						// Aitima apo tin PM1
    set_semvalue(1,0);						// Aitima apo tin PM2
    set_semvalue(2,0);						// Enimerothike i Shared Memory
	set_semvalue(3,1);						// Etoimi i Shared Memory

	shm_id=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shm=(char*) shmat(shm_id, 0, 0);

	ref=malloc(11*sizeof(char));
	for(i=0; i<106; i++){
		hashTable[i]=NULL;
	}

	for(i=0; i<3; i++){							// Dimiourgw tis 3 diergasies
		pid=fork();
		if(pid==0){
			pid=i;
			break;
		}
	}
	
	if(pid==0){									// PM1
		fp1 = fopen("bzip.trace","r");

		j=0;
		while(j<max){
			ch = fgetc(fp1);
			i=0;
			while(ch!='\n'){					// Diabazw to bzip.trace grammi-grammi
				ref[i++]= ch;
				ch = fgetc(fp1);
			}
			down(0);							// Kai tis proothw ston MM
			down(3);
			memcpy(shm, ref, 11*sizeof(char));
			up(2);

			j++;
		}
		return 0;
	}else if(pid==1){							// PM2
		fp2 = fopen("gcc.trace","r");

		j=0;
		while(j<max){
			ch = fgetc(fp2);
			i=0;
			while(ch!='\n'){					// Diabazw to gcc.trace grammi-grammi
				ref[i++]= ch;
				ch = fgetc(fp2);
			}
			down(1);							// Kai tis proothw ston MM
			down(3);
			memcpy(shm, ref, 11*sizeof(char));
			up(2);

			j++;
		}
		return 0;
	}else if(pid==2){							// MM
		total_pf=0;
		pf_count1=0;
		pf_count2=0;
		fr=0;
		w=0;
		hits=0;
		j=0;
		while(j<max/q){
			for(i=0; i<q; i++){					// Pairnw q ap tin PM1
				up(0);
				down(2);
				memcpy(ref, shm, 11*sizeof(char));
				up(3);
//				printf("%s\n", ref);
				hex=0;
				for(z=0; z<5; z++){									// Metatropi tis 16adikis anaforas se dekadiko sustima (den lambanetai upopsin to offset)
					if(ref[z]=='1') {hex+=power(16, 4-z);}
					else if(ref[z]=='2') {hex+=2* power(16, 4-z);}
					else if(ref[z]=='3') {hex+=3* power(16, 4-z);}
					else if(ref[z]=='4') {hex+=4* power(16, 4-z);}
					else if(ref[z]=='5') {hex+=5* power(16, 4-z);}
					else if(ref[z]=='6') {hex+=6* power(16, 4-z);}
					else if(ref[z]=='7') {hex+=7* power(16, 4-z);}
					else if(ref[z]=='8') {hex+=8* power(16, 4-z);}
					else if(ref[z]=='9') {hex+=9* power(16, 4-z);}
					else if(ref[z]=='a') {hex+=10* power(16,4-z);}
					else if(ref[z]=='b') {hex+=11* power(16,4-z);}
					else if(ref[z]=='c') {hex+=12* power(16,4-z);}
					else if(ref[z]=='d') {hex+=13* power(16,4-z);}
					else if(ref[z]=='e') {hex+=14* power(16,4-z);}
					else if(ref[z]=='f') {hex+=15* power(16,4-z);}
				}
				l= search(hex, hashTable, 0);				// Elegxw an i selida einai stin mnimi
				if(l==NULL){								// An oxi exw page fault
					fr++;
					pf_count1++;
					total_pf++;
					if(pf_count1>k){						// An einai to k+1 page fault kanw flush ton miso pinaka
						ret= delete(hashTable, 0, &w);		// KAnw flush mono to kommati tis PM1
						for(z=0; z<53; z++){
							hashTable[z]=NULL;
						}
						insert(hex, ref, hashTable, 0);
						pf_count1=0;
						fr=fr-ret;
					}else if(fr>max_fr){					// An i mnimi itan gemati kanw flush olo ton pinaka
						ret= delete(hashTable, 0, &w);
						ret= delete(hashTable, 1, &w);
						for(z=0; z<106; z++){
							hashTable[z]=NULL;
						}
						insert(hex, ref, hashTable, 0);
						fr=1;
					}else{
						insert(hex, ref, hashTable, 0);
					}
				}else{										// An i selida upirxe auxanete o metritis twn hits
					hits++;
				}
			}
		
			for(i=0; i<q; i++){						// Pairnw q ap tin PM2
				up(1);
				down(2);
				memcpy(ref, shm, 11*sizeof(char));
				up(3);
//				printf("%s\n", ref);
				hex=0;
				for(z=0; z<5; z++){									// Metatropi tis 16adikis anaforas se dekadiko sustima (den lambanetai upopsin to offset)
					if(ref[z]=='1') {hex+=power(16, 4-z);}
					else if(ref[z]=='2') {hex+=2* power(16, 4-z);}
					else if(ref[z]=='3') {hex+=3* power(16, 4-z);}
					else if(ref[z]=='4') {hex+=4* power(16, 4-z);}
					else if(ref[z]=='5') {hex+=5* power(16, 4-z);}
					else if(ref[z]=='6') {hex+=6* power(16, 4-z);}
					else if(ref[z]=='7') {hex+=7* power(16, 4-z);}
					else if(ref[z]=='8') {hex+=8* power(16, 4-z);}
					else if(ref[z]=='9') {hex+=9* power(16, 4-z);}
					else if(ref[z]=='a') {hex+=10* power(16,4-z);}
					else if(ref[z]=='b') {hex+=11* power(16,4-z);}
					else if(ref[z]=='c') {hex+=12* power(16,4-z);}
					else if(ref[z]=='d') {hex+=13* power(16,4-z);}
					else if(ref[z]=='e') {hex+=14* power(16,4-z);}
					else if(ref[z]=='f') {hex+=15* power(16,4-z);}
				}
				l= search(hex, hashTable, 1);
				if(l==NULL){							// Elegxw an i selida einai stin mnimi
					fr++;								// An oxi exw page fault
					pf_count2++;
					total_pf++;
					if(pf_count2>k){					// An einai to k+1 page fault kanw flush ton miso pinaka
						ret= delete(hashTable, 1, &w);	// KAnw flush mono to kommati tis PM2
						for(z=53; z<106; z++){
							hashTable[z]=NULL;
						}
						insert(hex, ref, hashTable, 1);
						pf_count2=0;
						fr=fr-ret;
					}else if(fr>max_fr){				// An i mnimi itan gemati kanw flush olo ton pinaka
							ret= delete(hashTable, 0, &w);
							ret= delete(hashTable, 1, &w);
							for(z=0; z<106; z++){
								hashTable[z]=NULL;
							}
							insert(hex, ref, hashTable, 1);
							fr=1;
					}else{
						insert(hex, ref, hashTable, 1);
					}
				}else{									// An i selida upirxe auxanete o metritis twn hits
					hits++;
				}
			}
			j++;
		}

		cur_fr= delete(hashTable, 0, &w);				// adeiazei ti mnimi gia na termatisei
		ret= delete(hashTable, 1, &w);
		cur_fr+=ret;
		printf("\nStatistics:\nTotal Reads = %d\nTotal Writes = %d\nTotal Page Faults = %d\nHits = %d\nFrames in use = %d\n", total_pf, w, total_pf, hits, cur_fr);
		return 0;
	}else{										// Parent
        while( (ch_id=wait(&stat_val)>0) );		// Perimenei ola ta paidia na termatisoun
		del_semvalue(0);						// Eleutherwnei tis domes kai termatizei
		del_semvalue(1);
		del_semvalue(2);
		del_semvalue(3);
		shmdt(shm);
		free(ref);

		return 0;
	}
}


static int set_semvalue(int sem_num, int start){
	union semun sem_union;
	sem_union.val = start;
	if (semctl(sem_id, sem_num, SETVAL, sem_union) == -1){printf("Failed to initialize semaphore %d\n",sem_num); return(0);}
	return(1);
}

static void del_semvalue(int sem_num){
	union semun sem_union;
//	if (semctl(sem_id, sem_num, IPC_RMID) == -1)
//	fprintf(stderr, "Failed to delete semaphore %d\n",sem_num);
}

static int down(int sem_num){
	struct sembuf sem_b;
	sem_b.sem_num = sem_num;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "down failed\n");
		return(0);
	}
	return(1);
}

static int up(int sem_num){
	struct sembuf sem_b;
	sem_b.sem_num = sem_num;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "up failed\n");
		return(0);
	}
	return(1);
}

int get_semval(int sem_num){
    union semun arg;
    arg.array=malloc(9*sizeof(int));
    semctl(sem_id,0,GETALL,arg);
    return(arg.array[sem_num]);
}

int power(int base, int exp){
	int i, val;
	val=1;
	for(i=0; i<exp; i++){
		val=val*base;
	}
	return val;
}
