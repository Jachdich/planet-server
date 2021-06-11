#include "network.h"
#include "server.h"
#include "common/enums.h"
#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectormap.h"
#include "common/surfacelocator_test.h"
#include <thread>
//std::mutex m;
//std::mutex updateQueue;

#define bade nullptr

struct TaskTypeInfo {
    std::vector<TileType> expectedTileTypes;
    Resources cost;
    Resources gains;
    TileType tileType;
    uint32_t time;
};

std::unordered_map<TaskType, TaskTypeInfo> taskTypeInfos;

void registerTaskTypeInfo() {/*8
    taskTypeInfos[TaskType::FELL_TREE]          = TaskTypeInfo{{TileType::TREE, TileType::FOREST, TileType::PINE, TileType::PINEFOREST}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 5};
    taskTypeInfos[TaskType::MINE_ROCK]          = TaskTypeInfo{{TileType::ROCK}, Resources(), Resources({{"stone", 1}}), TileType::GRASS, 10};
    taskTypeInfos[TaskType::CLEAR]              = TaskTypeInfo{{}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 2};
    taskTypeInfos[TaskType::PLANT_TREE]         = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 1}}), Resources(), TileType::TREE, 2};
    taskTypeInfos[TaskType::BUILD_HOUSE]        = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 6}, {"stone", 3}}), Resources(), TileType::HOUSE, 20};
    taskTypeInfos[TaskType::BUILD_FARM]         = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 5}, {"stone", 7}}), Resources(), TileType::FARM, 45};
    taskTypeInfos[TaskType::BUILD_GREENHOUSE]   = TaskTypeInfo{{TileType::GRASS}, Resources({{"glass", 8}, {"wood", 3}, {"iron", 1}}), Resources(), TileType::GREENHOUSE, 100};
    taskTypeInfos[TaskType::BUILD_WATERPUMP]    = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 2}, {"stone", 3}, {"iron", 6}}), Resources(), TileType::WATERPUMP, 120};
    taskTypeInfos[TaskType::BUILD_MINE]         = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 8}, {"stone", 8}}), Resources(), TileType::MINE, 120};
    taskTypeInfos[TaskType::BUILD_BLASTFURNACE] = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 2}, {"stone", 8}}), Resources(), TileType::BLASTFURNACE, 140};
    taskTypeInfos[TaskType::BUILD_WAREHOUSE]    = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 16}, {"stone", 12}}), Resources(), TileType::WAREHOUSE, 150};
    taskTypeInfos[TaskType::BUILD_FORESTRY]     = TaskTypeInfo{{TileType::GRASS}, Resources({{"wood", 24}, {"stone", 6}, {"iron", 5}}), Resources(), TileType::FORESTRY, 2};

*/
    taskTypeInfos[TaskType::FELL_TREE]          = TaskTypeInfo{{TileType::TREE, TileType::FOREST, TileType::PINE, TileType::PINEFOREST}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 2};
    taskTypeInfos[TaskType::MINE_ROCK]          = TaskTypeInfo{{TileType::ROCK}, Resources(), Resources({{"stone", 1}}), TileType::GRASS, 2};
    taskTypeInfos[TaskType::CLEAR]              = TaskTypeInfo{{}, Resources(), Resources({{"wood", 1}}), TileType::GRASS, 2};
    taskTypeInfos[TaskType::PLANT_TREE]         = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::TREE, 2};
    taskTypeInfos[TaskType::BUILD_HOUSE]        = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::HOUSE, 2};
    taskTypeInfos[TaskType::BUILD_FARM]         = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::FARM, 2};
    taskTypeInfos[TaskType::BUILD_GREENHOUSE]   = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::GREENHOUSE, 2};
    taskTypeInfos[TaskType::BUILD_WATERPUMP]    = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::WATERPUMP, 2};
    taskTypeInfos[TaskType::BUILD_MINE]         = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::MINE, 2};
    taskTypeInfos[TaskType::BUILD_BLASTFURNACE] = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::BLASTFURNACE, 2};
    taskTypeInfos[TaskType::BUILD_WAREHOUSE]    = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::WAREHOUSE, 2};
    taskTypeInfos[TaskType::BUILD_FORESTRY]     = TaskTypeInfo{{TileType::GRASS}, Resources(), Resources(), TileType::FORESTRY, 2}; 
}

