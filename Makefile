.PHONY: all clean run fast
all: clean main.o
	g++ -g -o run main.o -pthread
main.o:
	g++ -c -Wall -g main.cpp -I./spdlog/include -o main.o
clean:
	rm -rf *.o run
clean_logs:
	rm -rf counter_values dump dump1
run: clean all
	./run
fast:
	./run
