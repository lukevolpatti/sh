#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define QSH_RL_BUFSIZE 1024
#define QSH_TOK_BUFSIZE 64
#define QSH_TOK_DELIM " \t\r\n\a"

char** qsh_split_line(char* line){
    int bufsize = QSH_TOK_BUFSIZE;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;
    int position = 0;

    if(!tokens){
        fprintf(stderr, "qsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, QSH_TOK_DELIM);
    while(token){
        tokens[position] = token;
        position++;

        if(position >= bufsize){
            bufsize += QSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(line, QSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

char* qsh_read_line(void){
    int bufsize = QSH_RL_BUFSIZE;
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
            bufsize += QSH_RL_BUFSIZE;
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
