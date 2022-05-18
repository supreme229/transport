CPP = g++
FLAGS = -std=gnu++17 -Wall -Wextra

transport: Makefile main.o utils.o transport.o
		$(CPP) $(FLAGS) -o transport main.o utils.o transport.o

main.o:  Makefile main.cpp
		$(CPP) $(FLAGS) -c main.cpp -o main.o

utils.o:  Makefile utils.cpp
		$(CPP) $(FLAGS) -c utils.cpp -o utils.o

transport.o:  Makefile transport.cpp
		$(CPP) $(FLAGS) -c transport.cpp -o transport.o
		
clean:
	rm -rf *.o 

distclean:
	rm -rf transport *.o