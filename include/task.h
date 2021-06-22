#ifndef __TASK_H
#define __TASK_H
#include <vector>
#include "common/resources.h"
#include "common/enums.h"
#include "common/surfacelocator.h"
#include "tile.h"

class PlanetSurface;

struct Task {
	TaskType type;
	uint32_t target;
	SurfaceLocator surface;
	double timeLeft;
};

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
extern std::unordered_map<TaskType, TaskTypeInfo> taskTypeInfos;
#endif