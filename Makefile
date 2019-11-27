.PHONY: all clean run fast
all: main.o
	g++ -g -o run main.o -pthread
main.o:
	g++ -c -Wall -g main.cpp -I./spdlog/include -o main.o
clean:
	rm -rf *.o run
run: all
	./run
fast:
	./run
