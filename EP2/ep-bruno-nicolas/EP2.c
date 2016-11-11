/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/

#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>  
#include <semaphore.h>
#define Tsimulado 0.01

typedef struct timespec tempo;

typedef struct _competidor{
    pthread_t id;
    int velocidade;
    int distancia;
    int voltas;
    double instante;
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
pthread_mutex_t lock;
pthread_barrier_t barrier;
int flaglock;
int tam_pista;
int n;
int vencedor;
int acabou;
int debug;
char option;
tempo comeca;

/*calcula diferenças de tempo*/
float diff (tempo end, tempo init) {
    return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - 
    ((double)init.tv_sec + 1.0e-9*init.tv_nsec);
}
/* faz delay de val segundos*/
void wait(float val){
    tempo start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        
    clock_gettime(CLOCK_MONOTONIC, &stop);
    
    } while(diff (stop, start) < val);
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
/* verifica ultrapassagem de um terceiro de integrante de uma equipe por outra 
    retorna -1 se não houve ultrapassagem, 1 se o 3º integrante da equipe Iron
    ultrapassou o 3º da equipe American e retorna 2 caso  contrário ocorrer */
int verifica_ultrapassagem() {
    int t = n - 3;

    if (Team_Iron->onde_comeco + Team_Iron->integrantes[t].distancia > Team_American->onde_comeco + Team_American->integrantes[t].distancia)
        return 1;
    if (Team_American->onde_comeco + Team_American->integrantes[t].distancia - tam_pista > Team_Iron->onde_comeco + Team_Iron->integrantes[t].distancia)
        return 2;
    return -1;
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

/* Ordena o vetor de integrantes de a cordo com a distancia, 
que estara em primeiro fica no fim do vetor (Ordem crescente)*/
void ordena_integrantes (int n, competidor *v) {
    int i, j;
    competidor x;

    for (j = 1; j < n; ++j) {
        x = v[j];
        for (i = j-1; i >= 0 && v[i].distancia > x.distancia; --i) 
            v[i+1] = v[i];
        v[i+1] = x;
    }
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
void evento (char option, equipe E, int indice, int m_percorridos, double instante){
    int i;
    int flag = 0;
    if (quebra (E, indice, m_percorridos)) {
        printf("quebrou ciclista %d, na volta %d\n", (int) E.integrantes[indice].id, m_percorridos/tam_pista);
    }
    if (indice == n - 3) { //relatorio de saida
        printf("Ciclista %d, Ciclista %d, Ciclista %d\nVolta %d\nInstante: %f\n", (int) E.integrantes[n - 1].id, (int) E.integrantes[n - 2].id, (int) E.integrantes[n - 3].id, m_percorridos/tam_pista, instante);
    }
  
    if(option == 'v') {
        if (rand() % 2){
            for (i = indice+1; i < E.n_de_integrantes; i++) 
                if (30 == E.integrantes[i].velocidade) // Verifica se tem alguem com velocidade 30 na frente
                    flag++;

            if(flag) E.integrantes[indice].velocidade = 30; // Se tiver fica com 30
            else E.integrantes[indice].velocidade = 60;
        }
    
        else
            E.integrantes[indice].velocidade = 30;
    }
}
void *temporizador (void *arg) {
    return NULL;
}

/*codigo da thread*/
void *ciclista (void *arg) {
    int posicao, volta = 0, indice, m_percorridos = 0;
    equipe eqp;
    pthread_t I;
    tempo cheguei, termineivolta;
    eqp = *(equipe*) arg;
    
    I = pthread_self();              //pega seu proprio tID
    posicao = eqp.onde_comeco;
    indice = meu_indice(eqp, I);
    if(option == 'u')
        eqp.integrantes[indice].velocidade = 60;
    else if(option == 'v')
        eqp.integrantes[indice].velocidade = 30;
    
    while (volta < 16) {
        sem_wait(&pista[posicao].mutex);
        chega (I, posicao);                     //marca que chegou
        sai(I, posicao); // marca que saiu
        wait(0.6 * 60/eqp.integrantes[indice].velocidade *Tsimulado);       //faz o delay
        
        
        atualiza_meu_status(eqp, indice, volta, m_percorridos); // atualiza no vetor integrantes

        ordena_integrantes(eqp.n_de_integrantes, eqp.integrantes);
        clock_gettime (CLOCK_MONOTONIC, &termineivolta);
        if (m_percorridos && !(m_percorridos % tam_pista)) { // ve se completou uma volta
            volta++;
            evento (option, eqp, indice, m_percorridos, diff(termineivolta, comeca));  // dispara eventos
        }
        
        sem_post(&pista[posicao].mutex);
        
        
        m_percorridos++;
        posicao = (posicao+1) % tam_pista; // avança 1 no vetor circular da pista


        flaglock = 1;

        // barreira de sincronizacao
        pthread_barrier_wait(&barrier);
        ordena_integrantes(eqp.n_de_integrantes, eqp.integrantes);

        pthread_mutex_lock(&lock);
        if (flaglock) {      
            if ((vencedor = verifica_ultrapassagem()) != -1) {
                printf("Vencedor! %d\n", vencedor);
                break;
            }
            flaglock = 0;
        }
        pthread_mutex_unlock(&lock);

    
    }

    flaglock = 1;
    
    clock_gettime (CLOCK_MONOTONIC, &cheguei);
    printf ("Thread %d chegou no fim da corrida em %f\n s", (int) I, diff(cheguei, comeca));
    return NULL;
}

void inicia_pista () {
    int i;
    pista = malloc (tam_pista * sizeof (trecho));
    for (i = 0; i < tam_pista; i++){
        sem_init(&pista[i].mutex, 0, 2);
        pista[i].pos = 0;
    }
    pthread_mutex_init(&lock, NULL);
}

void inicia_corredores () {
    int i;
    
    Team_Iron = malloc (sizeof (equipe));
    Team_American = malloc (sizeof (equipe));
    Team_Iron->n_da_equipe = 1;
    Team_American->n_da_equipe = 2;
    Team_Iron->n_de_integrantes = n;
    Team_American->n_de_integrantes = n;    Team_Iron->onde_comeco = 0;
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

void destroi_pista () {
    int i;
    for (i = 0; i < tam_pista; i++)
        sem_destroy(&pista[i].mutex);
    free(pista);
}

int main (int argc, char **argv) {
    int i;
    tam_pista = atoi(argv[1]);
    n = atoi(argv[2]);
    option = argv[3][0];
    printf("Tamanho da pista %d numero de corredores %d\n", tam_pista, n);
    inicia_pista ();
    pthread_barrier_init(&barrier, NULL, n);
    clock_gettime(CLOCK_MONOTONIC, &comeca);
    inicia_corredores ();
    for (i = 0; i< n; i++)
        pthread_join(ciclistas[i], NULL);

    finaliza_corredores();
    destroi_pista();
    return 0;
    
}
    
