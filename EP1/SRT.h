/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#ifndef SRT_H
#define SRT_H

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


void SRT (process *routine, int Nprocs);

#endif
