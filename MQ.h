#ifndef MQ_H
#define MQ_H

#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "fila.h"
#include "globals.h"
#define NUMBER_OF_QUEUES 4



void MultiplasFIlas (process *routine, int Nprocs);

#endif

    
