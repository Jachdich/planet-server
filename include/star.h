#ifndef __STAR_H
#define __STAR_H
#include <jsoncpp/json/json.h>

#include "generation.h"
#include "planet.h"
#include "common/surfacelocator.h"

class Planet;
class Star {
public:
    uint32_t radius;
    uint32_t colour;
    Planet * planets;
    uint32_t x = 0;
    uint32_t y = 0;

    double noiseZ;
    double noiseScl;
    double noiseEffect;

    uint32_t num = 0;
    Star();
    Star(uint32_t x, uint32_t y, SurfaceLocator loc);
	Star(Json::Value root, SurfaceLocator loc);
    Json::Value asJson();
    
};
#endif
