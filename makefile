SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

server: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lpthread -ljsoncpp

obj/%.o: src/%.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -g -std=c++11 -Iinclude 

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3 -std=c++11 -Iinclude

debug: server
	gdb server

run: server
	./server

clean:
	rm obj/*.o
	rm client

.PHONY: clean
