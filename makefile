FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster
clean:
	rm -rf *.o cluster

cluster: main.o spmat.o Algorithm.o linkedList.o utilities.o
	 gcc  main.o spmat.o Algorithm.o linkedList.o utilities.o -o cluster $(LIBS)

main.o: main.c spmat.h Algorithm.h linkedList.h utilities.h
	 gcc $(FLAGS) -c main.c

spmat.o: spmat.c spmat.h utilities.h
	gcc $(FLAGS) -c spmat.c
	
Algorithm.o: Algorithm.c spmat.h Algorithm.h linkedList.h utilities.h

linkedList.o: linkedList.c linkedList.h utilities.h

utilities.o: utilities.c spmat.h linkedList.h utilities.h