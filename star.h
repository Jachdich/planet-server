#ifndef __STAR_H
#define __STAR_H
#include <jsoncpp/json/json.h>

#include "generation.h"
#include "planet.h"
class Star {
public:
    int radius;
    Pixel colour;
    Planet * planets;
    int x = 0;
    int y = 0;

    int num = 0;
    Star();
    Star(int x, int y);
    Json::Value asJson();
    
};
#endif