std::mutex m;

int lastID;
int numConnectedClients;
//int lastID;
//int numConnectedClients;

//Logger logger;

SectorMap map;
FastNoise noiseGen;

/*

client: {request: user action, type: build house}
server: {status: 0, serverRequest: change these material values}
server: {serverRequest: add timer on this tile at 5 seconds}
server: {serverRequest: set this tile to house}

*/

bool hasMaterialsFor(PlanetSurface * surf, TaskType type) {
   return surf->resources >= taskTypeInfos[type].cost;
}

void sendResourcesChangeRequest(Resources resources, SurfaceLocator loc) {
	Json::Value root;
	getJsonFromSurfaceLocator(loc, root);
	root["resources"] = getJsonFromResources(resources);
	PlanetSurface * s = getSurfaceFromLocator(loc);
	root["serverRequest"] = "statsChange";
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
}

void sendTileChangeRequest(uint32_t pos, TileType type, SurfaceLocator loc) {
    Json::Value root;
    PlanetSurface * s = getSurfaceFromLocator(loc);
    uint32_t z = s->tiles[pos]->z;
    delete s->tiles[pos]; // I Think
    s->tiles[pos] = Tile::fromType(type);
    s->tiles[pos]->z = z;
    root["tilePos"] = pos;
    root["type"] = (int)type;
    root["serverRequest"] = "changeTile";
    getJsonFromSurfaceLocator(loc, root);
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
}

void sendSetTimerRequest(double time, uint32_t target, SurfaceLocator loc) {
    Json::Value root;
    root["serverRequest"] = "setTimer";
    root["time"] = time;
    root["tile"] = target;
    getJsonFromSurfaceLocator(loc, root);
    PlanetSurface * s = getSurfaceFromLocator(loc);
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
}

bool isTaskOnTile(uint32_t tile) {
    for (Task &t : tasks) {
        if (t.target == tile) {
            return true;
        }
    }
    return false;
}

std::vector<TileType> getExpectedTileType(TaskType type) {
    return taskTypeInfos[type].expectedTileTypes;
}

ErrorCode dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, PlanetSurface * surf) {
    if (surf->resources["peopleIdle"] <= 0) {
        return ErrorCode::NO_PEOPLE_AVAILABLE;
	}
	if (!hasMaterialsFor(surf, type)) {
	    return ErrorCode::INSUFFICIENT_RESOURCES;
	}
    if (isTaskOnTile(target)) {
        return ErrorCode::TASK_ALREADY_STARTED;
    }

    std::vector<TileType> expected = getExpectedTileType(type);
    TileType got = surf->tiles[target]->getType();
    if (std::find(expected.begin(), expected.end(), got) == expected.end()) {
        return ErrorCode::TASK_ON_WRONG_TILE;
    }
    surf->resources["peopleIdle"]--;
    surf->resources -= taskTypeInfos[type].cost;

    sendResourcesChangeRequest(surf->resources, loc);
	double time = taskTypeInfos[type].time;
	
	sendSetTimerRequest(time, target, loc);
	tasks.push_back({type, target, loc, time});
	return ErrorCode::OK;
}

void taskFinished(Task &t) {
    PlanetSurface * surf = getSurfaceFromLocator(t.surface);
    surf->resources["peopleIdle"]++;

    surf->resources += taskTypeInfos[t.type].gains;
    sendTileChangeRequest(t.target, taskTypeInfos[t.type].tileType, t.surface);
    sendResourcesChangeRequest(surf->resources, t.surface);
}

long lastTime;

