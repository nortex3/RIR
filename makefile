cc = gcc
CFLAGS=-Wall -Wextra -O2 

all: servidor cliente 

servidor:
	$(CC) $(CFLAGS) -o servidor  src/servidor.c

cliente:
	$(CC) $(CFLAGS) -o cliente src/cliente.c


clean: 
	  $(RM) servidor cliente