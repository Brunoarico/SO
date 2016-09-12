/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#include "fila.h"

queue NewQueue () {
    cell *head;
    head = malloc (sizeof (cell));
    head->prox = head;
    head->proc.t_begin = -1;
    head->proc.remaining = -1;
    return head;
}

queue *NewMultipleQueues (int size) {
    queue *Qs;
    int i;
    Qs = malloc (size * sizeof (queue));
    /*printf("mallocquei o Qs em %d\n", size);*/
    for (i = 0; i < size; i++) {
        Qs[i] = NewQueue();
    }
    return Qs;
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
    } else {
        printf ("Desempilhou do vazio!\n");
        exit (1);
    }
}

process peek (queue Q) {
    process p;
    cell *this;
    if (!is_Empty (Q)) {
        this = Q->prox;
        p = this->proc;
        return p;
    } else {
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
    
    
queue to_PQueue (process this, queue Q) {
    cell *new;
    cell *ant, *c;
    new = malloc (sizeof (cell));
    c = Q->prox;
    ant = Q;
    new->proc = this;
    while (1) {
        if(this.remaining < c->proc.remaining || c == Q) {
            ant->prox = new;
            new->prox = c;
            return Q;
        } else {
            ant = c;
            c=c->prox;
        }
    }
}

