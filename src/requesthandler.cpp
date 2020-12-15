#include "network.h"
#include "server.h"
#include "common/enums.h"
#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectormap.h"
#include "common/surfacelocator.h"

struct Update {
	Json::Value value;
	std::vector<int> readBy;
	int numRead = 1; //1 because the thread that instantiated the object has read it
	Update(Json::Value v, int id) {
		value = v;
		readBy.push_back(id);
	}
};

struct Task {
	TaskType type;
	uint32_t target;
	SurfaceLocator surface;
	double timeLeft;
	Connection * caller;
};

//std::mutex m;
//std::mutex updateQueue;
std::vector<Task> tasks;

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
    switch (type) {
        case TaskType::BUILD_HOUSE:
            if (surf->stats.wood >= 3 && surf->stats.stone >= 6) return true;
            break;
        default:
            return true;
    }
    return false;
}

void sendStatsChangeRequest(Stats stats, SurfaceLocator loc, Connection * conn) {
	Json::Value root;
	root["wood"] = stats.wood;
	root["stone"] = stats.stone;
	root["people"] = stats.people;
	root["peopleIdle"] = stats.peopleIdle;
	root["serverRequest"] = "statsChange";
	getJsonFromSurfaceLocator(loc, root);
	conn->sendMessage(root);
}

void sendTileChangeRequest(uint32_t pos, TileType type, SurfaceLocator loc, Connection * conn) {
    Json::Value root;
    PlanetSurface * s = getSurfaceFromLocator(loc);
    s->tiles[pos] = (s->tiles[pos] & 0xFFFFFFFF00000000) | (int)type;
    root["tilePos"] = pos;
    root["type"] = (int)type;
    root["serverRequest"] = "changeTile";
    getJsonFromSurfaceLocator(loc, root);
    conn->sendMessage(root);
}

void sendSetTimerRequest(double time, uint32_t target, SurfaceLocator loc, Connection * conn) {
    Json::Value root;
    root["serverRequest"] = "setTimer";
    root["time"] = time;
    root["tile"] = target;
    getJsonFromSurfaceLocator(loc, root);
    conn->sendMessage(root);
}

bool isTaskOnTile(uint32_t tile) {
    for (Task &t : tasks) {
        if (t.target == tile) {
            return true;
        }
    }
    return false;
}

ErrorCode dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, PlanetSurface * surf, Connection * caller) {
    if (surf->stats.peopleIdle <= 0) {
        return ErrorCode::NO_PEOPLE_AVAILABLE;
	}
	if (!hasMaterialsFor(surf, type)) {
	    return ErrorCode::INSUFFICIENT_RESOURCES;
	}
    if (isTaskOnTile(target)) {
        return ErrorCode::TASK_ALREADY_STARTED;
    }
    surf->stats.peopleIdle--;
    switch (type) {
        case TaskType::BUILD_HOUSE:
            surf->stats.wood -= 3;
            surf->stats.stone -= 6;
            break;
        default: break;
    }
    sendStatsChangeRequest(surf->stats, loc, caller);
	double time = getTimeForTask(type);
	
	sendSetTimerRequest(time, target, loc, caller);
	tasks.push_back({type, target, loc, time, caller});
	return ErrorCode::OK;
}

void taskFinished(Task &t) {
    PlanetSurface * surf = getSurfaceFromLocator(t.surface);
    surf->stats.peopleIdle++;
    switch (t.type) {
        case TaskType::FELL_TREE:
            sendTileChangeRequest(t.target, TileType::GRASS, t.surface, t.caller);
            surf->stats.wood += 1;
            break;
            
        case TaskType::GATHER_MINERALS:
            sendTileChangeRequest(t.target, TileType::GRASS, t.surface, t.caller);
            surf->stats.stone += 1;
            break;
            
        case TaskType::CLEAR:
            break;
            
	    case TaskType::PLANT_TREE:
	        break;
	        
	    case TaskType::BUILD_HOUSE:
	        sendTileChangeRequest(t.target, TileType::HOUSE, t.surface, t.caller);
	        surf->stats.houses += 1;
	        break;
    }
    sendStatsChangeRequest(surf->stats, t.surface, t.caller);
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

    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
	[](Task& t) {
		return t.timeLeft <= 0;
	}), tasks.end());

	lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	    		std::chrono::system_clock::now().time_since_epoch()).count();
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
            int x = requestJson.get("x", 0).asInt();
            int y = requestJson.get("y", 0).asInt();
            Sector * sector = map.getSectorAt(x, y);
            //sector->save("testsave");
            Json::Value sec = sector->asJson();

            Json::Value result;
            result["status"] = (int)ErrorCode::OK;
            result["result"] = sec;
            totalJson["results"].append(result);

        } else if (req == "getSurface") {
            Json::Value result;
            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            //Sector * sec = map.getSectorAt(requestJson["secX"].asInt(), requestJson["secT"].asInt());
            if (surf != nullptr) {
	            result["result"] = surf->asJson();
	            result["status"] = (int)ErrorCode::OK;
            } else {
	            result["status"] = (int)ErrorCode::OUT_OF_BOUNDS;
            }

            totalJson["results"].append(result);

        } else if (req == "userAction") {
            Json::Value result;

            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
            uint32_t target = requestJson["y"].asInt() * surf->rad * 2 + requestJson["x"].asInt();

        	result["status"] = (int)dispachTask((TaskType)requestJson["action"].asInt(), target, loc, surf, this);
        	
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
