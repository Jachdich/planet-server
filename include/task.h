#ifndef __TASK_H
#define __TASK_H
#include <vector>
#include "common/enums.h"
#include "common/surfacelocator.h"

struct Task {
	TaskType type;
	uint32_t target;
	SurfaceLocator surface;
	double timeLeft;
};

#include "common/resources.h"
#include "tile.h"

class PlanetSurface;


struct TaskTypeInfo {
    std::vector<TileType> expectedTileTypes;
    Resources cost;
    Resources gains;
    TileType tileType;
    uint32_t time;
    bool requiresPeople = true;

    inline TaskTypeInfo(std::vector<TileType> expectedTileTypes, Resources cost, Resources gains, TileType tileType, uint32_t time) {
        this->expectedTileTypes = expectedTileTypes;
        this->cost = cost;
        this->gains = gains;
        this->tileType = tileType;
        this->time = time;
    }

    inline TaskTypeInfo(std::vector<TileType> expectedTileTypes, Resources cost, Resources gains, TileType tileType, uint32_t time, bool requiresPeople) {
        this->expectedTileTypes = expectedTileTypes;
        this->cost = cost;
        this->gains = gains;
        this->tileType = tileType;
        this->time = time;
        this->requiresPeople = requiresPeople;
    }

    inline TaskTypeInfo() {}
};
ErrorCode dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, PlanetSurface * surf);
void taskFinished(Task &t, PlanetSurface *surf);
extern TaskTypeInfo taskTypeInfos[TASK_INVALID + 1];
#endif
