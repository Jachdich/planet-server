#ifndef __SECTORMAP_H
#define __SECTORMAP_H

#include <unordered_map>
#include "sector.h"

class SectorMap {
public:
	std::unordered_map<uint64_t, Sector> cache;

    SectorMap();
    Sector * getSectorAt(int32_t x, int32_t y);
    void saveAll(std::string name);
    void unloadAll();
};
#endif
