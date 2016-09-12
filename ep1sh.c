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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef char *string;

char cwd[1024];

void showdir(string dir) {
    strcat(dir, "(");
    strcat(dir, getcwd(cwd, sizeof(cwd)));
    strcat(dir, ") : ");
}

void tokenizer (string cmd, char *argv[]){
    int i;
    string token;
    	token = strtok(cmd, " ");	
	for(i = 0; token != NULL; i++) {
	    argv[i] = token;
	    token = strtok (NULL, " ");
	}
	argv[i] = NULL;
}

void chmod (string archive, string code) {
    syscall (SYS_chmod, archive, atoi (code));
}

int id (string argument) {
   return syscall(SYS_getuid, argument);
}


int main() {
    string cmd;
    char dir [1024];
    char *argv[30];
    int pid;
    
    while(1) {
	
	strcpy(dir, "");
	showdir( dir );
	cmd = readline( dir );
	add_history(cmd);
	
	tokenizer (cmd, argv);

	pid = fork ();

	if (pid == 0) {
	    
	    if (!strcmp ("chmod", argv[0])) {
		printf ("%s = chmod\n", argv[0]);
		chmod (argv[1], argv[2]);
	    }
	    
	    else if ( !strcmp("id", argv[0])) {
		printf ("%s = id\n", argv[0]);
	        id (argv[1]);
	    }
	    
	    else {
		printf ("execve\n");
		execvp (argv[0], argv);
	    }
	        
	}
	
	else {                    //Parent
	    wait(NULL);
	    //printf("Child exited\n");
	}
    }
    
    return 0;

}
