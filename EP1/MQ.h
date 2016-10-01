/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#ifndef MQ_H
#define MQ_H

#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include "cronometer.h"
#include "structs.h"
#include "fila.h"
#include "globals.h"
#define NUMBER_OF_QUEUES 4



void MultiplasFIlas (process *routine, int Nprocs);

#endif

    
