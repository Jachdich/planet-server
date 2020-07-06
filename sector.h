#ifndef __SECTOR_H
#define __SECTOR_H
#include <jsoncpp/json/json.h>

#include "star.h"

class Sector {
public:
    int x, y, r, numStars;
    bool generated = false;
    std::vector<Star> stars;
    
    Sector();
    Sector(int sx, int sy, int sr);
    Star * getStarAt(int x, int y);
    void generate();
    Json::Value asJson();
};

#endif
