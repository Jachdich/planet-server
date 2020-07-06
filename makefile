SOURCES = $(wildcard *.cpp)
HEADERS = $(SOURCES:.cpp=.h)
OBJECTS = $(SOURCES:.cpp=.o)

server: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lpthread -ljsoncpp

%.o: %.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -g

FastNoise.o: FastNoise.cpp
	g++ -c -o $@ $< -Wall -O3

debug: server
	gdb server

run: server
	./server

clean:
	rm *.o
	rm server
