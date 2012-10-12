

all: clean
	g++ -Wall -Wunused -Wextra -Werror -g -o main *.cpp -lpthread -lSDL -lrt

test:
	valgrind --tool=memcheck --leak-check=full  --show-reachable=yes --track-origins=yes ./main

clean:
	rm -f main *.o *~ core
