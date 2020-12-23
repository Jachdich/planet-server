#ifndef __SECTORMAP_H
#define __SECTORMAP_H

#include "sector.h"

class SectorMap {
public:
    std::vector<std::vector<Sector>> cache;

    SectorMap();
    Sector * getSectorAt(int x, int y);
    void saveAll(std::string name);
};
#endif
