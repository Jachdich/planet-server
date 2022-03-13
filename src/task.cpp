#include "../include/config.h"
#include "../include/task.h"
#include "../include/network.h"

TaskTypeInfo taskTypeInfos[TASK_INVALID + 1];

//shorthand for making the struct
#define RES(n) res_from_items(n)
#define BR res_init()

//even shorter hand for making a Resource Value
#define RI(t,c) (ResourceItem){t,c}

void registerTaskTypeInfo() {
    ResourceItem tree_gain[]  = {RI(RES_WOOD, 1)};
    ResourceItem rock_gain[]  = {RI(RES_STONE, 1)};
    ResourceItem tree_cost[]  = {RI(RES_WOOD, 1)};
    ResourceItem house_cost[] = {RI(RES_WOOD, 6), RI(RES_STONE, 3)};
    ResourceItem farm_cost[] = {RI(RES_WOOD, 5), RI(RES_STONE, 7)};
    ResourceItem greenhouse_cost[] = {RI(RES_GLASS, 8), RI(RES_WOOD, 3), RI(RES_IRON, 1)};
/*#ifdef DEBUG_MODE

    taskTypeInfos[TASK_FELL_TREE]          = TaskTypeInfo({TILE_TREE, TILE_FOREST, TILE_PINE, TILE_PINEFOREST}, Resources(), RES((ResourceItem[NUM_RESOURCES]){RV(RES_WOOD, 1)}), TILE_GRASS, 2, false);
    taskTypeInfos[TASK_MINE_ROCK]          = TaskTypeInfo({TILE_ROCK}, (Resources){}, Resources({{"stone", 1}}), TILE_GRASS, 2, false);
    taskTypeInfos[TASK_CLEAR]              = TaskTypeInfo({}, (Resources){}, (Resources){}, TILE_GRASS, 2, false);
    taskTypeInfos[TASK_PLANT_TREE]         = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_TREE,         2, false);
    taskTypeInfos[TASK_BUILD_HOUSE]        = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_HOUSE,        2, false);
    taskTypeInfos[TASK_BUILD_FARM]         = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_FARM,         2, false);
    taskTypeInfos[TASK_BUILD_GREENHOUSE]   = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_GREENHOUSE,   2, false);
    taskTypeInfos[TASK_BUILD_WATERPUMP]    = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_WATERPUMP,    2, false);
    taskTypeInfos[TASK_BUILD_MINE]         = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_MINE,         2, false);
    taskTypeInfos[TASK_BUILD_BLASTFURNACE] = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_BLASTFURNACE, 2, false);
    taskTypeInfos[TASK_BUILD_WAREHOUSE]    = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_WAREHOUSE,    2, false);
    taskTypeInfos[TASK_BUILD_FORESTRY]     = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_FORESTRY,     2, false);
    taskTypeInfos[TASK_BUILD_CAPSULE]      = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_CAPSULE,      2, false);
    taskTypeInfos[TASK_BUILD_ROAD]         = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_ROAD,         2, false);
    taskTypeInfos[TASK_BUILD_PIPE]         = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_PIPE,         2, false);
    taskTypeInfos[TASK_BUILD_CABLE]        = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_CABLE,        2, false);
    taskTypeInfos[TASK_BUILD_POWERSTATION] = TaskTypeInfo({TILE_GRASS}, (Resources){}, (Resources){}, TILE_POWERSTATION, 2, false);
#else*/
    taskTypeInfos[TASK_FELL_TREE]          = TaskTypeInfo({TILE_TREE, TILE_FOREST, TILE_PINE, TILE_PINEFOREST}, BR, RES(tree_gain), TILE_GRASS, 5);
    taskTypeInfos[TASK_MINE_ROCK]          = TaskTypeInfo({TILE_ROCK}, BR, RES(rock_gain), TILE_GRASS, 6);
    taskTypeInfos[TASK_CLEAR]              = TaskTypeInfo({}, BR, RES(tree_gain), TILE_GRASS, 2);
    taskTypeInfos[TASK_PLANT_TREE]         = TaskTypeInfo({TILE_GRASS}, RES(tree_cost),       BR, TILE_TREE,       2);
    taskTypeInfos[TASK_BUILD_HOUSE]        = TaskTypeInfo({TILE_GRASS}, RES(house_cost),      BR, TILE_HOUSE,      20);
    taskTypeInfos[TASK_BUILD_FARM]         = TaskTypeInfo({TILE_GRASS}, RES(farm_cost),       BR, TILE_FARM,       30);
    taskTypeInfos[TASK_BUILD_GREENHOUSE]   = TaskTypeInfo({TILE_GRASS}, RES(greenhouse_cost), BR, TILE_GREENHOUSE, 100);
    taskTypeInfos[TASK_BUILD_WATERPUMP]    = TaskTypeInfo({TILE_GRASS}, (Resources){{"wood", 2}, {"stone", 3}, {"iron", 6}}), Resources(), TILE_WATERPUMP, 120);
    taskTypeInfos[TASK_BUILD_MINE]         = TaskTypeInfo({TILE_GRASS}, (Resources){{"wood", 8}, {"stone", 8}}), Resources(), TILE_MINE, 120);
    taskTypeInfos[TASK_BUILD_BLASTFURNACE] = TaskTypeInfo({TILE_GRASS}, (Resources){{"wood", 2}, {"stone", 8}}), Resources(), TILE_BLASTFURNACE, 140);
    taskTypeInfos[TASK_BUILD_WAREHOUSE]    = TaskTypeInfo({TILE_GRASS}, (Resources){{"wood", 16}, {"stone", 12}}), Resources(), TILE_WAREHOUSE, 150);
    taskTypeInfos[TASK_BUILD_FORESTRY]     = TaskTypeInfo({TILE_GRASS}, (Resources){{"wood", 24}, {"stone", 6}, {"iron", 5}}), Resources(), TILE_FORESTRY, 2);
    taskTypeInfos[TASK_BUILD_CAPSULE]      = TaskTypeInfo({TILE_GRASS}, (Resources){}, Resources(), TILE_CAPSULE, 1, false);
    taskTypeInfos[TASK_BUILD_ROAD]         = TaskTypeInfo({TILE_GRASS}, (Resources){{"stone", 2}}), Resources(), TILE_ROAD, 10);
    taskTypeInfos[TASK_BUILD_PIPE]         = TaskTypeInfo({TILE_GRASS}, (Resources){{"iron",  3}}), Resources(), TILE_PIPE, 10);
    taskTypeInfos[TASK_BUILD_CABLE]        = TaskTypeInfo({TILE_GRASS}, (Resources){{"copper", 3}}), Resources(), TILE_CABLE, 10);
    taskTypeInfos[TASK_BUILD_POWERSTATION] = TaskTypeInfo({TILE_GRASS}, (Resources){{"copper", 10}, {"aluminium", 5}, {"stone", 8}, {"iron", 5}}), Resources(), TILE_POWERSTATION, 10);
//#endif
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