void tick() {
    long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    		std::chrono::system_clock::now().time_since_epoch()).count();
    double delta = (ms - lastTime) / 1000.0;

    for (Task &t : tasks) {
        t.timeLeft -= delta;
        if (t.timeLeft <= 0) {
            taskFinished(t);
        }
    }
    
	std::vector<PlanetSurface*> surfacesToTick;
	for (ServerInterface::Conn conn : iface.connections) {
		for (PlanetSurface *surf : conn->surfacesLoaded) {
			if (std::find(surfacesToTick.begin(), surfacesToTick.end(), surf) == surfacesToTick.end()) {
				surfacesToTick.push_back(surf);
			}
		}
	}

	for (PlanetSurface *surf : surfacesToTick) {
		surf->tick(delta);
	}

    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
	[](Task& t) {
		return t.timeLeft <= 0;
	}), tasks.end());

	lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	    		std::chrono::system_clock::now().time_since_epoch()).count();
    if (ticks % 100 == 0) {
	    save(); //TODO not a good idea!
	}
	ticks++;
}

void runServerLogic() {
     long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    		std::chrono::system_clock::now().time_since_epoch()).count();
    lastTime = ms;
	while (true) {
		unsigned long long startns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();
	    
		tick();

		unsigned long long endns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();

		std::this_thread::sleep_for(std::chrono::microseconds(100000 - (endns - startns)));
	}
}

void Connection::handleRequest(Json::Value& root) {
    Json::Value totalJson;
    totalJson["requests"] = root["requests"];

    for (Json::Value requestJson: root["requests"]) {
        std::string req = requestJson.get("request", "NULL").asString();

        if (req == "getSector") {
            int x = requestJson.get("x", 0).asUInt();
            int y = requestJson.get("y", 0).asUInt();
            Sector * sector = map.getSectorAt(x, y);
            Json::Value sec = sector->asJson();

            Json::Value result;
            result["status"] = (int)ErrorCode::OK;
            result["result"] = sec;
            totalJson["results"].append(result);

        } else if (req == "getSurface") {
            Json::Value result;
            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            surf->connectedClients.push_back(this);
            this->surfacesLoaded.push_back(surf);
            SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
            for (Task &t : tasks) {
                if (t.surface == loc) {
                    //sendSetTimerRequest(t.timeLeft, t.target, loc, this);
                    totalJson["serverRequest"] = "setTimer";
                    totalJson["time"] = t.timeLeft;
                    totalJson["tile"] = t.target;
                    getJsonFromSurfaceLocator(t.surface, totalJson);
                }
            }
            if (surf != bade) {
	            result["result"] = surf->asJson();
	            result["status"] = (int)ErrorCode::OK;
	            Sector * sec = map.getSectorAt(requestJson["secX"].asInt(), requestJson["secT"].asInt());
	            sec->save(saveName);
            } else {
	            result["status"] = (int)ErrorCode::OUT_OF_BOUNDS;
            }

            totalJson["results"].append(result);
        } else if (req == "unloadSurface") {
            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            surf->connectedClients.erase(std::remove(surf->connectedClients.begin(), surf->connectedClients.end(), this), surf->connectedClients.end()); 
            surfacesLoaded.erase(std::remove(surfacesLoaded.begin(), surfacesLoaded.end(), surf), surfacesLoaded.end()); 
        } else if (req == "userAction") {
            Json::Value result;

            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
            uint32_t target = requestJson["y"].asInt() * surf->rad * 2 + requestJson["x"].asInt();

        	result["status"] = (int)dispachTask((TaskType)requestJson["action"].asInt(), target, loc, surf);
        	
            totalJson["results"].append(result);

        } else {
            logger.warn("Client sent invalid request: " + root.get("request", "NULL").asString());
            Json::Value result;
            result["status"] = (int)ErrorCode::INVALID_REQUEST;
            totalJson["results"].append(result);
        }
    }

   sendMessage(totalJson);
}

void Connection::disconnect() {
	std::cout << "Client disconnected\n";
    for (PlanetSurface *surf : surfacesLoaded) {
        surf->connectedClients.erase(std::remove(surf->connectedClients.begin(), surf->connectedClients.end(), this), surf->connectedClients.end()); 
    }
    iface.connections.erase(std::remove(iface.connections.begin(), iface.connections.end(), shared_from_this()), iface.connections.end()); 

}
