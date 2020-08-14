#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H
#include <jsoncpp/json/json.h>
#include <vector>
class Planet;

class PlanetSurface {
public:
    std::vector<int> tiles;
    bool generated = false;
    int rad = 0;
    
    PlanetSurface();
    void generate(Planet * p);
	int getType(int r, int g, int b);
    int getInitialTileType(int x, int y, Planet * p);
    Json::Value asJson();
};
    
#endif
