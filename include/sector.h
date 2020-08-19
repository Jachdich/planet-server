#ifndef __SECTOR_H
#define __SECTOR_H
#include <jsoncpp/json/json.h>
//#include <filesystem>

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
	void generate(std::string dir);
	bool existsInSave(std::string dir);
    Json::Value asJson();
	void save(std::string dir);
};

#endif
