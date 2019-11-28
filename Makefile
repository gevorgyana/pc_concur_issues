.PHONY: all clean run fast

main_restricted.o:
	g++ -c -Wall -g main_restricted.cpp -I./spdlog/include -o main_restricted.o
	g++ -g -o restricted main_restricted.o  -pthread

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
