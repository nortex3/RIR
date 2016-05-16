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
	printf("Nome Ficheiro : copiado\n");
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
	char pid[10];

	sprintf(pid, "%d", getpid());

	signal(SIGINT,copiado);
	signal(SIGQUIT,arg_errados);
	signal(SIGUSR1,erro_backup);

	if(read(0,buffer,1024)){
		str=(char*)calloc(strlen(buffer)+ strlen(pid),sizeof(char));
		sprintf(str, "%s %s", pid ,buffer);
		escreve_pipe(str);
		pause();
		free(str);
	}

	return 0;
}




