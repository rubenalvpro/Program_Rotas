############################# Makefile ##########################
CC=gcc
CFLAGS=-Wall -ansi 

######Alterar Dependências e Objetos #####
DEPS = api.h #ficheiros .h
OBJ = main.o api.o #objetos. Um por cada ficheiro .c
##########################################

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

SGO_API: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o
	rm -rf SGO_API
	rm -rf *.myout
	rm -rf *.sgo
