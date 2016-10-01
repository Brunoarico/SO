#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>  
#include <semaphore.h> 

typedef struct timespec tempo;

typedef struct _briefing {
    int onde_estou;
    int equipe;
} briefing;

typedef struct _trecho {    
    sem_t mutex;
    pthread_t tID[2];
    int pos;  //guarda uma posição livre que tem no trecho (vetor tID)
} trecho;

briefing *Team_Iron, *Team_American;
trecho *pista;
pthread_t *ciclistas;
int tam_pista;
int n;

float diff (tempo end, tempo init) {
    return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - 
	((double)init.tv_sec + 1.0e-9*init.tv_nsec);
}

void wait(float val){
    tempo start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do{
	    
	clock_gettime(CLOCK_MONOTONIC, &stop);
	
    }while(diff (stop, start) < val);
}

void chega (pthread_t I, int posicao) {
    pista[posicao].tID[pista[posicao].pos] = I;
    pista[posicao].pos = (pista[posicao].pos + 1) % 2;
}

void sai (pthread_t I, int posicao) {
    if(pthread_equal(pista[posicao].tID[0], I)){
	pista[posicao].tID[0] = 0;
	pista[posicao].pos = 0;
    }
    else{
	pista[posicao].tID[1] = 0;	    
	pista[posicao].pos = 1;
    }
}

    

void *ciclista (void *arg) {
    //estado indica: 1 - funcionando, 0 - quebrado
    int posicao, velocidade, estado = 1, equipe, volta = 0, ocupando;
    briefing B;
    B = *(briefing*) arg;
    int i;
    pthread_t I;
    I = pthread_self();
    posicao = B.onde_estou, equipe = B.equipe;
    velocidade = 60;
        
    while(1){
	
        sem_wait(&pista[posicao].mutex);
	chega(I, posicao);
	printf("Thread %d nome %d posicao %d\n", B.equipe, I%100, posicao);
	for(i = 0; i < tam_pista; i++){
	    if (i != posicao)
		printf("-");
	    else
		printf("X");
	}
	printf("\n");
	sai(I, posicao);
	wait(0.6);
	sem_post(&pista[posicao].mutex);
	
        posicao = (posicao+1)%tam_pista;
    }
    
    return NULL;
}

void inicia_pista () {
    int i;
    pista = malloc (tam_pista * sizeof (trecho));
    for (i = 0; i < tam_pista; i++){
	sem_init(&pista[i].mutex, 0, 2);
	pista[i].pos = 0;
    }
}

void inicia_corredores () {
    int i;
    Team_Iron = malloc (sizeof (briefing));
    Team_American = malloc (sizeof (briefing));
    ciclistas = malloc (2 * n * sizeof (pthread_t));
    
    Team_Iron->onde_estou = 0;
 
    Team_Iron->equipe = 1;

    Team_American->onde_estou = tam_pista/2;
  
    Team_American->equipe = 2;
    
    for (i = 0; i < 2*n; i = i+2) {
	pthread_create (&ciclistas[i], NULL, ciclista, (void*) Team_Iron);
        pthread_create (&ciclistas[i+1], NULL, ciclista, (void*) Team_American);
    }
}

void finaliza_corredores () { //verificar corredores ja mortos
    int i;
    for (i = 0; i < n; i++)
        pthread_cancel (ciclistas[i]);
    free(ciclistas);
    free(Team_Iron);
    free(Team_American);
}

void destroiPista () {
    int i;
    for (i = 0; i < tam_pista; i++)
	sem_destroy(&pista[i].mutex);
    free(pista);
}



int main (int argc, char **argv) {
    int i;
    tam_pista = atoi(argv[2]);
    n = atoi(argv[1]);
    printf("Tamanho da pista %d numero de corredores %d\n", tam_pista, n);
    inicia_pista ();

    inicia_corredores ();
    for (i = 0; i< n; i++)
	pthread_join(ciclistas[i], NULL);
}
    
