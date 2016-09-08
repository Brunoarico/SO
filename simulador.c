#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>



pthread_mutex_t mutex;
float remaining_time = 0;

typedef char* string;

typedef struct _process {
    char name[100];
    float t_begin;
    float dt;
    float remaining;
    float deadline;
    int flag;
    int exist;
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
    head->proc.remaining = -1;
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

process peek (queue Q) {
    process p;
    cell *this;
    if (!is_Empty (Q)){
	this = Q->prox;
	p = this->proc;
	return p;
    }
    else {
	printf ("Olhou o vazio!\n");
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

void mark_exec (queue Q, int val) {
    process p;
    cell *this;
    if (!is_Empty (Q)){
	this = Q->prox;
	p = this->proc;
	this->proc.exist = val;
    }
    else {
	printf ("Nao é possivel marcar vazio!\n");
	exit (1);
    }
}
    
    
queue to_PQueue (process this, queue Q) {
    cell *new;
    cell *ant, *c;
    new = malloc (sizeof (cell));
    c = Q->prox;
    ant = Q;
    new->proc = this;
    while (1) {
        if(this.remaining < c->proc.remaining ||c == Q) {
	    ant->prox = new;
	    new->prox = c;
	    return Q;
	}
	else{
	    ant = c;
	    c=c->prox;
	}
    }
}


int Elements (queue Q) {
    queue aux;
    int i;
    aux = Q->prox;
    for( i = 0; aux != Q; i++, aux = aux->prox){
	printf("%s -> ",aux->proc.name);	       
    }
    printf("\n");
    return i;
}

void *ThreadAdd (void *arg) {
    int time, i = 0;
    process P;
    P = *(process*) arg;
    pthread_mutex_lock(&mutex);

    time = clock();
    printf("Começando %s para durar %f\n", P.name, P.dt);
    while((clock()-time)/CLOCKS_PER_SEC < P.dt){
	remaining_time = P.dt - (clock()-time)/CLOCKS_PER_SEC;
	i = i++ * 3*i + 4;
	//printf("%s %lu\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    }
    printf("Pronto %s em %lu segundos!\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    free(arg);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void FCFS (process *routine, int Nprocs) {
    queue Q;
    int i, execs = 0;
    unsigned int t_start, delta;
    process *P;
    
    t_start = clock ();
    Q = NewQueue ();
    while (execs < Nprocs || !is_Empty (Q)) {
   
	for (i = execs; i < Nprocs; i++) {	    
	    delta = (clock () - t_start) / CLOCKS_PER_SEC;
	    if (routine[i].t_begin <= delta) {
		printf ("Ja chegou o %s! %d segundos\n",routine[i].name, delta);
		execs++;
	        Q = to_Queue (routine[i], Q);
	    }
	}
	
	if (!is_Empty (Q)) {
	    P = malloc ( sizeof (process));
	    *P = Unqueue (Q);
	    pthread_create (&P->tID, NULL, ThreadAdd, P);
	}
    }
}

void *ThreadAdd2 (void *arg) {
    int i = 0;
    process P, *point;
    time_t Tstart, Tstop, pause;
    P = *(process*) arg;
    point = (process*) arg;
    time(&Tstart);
    
    while (P.remaining > 0) {
	
	if (point->flag) {
	    remaining_time = P.remaining = P.dt-difftime(pause,Tstart);
	    i = i - 3 * i++;
	    time(&pause);
	    //printf("thread %s falta %f\n",P.name, P.remaining);
	}
	//printf("%s point flag adr %d\n",P.name, &point->flag);
    }
    time(&Tstop);
    
    printf("Finalizado %s em %f segundos, tempo de parede %f. \n", P.name, difftime(Tstop, Tstart),difftime(pause, Tstart));
    free(arg);
    
}
process *initiate_thread (process p) {
    process *P;
    p.exist = 1;                                               //agora ele existe
    P = malloc (sizeof(process));                               //aloca um novo P para passar para a thread
    *P = p;
    printf("Criando: %s \n", P->name);
    pthread_create (&P->tID, NULL, ThreadAdd2, P);              //cria a thread
    remaining_time = P->remaining;                                     //atualiza o tempo de quem esta em execução
    return P;                                                   //retorna um ponteiro para quem esta executando
}

void pause_thread (process *p) {
    if (p->exist) {
	p->flag = 0;
	printf("Pausou thread %s\n", p->name);
    }
    else
	printf ("Pausou thread inexistente\n");
}

void continue_thread (process *p) {
    if (p->exist){
	p->flag = 1;
	printf("Continuando thread %s\n", p->name);
    }
    else
	printf ("Continuou thread inexistente\n");
}


int is_thread_exist (process *p) {
    return p->exist;
}

void SRT (process *routine, int Nprocs) {
    queue Q, NQ;
    int i, execs = 0, *execflag;
    double delta;
    process *P, *EXE = NULL, aux;
    time_t start, stop;
    time(&start);
    Q = NewQueue ();
    
    while (execs < Nprocs || !is_Empty (Q)) {
	time(&stop);
	for (i = execs; i < Nprocs; i++) {
	    delta = difftime (stop, start);                    // mede o tempo
	    if (routine[i].t_begin <= delta) {                 // se algum processo chegar 
		execs++;
		//printf("Chegou %s em %f s\n", routine[i].name, delta);
		if (routine[i].remaining < remaining_time) {        // se seu tempo for menor que o restante
		    printf("%s passou na frente\n", routine[i].name);
		    if(EXE != NULL){                           // se tiver alguem rodando antes
			printf("Mandando %s para a fila\n", EXE->name);
			pause_thread(EXE);                     // pausa ele
			Q = to_PQueue(*EXE, Q);                // e manda para a fila de volta
			Elements(Q);
		    }
		    EXE = initiate_thread(routine[i]);          // inicia o cara que tem urgencia
		}
		else {                                           //do contrario so manda para a fila
		    printf("mandando %s para a fila sem urgencia\n", routine[i].name); 
		    Q = to_PQueue (routine[i], Q);
		}
	    }
	}
	

	
	if(!is_Empty(Q) && !remaining_time) {                                    // se houver alguem na fila
	    EXE = NULL;
	    Elements(Q);
	    aux = Unqueue(Q);                                 // tira da fila
	    if (is_thread_exist(&aux)) {                      // se a thread ja existir
		printf("Ja existe %s\n", aux.name);
		*EXE = aux;                                   // faz dele o que esta rodando
		continue_thread(&aux);                         // e manda ele continuar
	    }
	    else EXE = initiate_thread(aux);
	}
    }
}

void SRT2 (process *routine, int Nprocs) {
    queue Q, NQ;
    int i, execs = 0, *execflag;
    double delta;
    process *P, *EXE;
    time_t start, stop;
    time(&start);
    Q = NewQueue ();
    NQ = Q->prox;

    while (execs < Nprocs || !is_Empty (Q)) {
	time(&stop);
	for (i = execs; i < Nprocs; i++) {
	    delta = difftime(stop, start);
	    if (routine[i].t_begin <= delta) {
		execs++;
		Q = to_PQueue (routine[i], Q);
		if (Q->prox != NQ) { //se muda a frente da fila
		    Elements(Q);
		    NQ = Q->prox; // atualiza a frente da fila
		    
		    if(peek(Q).exist == 0 ) {
			mark_exec(Q, 1);
			P = malloc ( sizeof (process)); //aloca um espaço para o processo
			*P = peek(Q); //pega o primeiro
			printf("Criando: %s \n", P->name);
			pthread_create (&P->tID, NULL, ThreadAdd2, P); //cria a thread
			remaining_time = P->dt; //atualiza o tempo de duração dele

			if(execs > 1){ // se existe thread anterior
			    printf("pausando o %s\n", EXE->name);
			    EXE->flag = 0; // para ela
			}
			EXE = P;

		    }
		}
	    }
	}
	if (!is_Empty(Q) && remaining_time <= 0) { // se o processo terminiou e ha outros 
	    Elements(Q);
	    if(peek(Q).exist == 0){  // ve se o proximo ja existe
		mark_exec(Q, 1);
		P = malloc ( sizeof (process));
		*P = peek(Q);
		printf("Criando aqui: %s \n", P->name);
		pthread_create (&P->tID, NULL, ThreadAdd2, P);
		remaining_time = P->dt;
		EXE = P;
	    }
	    else {
		printf("%s continua\n", peek(Q).name);
		*P = Unqueue(Q);
		//printf("adr EXE flag %d\n", &EXE->flag);
		EXE = P;
		EXE->flag = 1;
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
    for(i = 0; fscanf (trace, "%f %s %f %f", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++) {
        //printf ("%d %s %d %d\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );
	routine[i].flag = 1;
	routine[i].remaining = routine[i].dt;
	routine[i].exist = 0;
    }
    
    SRT (routine, i);

    pthread_exit (NULL);
    return 0;
}
