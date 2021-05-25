#include "sectormap.h"

#include <mutex>
#include <iostream>
#include "sector.h"
#include "server.h"

SectorMap::SectorMap() {
}

Sector * SectorMap::getSectorAt(int x, int y) {
    std::lock_guard<std::mutex> lock(m);
    uint64_t index = (uint64_t)x << 32 | (uint32_t)y;
    //if (x < 0 || y < 0) {
    //    return &this->cache[0][0];
    //}
    /*
    bool needToGenerate = false;
    if ((signed)cache.size() <= y) {
        cache.resize(y + 1);
        cache[y] = std::vector<Sector>();
        needToGenerate = true;
    }
    if ((signed)cache[y].size() <= x) {
        cache[y].resize(x + 1);
        needToGenerate = true;
    }*/
    if (!(this->cache.count(index) > 0)) {
        Sector a(x, y, 256);

		if (a.existsInSave(saveName)) {
			a.generate(saveName);
		} else {
			a.generate();
			a.save(saveName);
		}

        cache[index] = a;
    }
    return &this->cache[index];
}

void SectorMap::saveAll(std::string name) {
    //for (int y = 0; y < )
}

void SectorMap::unloadAll() {

}
