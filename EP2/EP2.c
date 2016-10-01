#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>  
#include <semaphore.h> 

typedef struct timespec tempo;

typedef struct _competidor{
    pthread_t id;
    int velocidade;
    int distancia;
    int voltas;
} competidor;

typedef struct _equipe {
    int n_da_equipe;
    int n_de_integrantes;
    int onde_comeco;
    competidor *integrantes;
} equipe;


typedef struct _trecho {    
    sem_t mutex;
    pthread_t tID[2];
    int pos;  //guarda uma posição livre que tem no trecho (vetor tID)
} trecho;

equipe *Team_Iron, *Team_American;
trecho *pista;
pthread_t *ciclistas;
int tam_pista;
int n;
char option;

/*calcula diferenças de tempo*/
float diff (tempo end, tempo init) {
    return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - 
	((double)init.tv_sec + 1.0e-9*init.tv_nsec);
}
/* faz delay de val segundos*/
void wait(float val){
    tempo start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do{
	    
	clock_gettime(CLOCK_MONOTONIC, &stop);
	
    }while(diff (stop, start) < val);
}

/*marca que uma thread chegou na posição*/
void chega (pthread_t I, int posicao) {
    pista[posicao].tID[pista[posicao].pos] = I;
    pista[posicao].pos = (pista[posicao].pos + 1) % 2;
}

/*marca que uma thread saiu da posição*/
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

/*pega o indice relativo a aquela thread no vetor integrantes*/
int meu_indice(equipe E, pthread_t I) {
    int i;
    for (i = 0; i < n; i++)
	if (pthread_equal (E.integrantes[i].id, I))
	    return i;
    
    printf ("Não estou nessa equipe\n");
    exit (1);	    
}

/*atualiza o status da thread no vetor integrantes*/
void atualiza_meu_status (equipe E, int indice, int volta, int m_percorridos) {
    E.integrantes[indice].voltas = volta;
    E.integrantes[indice].distancia = m_percorridos;
}

/*sorteia se a thread quebra*/
int quebra (equipe E, int indice, int m_percorridos) {
    if (rand () % 10 == 1 && E.n_de_integrantes > 3 && !(m_percorridos % (4 * tam_pista))) {
	pthread_cancel (E.integrantes[indice].id);
	E.n_de_integrantes--;
	return 1;
	}
    return 0;
}

/*faz os eventos aleatorios os eventos*/
void evento (char option, equipe E, int indice, int m_percorridos){
    if (quebra (E, indice, m_percorridos)) {
	printf("quebrou ciclista %d, na volta %d\n", E.integrantes[indice].id % 100, m_percorridos/tam_pista);
    }
    if(option == 'v') {
	if (rand() % 2) E.integrantes[indice].velocidade = 60;
	else E.integrantes[indice].velocidade = 30;
    }
}

/*codigo da thread*/
void *ciclista (void *arg) {
    int posicao, volta = 0, indice, m_percorridos = 0;
    equipe eqp;
    int i;
    pthread_t I;
    eqp = *(equipe*) arg;
    
    I = pthread_self();              //pega seu proprio tID
    posicao = eqp.onde_comeco;
    indice = meu_indice(eqp, I);
    if(option == 'u')
	eqp.integrantes[indice].velocidade = 60;
    else if(option == 'v')
	eqp.integrantes[indice].velocidade = 30;
    
    while (volta < 16){
	
        sem_wait(&pista[posicao].mutex);
	chega (I, posicao);                     //marca que chegou
	//printf ("Thread %d nome %d posicao %d meu indice %d, volta %d\n", eqp.n_da_equipe, I%100, posicao, indice, volta);
	for (i = 0; i < tam_pista; i++) { // printa a pista
	    if (i != posicao)
		printf("-");
	    else
		printf(" %d ", I%10);
	}
	printf("\n");
	sai(I, posicao); // marca que saiu
	wait(0.6 * 60/eqp.integrantes[indice].velocidade);       //faz o delay
	sem_post(&pista[posicao].mutex);
	
	atualiza_meu_status(eqp, indice, volta, m_percorridos); // atualiza no vetor integrantes
	
	if (m_percorridos && !(m_percorridos % tam_pista)) {               // ve se completou uma volta
	    volta++;
	    evento(option, eqp, indice, m_percorridos);                      // dispara eventos
	}
	
	m_percorridos++;
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
    
    Team_Iron = malloc (sizeof (equipe));
    Team_American = malloc (sizeof (equipe));
    Team_Iron->n_da_equipe = 1;
    Team_American->n_da_equipe = 2;
    Team_Iron->n_de_integrantes = n;
    Team_American->n_de_integrantes = n;
    Team_Iron->onde_comeco = 0;
    Team_American->onde_comeco = tam_pista/2;
    Team_Iron->integrantes = malloc (n * sizeof(competidor));
    Team_American->integrantes = malloc (n * sizeof(competidor));
    
    ciclistas = malloc (2 * n * sizeof (pthread_t));
    
    for (i = 0; i < n; i = i+1) {
	pthread_create (&ciclistas[i], NULL, ciclista, (void*) Team_Iron);
	Team_Iron->integrantes[i].id = ciclistas[i];
        pthread_create (&ciclistas[i+1], NULL, ciclista, (void*) Team_American);
	Team_American->integrantes[i].id = ciclistas[i+1];
    }
}

void finaliza_corredores () { //verificar corredores ja mortos
    free(ciclistas);
    free(Team_Iron->integrantes);
    free(Team_American->integrantes);
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
    option = argv[3][0];
    printf("Tamanho da pista %d numero de corredores %d\n", tam_pista, n);
    inicia_pista ();

    inicia_corredores ();
    for (i = 0; i< n; i++)
	pthread_join(ciclistas[i], NULL);
    printf("Ok");
    return 0;
    
}
    
