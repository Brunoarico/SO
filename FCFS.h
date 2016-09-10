#ifndef FCFS_H
#define FCFS_H

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



void FCFS (process *routine, int Nprocs, int debug);

#endif
