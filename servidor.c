
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "servidor.h"


void inicializaServidor(){
   mkfifo(NOME_PIPE,0666);
}


char** parser(char* agg,int tamanho) {
    int size = 1;
    int max_size = tamanho+1;
    char** args = (char**)malloc(sizeof(char*) * max_size);
    char* token = strtok(agg, " ");


    while (token != NULL) {
       // printf("%s\n", token);
        args[size++] = strdup(token);
        if (size == max_size) {
            max_size++;
            args = (char**)realloc(args, sizeof(char*) * max_size);
        }
        token = strtok(NULL, " ");
    }

    args[size] = NULL;

    return args;
}

int fazZip(char *arg, int tamanho,char* p){
    return 0;
}

int calcDigest(char *arg, int tamanho,char* p){

    pid_t forkpid;
    int i;
    char ** args = parser(arg,tamanho);
        forkpid=fork();
        args[0]=strdup("shasum");
    if (forkpid==0){

            if(execvp(args[0],args)==-1) perror("Erro Exec:");

    }else if(forkpid<0){
                puts("Erro na delegação de tarefa para processo filho");
                return -1;
              }else{
                    puts("Tarefa delegada para processo filho");
                    int status;
                    waitpid(forkpid,&status,0); //Espera que o filho termine
                    if(WIFEXITED(status)){ // Se o filho terminou normalmente, entao...

                        return 1;

                  }else { //Senão retorna -1
            return -1 ;
        }
                }

    printf("Chegeu\n");
}



int trataBackup(char *arg, int tamanho,char* p){
    
    int i;
    char ** args = parser(arg,tamanho);


    if(execvp("gzip",args)==-1){
        perror("Erro Exec: ");
        exit(-1);

    }
return -1;
   
}


int delegaTarefa(char *command, int tamanho){
    int forkpid,status;
    char args[3][128];
    int erro=0;
    if (sscanf(command,"%s %s %[^]\n]",args[0],args[1],args[2])!=3) return -1;

    if (strcmp(args[1],"backup")==0){

        forkpid=fork();

        if (forkpid==0){

             erro=calcDigest(args[2],tamanho-2,args[0]);  

             if(erro!=-1){
               // erro=trataBackup(args[2],tamanho-2,args[0]);
                //ENvia sinal ao cliente que backup feito;
              
                    kill(atoi(args[0]), SIGINT);
                    exit(1);
            }else{

                //ENvia sinal ao cliente que ocorreu um erro;
                kill(atoi(args[0]), SIGUSR1);
                 exit(0);
            }
        }else if(forkpid<0){
                puts("Erro na delegação de tarefa para processo filho");
                return -1;
              }else{
                    puts("Tarefa delegada para processo filho");
                    wait(&status);
                    return 1;
                }
    }

    else if(strcmp(args[1],"restore")==0){

        if ((forkpid=fork())==0){

            _exit(0);
        }
        else if(forkpid<0){
            puts("Erro na delegação de tarefa para processo filho");
            return -1;
        }
        else{
            puts("Tarefa delegada para processo filho");
            wait(&status);
        }
    }else  kill(atoi(args[0]), SIGQUIT);

return 0;

}

void recebePedido() {
	int fd;
	char buff[1024];
    int i = 0;
    int tamanho;
    int args=1;
	fd = open(NOME_PIPE, O_RDONLY);

    if(fd==-1) perror("Erro abertura pipe");
    else
        while ( (tamanho=read(fd, buff + i, 1) > 0 )) {             
            if(buff[i]==' '){
                args++;
            }
            if (buff[i] == '\n') {
                delegaTarefa(buff,args);
                i = 0;
            }
            else i++;
         }
    close(fd);
}



int main() {


   inicializaServidor();
   while(1) recebePedido();

	return 0;
}







