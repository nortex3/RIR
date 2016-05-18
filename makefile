all: servidor cliente

servidor:
	gcc -Wall -o servidor servidor.c

cliente:
	gcc -Wall -o cliente cliente.c

clean: 
	  $(RM) servidor cliente