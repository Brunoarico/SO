/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#ifndef FILA_H
#define FILA_H

#include <stdlib.h>
#include <stdio.h>
#include "structs.h"

queue NewQueue ();

queue *NewMultipleQueues (int size);

int is_Empty(queue Q);

process Unqueue (queue Q);

process peek (queue Q);

queue to_Queue (process this, queue Q);

queue to_PQueue (process this, queue Q);




#endif
