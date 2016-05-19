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



/* Inicializa Servidor */

void inicializaServidor(){
  
  struct stat st={};
  if(stat(DIR_BACKUP,&st)==-1){
      mkdir(DIR_BACKUP,0700);
      mkdir(DIR_DATA,0700);
      mkdir(DIR_METADATA,0700);
  }

   mkfifo(NOME_PIPE,0666);
}

/* Parser de strings */

char** parser(char* agg,int tamanho) {
    int size = 1;
    int max_size = tamanho+1;
    char** args = (char**)malloc(sizeof(char*) * max_size);
    char* token = strtok(agg," ");


    while (token != NULL) {
        args[size++] = strdup(token);
        if (size == max_size) {
            max_size++;
            args = (char**)realloc(args, sizeof(char*) * max_size);
        }
        token = strtok(NULL," ");
    }

    args[size] = NULL;

    return args;
}

/* Parser de strings */

char* retornaPrimeira(char* arg) {
   int i=0;
   char *pid=(char*)malloc(10*sizeof(char));
   while (arg[i] != ' ') {
      pid[i]=arg[i];
      i++;      
    }
    pid[i]='\0';
    return pid;
}

int contaArgs(char *arg){
  int r=0;
  int i=0;
    while (arg[i] != '\0') {
      if(arg[i]==' ') r++;
      i++;      
    }

    return r;
}

/* Adiciona Sufixo ao final de String */

char* insereSufixo(char* arg){
        int r = strlen(arg)-1;
        arg[r]='.';
        strcat(arg,"gz");
        char *str= strdup(arg);

return str;
}


/* Adiciona Sufixo ao final de String */

char* insereSufixoHash(char* arg){
        int r = strlen(arg);
        arg[r]='.';
        strcat(arg,"gz");
        char *str= strdup(arg);

return str;
}



/* Faz Zip dos ficheiros */

