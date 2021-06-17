#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H
#include <jsoncpp/json/json.h>
#include <vector>
#include <cstdint>
#include "common/enums.h"
#include "common/resources.h"
#include "common/surfacelocator.h"
#include "tile.h"
class Planet;
class Connection;
class Tile;

class PlanetSurface {
public:
    std::vector<Tile*> tiles;
    bool generated = false;
    Resources resources;
    int rad = 0;
    int noiseZ;
    uint64_t lastTicks = -1;
    double noiseScl;
    SurfaceLocator loc;
    Planet *parent;
    std::vector<Connection*> connectedClients;

    PlanetSurface(SurfaceLocator loc);
    PlanetSurface(Json::Value root, SurfaceLocator loc, Planet *parent);
    void generate(Planet * p);
    void tick(double elapsedTime);
	Tile* getType(uint8_t r, uint8_t g, uint8_t b, int32_t x, int32_t y);
    Tile* getInitialTileType(int32_t x, int32_t y);
    uint32_t getTileColour(int32_t x, int32_t y);
    Json::Value asJson(bool addErrors = false);

};

#endif
