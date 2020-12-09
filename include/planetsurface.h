#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H
#include <jsoncpp/json/json.h>
#include <vector>
#include <cstdint>
#include "common/enums.h"
#include "common/stats.h"
class Planet;

class PlanetSurface {
public:
    std::vector<uint64_t> tiles;
    bool generated = false;
    Stats stats;
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