int fazZip(){


    pid_t forkpid;
    int i=0,k,r,j,c;
;
    char *str=malloc(128*sizeof(char));
    char *hash=malloc(128*sizeof(char));

    char buffer[1024];
    
    int fd = open(NOME_HASH,O_RDONLY, 0666);


    if (fd < 0) {
        return -1;
    }



    while(read(fd,buffer+i,1)>0){
                if (buffer[i] == '\n') {
                        char **args = parser(buffer,3);
                        strcat(hash," ");
                        strcat(hash,args[1]);
                        strcat(str," ");
                        strcat(str,args[2]);
                        i=0;
                }
        i++;
    }

    char *aux=(char*)malloc((strlen(str)+1) *sizeof(char));
    aux=strdup(str);
    free(str);
    char **final=parser(aux,2);


     char *aux1=(char*)malloc((strlen(hash)+1) *sizeof(char));
    aux1=strdup(hash);
    free(hash);
    char **final1=parser(aux1,2);




    int total=0;
    for(i=1;final[i]!=NULL;i++) total++;

    char **tmp=(char**)malloc((2+total) *sizeof(char*));
    char **mover=(char**)malloc((1+total) *sizeof(char*));
    char **prefixos=(char**)malloc((1+total) *sizeof(char*));
    char **ficheiroNome=(char**)malloc((total) *sizeof(char*));
    char **ficheiroHash=(char**)malloc((total) *sizeof(char*));

    tmp[0]=strdup("gzip");
    tmp[1]=strdup("-k");
    k=2;
    j=0;

    for(i=1;i<=total;i++){
        r = strlen(final[i])-1;
        tmp[k]=strdup(final[i]);
        tmp[k][r]='\0';
        ficheiroNome[j]=strdup(final[i]);
        j++;
        prefixos[i]=strdup(insereSufixo(final[i]));
        k++;
    }
        tmp[k]=NULL;

    k=1;
    c=0;
    for(i=1;i<=total;i++){
        r = strlen(final1[i])+3;

        mover[k]=strdup(insereSufixoHash(final1[i]));
        
        if(i!=1){
            for(j=0;mover[k][j]!='\0';j++)
                                mover[k][j]=mover[k][j+1];
        }

        
        mover[k][r]='\0';
        ficheiroHash[c]=strdup(mover[k]);

        k++;
        c++;
  }
                                  

     

    forkpid=fork();

   if (forkpid==0){
            if(execvp(tmp[0],tmp)==-1) perror("Erro Exec:");


    }else if(forkpid<0){
                puts("Erro na delegação de tarefa para processo filho");
                return -1;
              }else{
                    int status;
                    waitpid(forkpid,&status,0); 
                    if(WIFEXITED(status)){ 
                      
                        for(i=1;i<=total;i++){

                          forkpid=fork();
                          if (forkpid==0){
                                if(execlp("mv","mv",prefixos[i],mover[i],NULL)==-1){
                                 perror("Erro Exec:");
                                 return -1;
                               }
                          }
                        }

                        for(i=1;i<=total;i++){

                            wait(&status); 
                        }
                      
                        for(i=1;i<=total;i++){
                            forkpid=fork();
                            if (forkpid==0){
                                    if(execlp("mv","mv",mover[i],DIR_DATA,NULL)==-1){
                                           perror("Erro Exec:");
                                           return -1;
                                         }
                              }
                            }
                            for(i=1;i<=total;i++){

                                wait(&status); 
                            }
                            int u=0;
                        for(i=1;i<=total;i++){
                           
                                char *metadata= (char*)malloc(228*sizeof(char));
                                char *data= (char*)malloc(228*sizeof(char));

                            forkpid=fork();
                            if (forkpid==0){

                                    strcat(data,DIR_DATA);
                                    strcat(data,ficheiroHash[u]);
                                    strcat(metadata,DIR_METADATA);
                                  r=strlen(ficheiroNome[u]);
                                  ficheiroNome[u][r-1]='\0';

                                    strcat(metadata,ficheiroNome[u]);


                                     if(execlp("ln", "ln", "-sf", data,metadata,  NULL)==-1)return -1;

                              }
                                     u++; 

                            }
                            for(i=1;i<=total;i++){

                                wait(&status); 
                            }
                            forkpid=fork();
                            if (forkpid==0){
                               if(execlp("rm", "rm", NOME_HASH,NULL)==-1)return -1;
                            }else{
                              int status;
                              wait(&status);
                            }
                            return 1;
                        

         }else return -1;
          }
return -1;
}


int existeFicheiroAux(char *fich)
{
    char *file = strdup(fich);
    if( access( file, F_OK ) != -1 ) return 1; 
    else return 0;
}

int existeFicheiro(char *args)
{
    return existeFicheiroAux(args);
}

/* Calcula shasum do ficheiro */

int calcDigest(char *arg, int tamanho){

    pid_t forkpid;
    
    
        char **args = parser(arg,tamanho);
        args[0]=strdup("shasum");

       int fd = open(NOME_HASH, O_CREAT |O_TRUNC| O_RDWR , 0666);
        if (fd < 0) {
           return -1;
        }

        dup2(fd, 1);
        close(fd);
        int r=0;
        int i;
        for(i=1;i<=tamanho && r==0;i++) if(existeFicheiro(args[i])!=1) r=1;
      
        if(r==0){
        forkpid=fork();

            if (forkpid==0){

                    if(execvp(args[0],args)==-1) {
                                 perror("Erro Exec:");
                                 return -1;
                               }


            }else if(forkpid<0){
                        puts("Erro na delegação de tarefa para processo filho");
                        return -1;
                      }else{
                         
                            int status;
                            waitpid(forkpid,&status,0); 
                            if(WIFEXITED(status)){ 
                               
                             return 1;

                          }else { 
                    return -1 ;
                }
    }
  }   

return -1;

}


/* Faz unzip de ficheiros */

