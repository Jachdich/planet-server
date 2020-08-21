SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

server: $(OBJECTS)
	g++ $(OBJECTS) common.a -o $@ -lpthread -ljsoncpp

obj/%.o: src/%.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -Werror -g -std=c++11 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3 -std=c++11 -Iinclude

debug: server
	gdb server

.PHONY: run
run: server
	./server

.PHONY: runtime_ld_error
runtime_ld_error:
	echo "Warning: Workaround for my specific machine. MAY BREAK THINGS!"
	export LD_LIBRARY_PATH=/usr/local/lib64

clean:
	rm obj/*.o
	rm client

.PHONY: clean
