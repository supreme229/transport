CPP = g++
FLAGS = -std=gnu++17 -Wall -Wextra

transport: Makefile transport.o utils.o
		$(CPP) $(FLAGS) -o transport transport.o utils.o

transport.o:  Makefile transport.cpp
		$(CPP) $(FLAGS) -c transport.cpp -o transport.o

utils.o:  Makefile utils.cpp
		$(CPP) $(FLAGS) -c utils.cpp -o utils.o
clean:
	rm -rf *.o 

distclean:
	rm -rf transport *.o