int fazUnzip(char *arg, int tamanho){

 pid_t forkpid;
 char **args = parser(arg,tamanho);
 int i,j,total=0;
 for(i=1;args[i]!=NULL;i++) total++;


 char **tmp=(char**)malloc((3+total) *sizeof(char*));    
  char**buff=(char**)malloc(2+total*sizeof(char*));
  
  tmp[0]=strdup("gunzip");
  tmp[1]=strdup("-k");  


    for(i=1;i<=total;i++){
        int pd[2];
        pipe(pd);
        char *s=(char*)malloc(128*sizeof(char*));
        char *b=(char*)malloc(128*sizeof(char));

        forkpid=fork();

        sprintf(s,"%s",DIR_METADATA);
        strcat(s,args[i]);
 
        if (forkpid==0){

           dup2(pd[1],1);
           close(pd[1]);
           close(pd[0]);

           if(execlp("readlink","readlink",s,NULL)==-1){
             perror("Erro Exec:");
             return -1;
           }
        }else{   
             int status;           
             wait(&status); 
             dup2(pd[0],0);
             close(pd[1]);
             read(pd[0],b,128);
             close(pd[0]);
             if(strlen(b)==0) return -1;
             else{
                  int r = strlen(b);
                  b[r-1]='\0';
                  buff[i]=strdup(b); 
             }           
           }

      }
  
        j=1;
 
        for(i=2;i<=total+1;i++){

            tmp[i] = strdup(buff[j]);
            j++;
        }

        tmp[i]=NULL;
  
        forkpid=fork();
        
        if (forkpid==0){

            if(execvp(tmp[0],tmp)==-1){
                 perror("Erro Exec:"); 
                 return -1;
             }
            }else{
                int status;
                waitpid(forkpid,&status,0); 
                
                if(WIFEXITED(status)){
                    for(j=1;j<=total;j++){

                        forkpid=fork();

                        if (forkpid==0){

                            for(i=1;i<=total;i++){

                                int r = strlen(buff[i]);

                                buff[i][r-3]='\0';
                            }

                            if(execlp("mv","mv",buff[j],args[j],NULL)==-1){
                                 perror("Erro Exec:"); 
                                 return -1;
                             }
                         }else{
                            wait(&status);

                         }
                        }
                    }
                    else return -1;
                }     
    return 1;

}

/* Delega tarefa Backup ou Restore */

int delegaTarefa(char *command, int tamanho){
    int forkpid,status;
    char args[3][128];
    int erro=0;

    if (sscanf(command,"%s %s %[^]\n]",args[0],args[1],args[2])!=3) return -1;
    
    if (strcmp(args[1],"backup")==0){
        forkpid=fork();

        if (forkpid==0){
                 erro=calcDigest(args[2],tamanho-2);  

                 if(erro!=-1){

                    erro=fazZip();
                    if(erro!=-1){
                        kill(atoi(args[0]), SIGUSR1);
                        exit(1);
                    }else{

                       kill(atoi(args[0]), SIGINT);

                     exit(0);

                 }
                }else{
                       kill(atoi(args[0]), SIGINT);
                     exit(0);
                 }
                
          } 
        else if(forkpid<0){
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

                 erro=fazUnzip(args[2],tamanho-2);  

                    
                    if(erro!=-1){
                          kill(atoi(args[0]), SIGUSR2);

                        exit(1);
                    }else{
                         kill(atoi(args[0]), SIGINT);

                     exit(0);

                 }
                
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

/* Recebe pedido de cliente */

void recebePedido() {
    int fd;
    char buff[1024];
    pid_t forkpid;
    fd = open(NOME_PIPE, O_RDONLY);
    if(fd==-1) perror("Erro abertura pipe");
    else{
        
        while ( read(fd, buff, 1024) > 0 ) {    
                
             forkpid=fork();

            if(forkpid==0){

               delegaTarefa(buff,contaArgs(buff));
                 exit(1);
             }

              
         } 
    }
       
    close(fd);
}




int main() {


   inicializaServidor();
   
   while(1)
    recebePedido();
    

    return 0;
}