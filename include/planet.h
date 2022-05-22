#ifndef __PLANET_H
#define __PLANET_H
#include <jsoncpp/json/json.h>
#include "generation.h"
#include "planetsurface.h"
#include "common/surfacelocator.h"

class PlanetSurface;

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    int seaLevel;

    uint64_t owner = (uint64_t)-1;
    
    double * generationChances;
    Pixel * generationColours;
    int * generationZValues;
    double * generationNoise;

    uint32_t sectorSeed;
    
    PlanetSurface * surface;
    
    Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet();
    Planet(SurfaceLocator loc, uint32_t sectorSeed);
	Planet(Json::Value res, SurfaceLocator loc, Planet *other_this);
    Json::Value asJson();
    PlanetSurface * getSurface();

    void setPosFromStar(uint32_t pos);
};
#endif
