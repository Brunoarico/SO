/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#ifndef FCFS_H
#define FCFS_H

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



void FCFS (process *routine, int Nprocs);

#endif
