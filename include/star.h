#ifndef __STAR_H
#define __STAR_H
#include <jsoncpp/json/json.h>

#include "generation.h"
#include "planet.h"
#include "common/surfacelocator.h"

class Planet;
class Star {
public:
    int radius;
    Pixel colour;
    Planet * planets;
    int x = 0;
    int y = 0;

    int num = 0;
    Star();
    Star(int x, int y, SurfaceLocator loc);
	Star(Json::Value root, SurfaceLocator loc);
    Json::Value asJson();
    
};
#endif
