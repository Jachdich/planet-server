#include "config.h"
#include "task.h"
#include "network.h"

std::unordered_map<TaskType, TaskTypeInfo> taskTypeInfos;

void registerTaskTypeInfo() {
#ifdef DEBUG_MODE
    taskTypeInfos[TaskType::FELL_TREE]          = TaskTypeInfo({TileType::TREE, TileType::FOREST, TileType::PINE, TileType::PINEFOREST}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 2, false);
    taskTypeInfos[TaskType::MINE_ROCK]          = TaskTypeInfo({TileType::ROCK}, Resources(), Resources({{"stone", 1}}), TileType::GRASS, 2, false);
    taskTypeInfos[TaskType::CLEAR]              = TaskTypeInfo({}, Resources(), Resources(), TileType::GRASS, 2, false);
    taskTypeInfos[TaskType::PLANT_TREE]         = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::TREE, 2, false);
    taskTypeInfos[TaskType::BUILD_HOUSE]        = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::HOUSE, 2, false);
    taskTypeInfos[TaskType::BUILD_FARM]         = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::FARM, 2, false);
    taskTypeInfos[TaskType::BUILD_GREENHOUSE]   = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::GREENHOUSE, 2, false);
    taskTypeInfos[TaskType::BUILD_WATERPUMP]    = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::WATERPUMP, 2, false);
    taskTypeInfos[TaskType::BUILD_MINE]         = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::MINE, 2, false);
    taskTypeInfos[TaskType::BUILD_BLASTFURNACE] = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::BLASTFURNACE, 2, false);
    taskTypeInfos[TaskType::BUILD_WAREHOUSE]    = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::WAREHOUSE, 2, false);
    taskTypeInfos[TaskType::BUILD_FORESTRY]     = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::FORESTRY, 2, false);
    taskTypeInfos[TaskType::BUILD_CAPSULE]      = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::CAPSULE, 2, false);
    taskTypeInfos[TaskType::BUILD_ROAD]         = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::ROAD, 2, false);
    taskTypeInfos[TaskType::BUILD_PIPE]         = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::PIPE, 2, false);
    taskTypeInfos[TaskType::BUILD_CABLE]        = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::CABLE, 2, false);
    taskTypeInfos[TaskType::BUILD_POWERSTATION] = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::POWERSTATION, 2, false);
#else
    taskTypeInfos[TaskType::FELL_TREE]          = TaskTypeInfo({TileType::TREE, TileType::FOREST, TileType::PINE, TileType::PINEFOREST}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 5);
    taskTypeInfos[TaskType::MINE_ROCK]          = TaskTypeInfo({TileType::ROCK}, Resources(), Resources({{"stone", 1}}), TileType::GRASS, 10);
    taskTypeInfos[TaskType::CLEAR]              = TaskTypeInfo({}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 2);
    taskTypeInfos[TaskType::PLANT_TREE]         = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 1}}), Resources(), TileType::TREE, 2);
    taskTypeInfos[TaskType::BUILD_HOUSE]        = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 6}, {"stone", 3}}), Resources(), TileType::HOUSE, 20);
    taskTypeInfos[TaskType::BUILD_FARM]         = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 5}, {"stone", 7}}), Resources(), TileType::FARM, 45);
    taskTypeInfos[TaskType::BUILD_GREENHOUSE]   = TaskTypeInfo({TileType::GRASS}, Resources({{"glass", 8}, {"wood", 3}, {"iron", 1}}), Resources(), TileType::GREENHOUSE, 100);
    taskTypeInfos[TaskType::BUILD_WATERPUMP]    = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 2}, {"stone", 3}, {"iron", 6}}), Resources(), TileType::WATERPUMP, 120);
    taskTypeInfos[TaskType::BUILD_MINE]         = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 8}, {"stone", 8}}), Resources(), TileType::MINE, 120);
    taskTypeInfos[TaskType::BUILD_BLASTFURNACE] = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 2}, {"stone", 8}}), Resources(), TileType::BLASTFURNACE, 140);
    taskTypeInfos[TaskType::BUILD_WAREHOUSE]    = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 16}, {"stone", 12}}), Resources(), TileType::WAREHOUSE, 150);
    taskTypeInfos[TaskType::BUILD_FORESTRY]     = TaskTypeInfo({TileType::GRASS}, Resources({{"wood", 24}, {"stone", 6}, {"iron", 5}}), Resources(), TileType::FORESTRY, 2);
    taskTypeInfos[TaskType::BUILD_CAPSULE]      = TaskTypeInfo({TileType::GRASS}, Resources(), Resources(), TileType::CAPSULE, 1, false);
    taskTypeInfos[TaskType::BUILD_ROAD]         = TaskTypeInfo({TileType::GRASS}, Resources({{"stone", 2}}), Resources(), TileType::ROAD, 10);
    taskTypeInfos[TaskType::BUILD_PIPE]         = TaskTypeInfo({TileType::GRASS}, Resources({{"iron",  3}}), Resources(), TileType::PIPE, 10);
    taskTypeInfos[TaskType::BUILD_CABLE]        = TaskTypeInfo({TileType::GRASS}, Resources({{"copper", 3}}), Resources(), TileType::CABLE, 10);
    taskTypeInfos[TaskType::BUILD_POWERSTATION] = TaskTypeInfo({TileType::GRASS}, Resources({{"copper", 10}, {"aluminium", 5}, {"stone", 8}, {"iron", 5}}), Resources(), TileType::POWERSTATION, 10);
#endif
}

bool isTaskOnTile(PlanetSurface* surf, uint32_t tile) {
    for (Task &t : surf->tasks) {
        if (t.target == tile) {
            return true;
        }
    }
    return false;
}

std::vector<TileType> getExpectedTileType(TaskType type) {
    return taskTypeInfos[type].expectedTileTypes;
}

bool hasMaterialsFor(PlanetSurface * surf, TaskType type) {
    return surf->resources >= taskTypeInfos[type].cost;
}

ErrorCode dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, PlanetSurface * surf) {
    if (surf->resources["peopleIdle"] <= 0 && taskTypeInfos[type].requiresPeople) {
        return ErrorCode(ErrorCode::INVALID_ACTION, "No people available to\ncomplete action!");
	}
	if (!hasMaterialsFor(surf, type)) {
	    return ErrorCode(ErrorCode::INVALID_ACTION, "Insufficient resources!");
	}
    if (isTaskOnTile(surf, target)) {
        return ErrorCode(ErrorCode::INVALID_ACTION, "There is already a task\non this tile!");
    }

    std::vector<TileType> expected = getExpectedTileType(type);
    TileType got = surf->tiles[target]->getType();
    if (std::find(expected.begin(), expected.end(), got) == expected.end() && taskTypeInfos[type].expectedTileTypes.size() != 0) {
        return ErrorCode(ErrorCode::INVALID_ACTION, "This task is not available\non this tile!");
    }
    
    if (taskTypeInfos[type].requiresPeople) {
        surf->resources["peopleIdle"]--;
    }
    surf->resources -= taskTypeInfos[type].cost;

    sendResourcesChangeRequest(surf->resources, loc);
	double time = taskTypeInfos[type].time;
	
	sendSetTimerRequest(time, target, loc);
	surf->tasks.push_back({type, target, loc, time});
	return ErrorCode::OK;
}

void taskFinished(Task &t, PlanetSurface *surf) {
    surf->resources += taskTypeInfos[t.type].gains;
    sendTileChangeRequest(t.target, taskTypeInfos[t.type].tileType, t.surface);
    sendResourcesChangeRequest(surf->resources, t.surface);
}
