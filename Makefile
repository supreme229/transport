CPP = g++
FLAGS = -std=gnu++17 -Wall -Wextra

transport: Makefile transport.o
		$(CPP) $(FLAGS) -o transport transport.o

transport.o:  Makefile transport.cpp
		$(CPP) $(FLAGS) -c transport.cpp -o transport.o

clean:
	rm -rf *.o 

distclean:
	rm -rf transport *.o