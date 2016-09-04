#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define MAXPROC 100000
typedef char* string;

typedef struct _process {
    char name[100];
    int t_begin;
    int dt;
    int deadline;
    pthread_t tID;
} process;

typedef struct _cell {
   process proc; 
   struct _cell *prox;
} cell;

typedef cell* queue;

queue NewQueue () {
    cell *head;
    head = malloc (sizeof (cell));
    head->prox = head;
    head->proc.t_begin = -1;
    return head;
}

int is_Empty(queue Q) {
    if (Q->prox == Q)
	return 1;
    else
	return 0;
}

process Unqueue (queue Q) {
    process p;
    cell *this;
    if (!is_Empty (Q)){
	this = Q->prox;
	p = this->proc;
	Q->prox = this->prox;
	free (this);
	return p;
    }
    else {
	printf ("Desempilhou do vazio!\n");
	exit (1);
    }
}

queue to_Queue (process this, queue Q) {
    cell *new;
    new = malloc (sizeof (cell));
    new->prox = Q->prox;
    Q->prox = new;
    Q->proc = this;
    return new;
}

int Elements (queue Q) {
    queue aux;
    int i;
    aux = Q->prox;
    for( i = 0; aux != Q; i++, aux = aux->prox){
	//printf("%s -> ",aux->proc.name);	       
    }
    printf("\n");
    return i;
}

void * ThreadAdd (void * a) {
   int i, tmp;
   for (i = 0; i < 1000000000; i++) {
       tmp = 2*tmp-1;
   }
   printf("Pronto!\n");
   return NULL;
}

void FCFS (process *routine, int Nprocs) {
    queue Q;
    int i, execs = 0;
    unsigned int t_start, delta;
    
    t_start = clock();
    Q = NewQueue();
    printf("i=%d\n", Nprocs);
    
    while(execs < Nprocs){
	for (i = execs; i < Nprocs; i++){
	    delta = (clock()-t_start)/CLOCKS_PER_SEC;
	    if (routine[i].t_begin <= delta){
		printf("Ja chegou o %s! %d segundos\n",routine[i].name, delta);
		pthread_create(&routine[i].tID, NULL, ThreadAdd, NULL);
		execs++;
	        Q = to_Queue(routine[i], Q);
		printf("%d\n", Elements(Q));;
	    }
	}
    }
}


int main() {
    int i, j;
    FILE *trace;
    process routine[1000];
    unsigned long int time;
    
    trace = fopen ("trace.txt", "r");
    
    if (trace == NULL){
	printf ("Problemas na abertura do arquivo\n");
	exit (1);
    }
    for(i = 0; fscanf (trace, "%d %s %d %d", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++)
        printf("%d %s %d %d\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );


    FCFS(routine, i);
    pthread_join(routine[0].tID, NULL);
    pthread_join(routine[1].tID, NULL);
    pthread_join(routine[2].tID, NULL);
    pthread_join(routine[3].tID, NULL);
    pthread_exit(NULL);
    return 0;
}
