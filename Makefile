.PHONY: all clean run
all: main.o
	g++ -g -o run main.o -pthread
main.o:
	g++ -c -Wall -g main.cpp -I./spdlog/include -o main.o
clean:
	rm -rf *.o
run: all
	./a.out
