#ifndef __SECTOR_H
#define __SECTOR_H
#include <jsoncpp/json/json.h>
//#include <filesystem>

#include "star.h"

class Sector {
public:
    uint32_t x, y;
    uint32_t r;
    uint32_t numStars;
    bool generated = false;
    std::vector<Star> stars;
    
    Sector();
    Sector(uint32_t sx, uint32_t sy, int sr);
    Star * getStarAt(int x, int y);
    void generate();
	void generate(std::string dir);
	bool existsInSave(std::string dir);
    Json::Value asJson();
	void save(std::string dir);
};

#endif
