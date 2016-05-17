#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "servidor.h"

/**Escreve no pipe*/
static void escreve_pipe(char* str) {
	int fd = open(NOME_PIPE, O_WRONLY);
	write(fd, str, strlen(str) + 1);
	close(fd);
}

/* Mensagem de comando errado */

void arg_errados() {
	printf("Comando nao existe\n");
}


/* Mensagem backup terminado */
void copiado(){
	printf("Ficheiros Copiados\n");
}

/* Mensagem restore terminado */

void recuperado(){
	printf("Ficheiros Recuperados\n");
}

/* Mensagem erro no backup */
void erro_backup(){
	printf("Ocorreu algum erro \n");
}

int main(int argc, char const *argv[])
{
	

	int t ;
	char* str;
	char buffer[1024];

	sprintf(buffer, "%d ", getpid());

	signal(SIGUSR1,copiado);
	signal(SIGUSR2,recuperado);
	signal(SIGQUIT,arg_errados);
	signal(SIGINT,erro_backup);


	for(t=1;t<argc;t++){

		strcat(buffer,argv[t]);
		strcat(buffer," ");
	}
	
		escreve_pipe(buffer);
		pause();
	

	return 0;
}




