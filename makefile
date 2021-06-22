SOURCES := src/FastNoise.cpp src/generation.cpp src/planet.cpp src/requesthandler.cpp src/sectormap.cpp src/serverinterface.cpp src/tile.cpp src/connection.cpp src/logging.cpp src/planetsurface.cpp src/sector.cpp src/server.cpp src/star.cpp src/task.cpp
HEADERS := include/FastNoise.h include/logging.h include/planet.h include/random_helper.h include/sectormap.h include/star.h include/generation.h include/network.h include/planetsurface.h include/server.h include/tile.h include/task.h include/config.h
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

server: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lpthread -ljsoncpp -lssl -lcrypto

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -std=c++17 -c -o $@ $< -Wall -g -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -O3 -std=c++11 -Iinclude

debug: server
	gdb server -ex ru

run: server
	./server

clean:
	rm obj/*.o
	rm server

.PHONY: clean
