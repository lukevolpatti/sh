#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define QSH_RL_BUFSIZE 1024
#define QSH_TOK_BUFSIZE 64
#define QSH_TOK_DELIM " \t\r\n\a"

/* Builtin shell commands */
int qsh_cd(char** args);
int qsh_help(char** args);
int qsh_exit(char** args);

char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char**) = {
    &qsh_cd,
    &qsh_help,
    &qsh_exit
};

int qsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char*);
}

int qsh_cd(char** args){
    if(args[1] == NULL){
        fprintf(stderr, "qsh: expected argument to \"cd\"\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("qsh");
        }
    }
    return 1;
}

int qsh_help(char** args){
    printf("You are using qsh.\n");
    printf("Type commands and arguments. Builtins:\n");

    /* print all builtin command names */
    for(int i = 0; i < qsh_num_builtins(); i++){
        printf("  %s\n", builtin_str[i]);
    }
    
    printf("Use the \"man\" command for information on other commands.\n");
    return 1;
}

int qsh_exit(char** args){
    return 0;
}

int qsh_launch(char** args){
    pid_t pid = fork();

    if(pid == 0){ /* child process */
        if(execvp(args[0], args) == -1){
            /* perror() will find the error associated with global variable
             * errno, and will write it. */
            perror("qsh");
        }
        exit(EXIT_FAILURE); /* shouldn't get here*/
    }
    else if(pid < 0){ /* forking didn't work */
        perror("qsh");
    }
    else{
        int status;
        pid_t wpid = waitpid(pid, &status, WUNTRACED);
        /* WIFEXITED is true if process terminated normally via exit().
         * WIFSIGNALED is true if process terminated due to receipt of signal.*/
        while(!WIFEXITED(status) && !WIFSIGNALED(status))
            wpid = waitpid(pid, &status, WUNTRACED);
    }
    return 1;
}

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

        token = strtok(NULL, QSH_TOK_DELIM);
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

int qsh_execute(char** args){
    if(args[0] == NULL){ /* empty command */
        return 1;
    }

    for(int i = 0; i < qsh_num_builtins(); i++){
        if(strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    }
    
    return qsh_launch(args);
}

void qsh_loop(){
    int status = 1;

    while(status){
        char* line;
        char** args;
        printf("> ");
        line = qsh_read_line();
        args = qsh_split_line(line);
        status = qsh_execute(args);

        free(line);
        free(args);
    }
}

int main(int argc, char** argv){
    qsh_loop();
    return 0;
}
