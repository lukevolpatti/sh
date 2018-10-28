#include <stdio.h>
#include <stdlib.h>
#define LSH_RL_BUFSIZE 1024


char* qsh_read_line(void){
    int bufsize = LSH_RL_BUFSIZE;
    char* buffer = malloc(sizeof(char) * bufsize);
    int position = 0;

    if(!buffer){
        fprintf(stderr, "qsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        /* Get one character */
        int c = getchar(); /* Needs to be int because EOF is int */

        /* bug: typing ctrl-D causes infinite loop*/
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

        if(position >= bufsize){
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer){
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void qsh_loop(){
    int status = 1;

    while(status){
        char* line;
        char** args;
        printf("> ");
        line = qsh_read_line();
        //args = qsh_split_line(line);
        //status = qsh_execute(args);

        free(line);
        //free(args);
    }
}

int main(int argc, char** argv){
    qsh_loop();
    return 0;
}
