#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

typedef char* string;

typedef struct _process {
    char name[100];
    int t_begin;
    int dt;
    int deadline;
} process;

int main() {
    int i;
    FILE *trace;
    process routine[100];
    trace = fopen ("trace.txt", "r");
    
    if (trace == NULL){
	printf ("Problemas na abertura do arquivo\n");
	exit (1);
    }
    for(i = 1; fscanf (trace, "%d %s %d %d", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++)
        printf("%d %s %d %d\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );
    
    return 0;
}
