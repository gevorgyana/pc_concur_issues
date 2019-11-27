all:
	g++ main.cpp -I./spdlog/include
clean:
	rm -rf *.o
