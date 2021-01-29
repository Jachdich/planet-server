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
    
    double * generationChances;
    Pixel * generationColours;
    int * generationZValues;
    double * generationNoise;
    
    PlanetSurface * surface;
    
    Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet();
    Planet(int posFromStar, SurfaceLocator loc);
	Planet(Json::Value res, SurfaceLocator loc);
    Json::Value asJson();
    PlanetSurface * getSurface();
};
#endif
