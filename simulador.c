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
    
    if (trace == NULL){
        printf ("Problemas na abertura do arquivo\n");
        exit (1);
    }
    for(i = 0; fscanf (trace, "%f %s %f %f", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++) {
        //printf ("%f %s %f %f\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );
        routine[i].flag = 1;
        routine[i].remaining = routine[i].dt;
        routine[i].self = NULL;
        routine[i].id = i;
    }
    switch (atoi (argv[1])) {
        case 1:
            FCFS (routine, i, debug);
            break;
        case 2:
            SRT (routine, i, debug);
            break;
        case 3:
            MultiplasFIlas (routine, i, debug);
            break;
        default:
            printf ("Escalonador invalido.\n");
    }

    pthread_exit (NULL);
    return 0;
}
