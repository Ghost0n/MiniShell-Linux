all: commandes.o mishell.o
	gcc -o mishell mishell.o commandes.o

mishell.o: mishell.c
	gcc -c -Wall mishell.c
	
commandes.o: commandes.c
	gcc -c -Wall commandes.c

clean:
	rm *.o
