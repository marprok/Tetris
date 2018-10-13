CPP=g++
FLAGS=-Wall -Wextra -std=c++17
LIBS=-lSDL2
INCLUDE=-I/usr/include/SDL2

main: main.o
	$(CPP) -o main main.o  $(LIBS)
main.o: main.cpp
	$(CPP) -c $(FLAGS) main.cpp $(INCLUDE)
clean:
	rm *.o main
