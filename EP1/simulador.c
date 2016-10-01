/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "SRT.h"
#include "FCFS.h"
#include "MQ.h"
#define MAXROUTINE 10000


int main (int argc, char **argv) {
    int i;
    FILE *trace;
    process routine[MAXROUTINE];

    debug = 0;

    if (argc > 4 && strcmp(argv[4], "d") == 0)
        debug = 1;

    trace = fopen (argv[2], "r");
    saida = fopen (argv[3], "w");
    debugfile = fopen (argv[5], "w");
    
    if (trace == NULL){
        printf ("Problemas na abertura do arquivo\n");
        exit (1);
    }
    for(i = 0; fscanf (trace, "%f %s %f %f", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++) {
       
        routine[i].flag = 1;
        routine[i].remaining = routine[i].dt;
        routine[i].self = NULL;
        routine[i].id = i;
    }
    switch (atoi (argv[1])) {
        case 1:
            FCFS (routine, i);
            break;
        case 2:
            SRT (routine, i);
            break;
        case 3:
            MultiplasFIlas (routine, i);
            break;
        default:
            printf ("Escalonador invalido.\n");
    }
    fclose (trace);
    fclose (saida);
    fclose (debugfile);
    pthread_exit (NULL);
    return 0;
}
