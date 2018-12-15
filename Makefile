CPP=g++
FLAGS=-Wall -Wextra -std=c++17 -g
LIBS=-lSDL2
INCLUDE=-I/usr/include/SDL2

main: main.o
	$(CPP) -o main main.o  $(LIBS)
main.o: main.cpp tetrominos.h
	$(CPP) -c $(FLAGS) main.cpp tetrominos.h $(INCLUDE)
clean:
	rm *.o main
