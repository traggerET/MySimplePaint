all: Shell1

Shell1: Shell1.o
	g++ Shell1.o -o Shell1 -lsfml-graphics -lsfml-window -lsfml-system -fsanitize=address -g -Wall -Werror

Shell1.o: Shell1.cpp
	g++ -c Shell1.cpp -lsfml-graphics -lsfml-window -lsfml-system -fsanitize=address -g

clean:
	rm -rf *.o Shell1.o
