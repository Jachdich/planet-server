
#SOURCES := src/FastNoise.cpp src/generation.cpp src/planet.cpp src/requesthandler.cpp src/sectormap.cpp src/serverinterface.cpp src/tile.cpp src/connection.cpp src/logging.cpp src/planetsurface.cpp src/sector.cpp src/server.cpp src/star.cpp src/task.cpp
#HEADERS := include/FastNoise.h include/logging.h include/planet.h include/random_helper.h include/sectormap.h include/star.h include/generation.h include/network.h include/planetsurface.h include/server.h include/tile.h include/task.h include/config.h
#OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

SOURCES := $(shell find src -type f -name *.cpp)
RUST_SOURCES := $(shell find src -type f -name *.rs)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

server: $(OBJECTS) target/debug/libplanet_server.a
	g++ $(OBJECTS) -o $@ -Ltarget/debug -lplanet_server -ldl -lpthread -ljsoncpp -lssl -lcrypto -lncurses

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -std=c++17 -c -o $@ $< -Wall -O3 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -O3 -std=c++11 -Iinclude

target/debug/libplanet_server.a: $(RUST_SOURCES)
	cargo build

debug: server
	gdb server -ex ru

run: server
	./server

clean:
	rm obj/*.o
	rm server

.PHONY: clean
