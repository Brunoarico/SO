#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>


typedef char* string;

typedef struct _process {
    char name[100];
    float t_begin;              //momento que chega
    float dt;                   // quanto dura
    float remaining;            // quanto resta
    float deadline;             // limite
    float quantum;              // classe de quantum para aquele processo
    int flag;                   // flag de ativo
    int id;                     // id do processo no routine
    struct _process *self;      // ja iniciado
    pthread_t tID;
} process;

typedef struct _cell {
   process proc; 
   struct _cell *prox;
} cell;

typedef cell* queue;

#endif
