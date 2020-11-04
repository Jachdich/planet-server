#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H
#include <jsoncpp/json/json.h>
#include <vector>
#include <cstdint>
#include "common/enums.h"
class Planet;

struct Stats {
	uint32_t people;
	uint32_t peopleIdle;
	uint32_t wood;
	uint32_t stone;
};

class PlanetSurface {
public:
    std::vector<uint64_t> tiles;
    bool generated = false;
    Stats stats = {4, 4, 0, 0};
    int rad = 0;
    int noiseZ;
    double noiseScl;

    PlanetSurface();
    void generate(Planet * p);
	TileType getType(int r, int g, int b, int x, int y);
    TileType getInitialTileType(int x, int y, Planet * p);
    Json::Value asJson();
};

#endif
