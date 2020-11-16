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
	uint32_t caller;
};

//std::mutex m;
//std::mutex updateQueue;
std::vector<Update> updates;
std::vector<Task> tasks;

std::mutex m;
std::mutex updateQueue;

int lastID;
int numConnectedClients;
//int lastID;
//int numConnectedClients;

//Logger logger;

SectorMap map;
FastNoise noiseGen;

void dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, PlanetSurface * surf, uint32_t id) {
    switch (type) {
        case TaskType::BUILD_HOUSE:
            surf->stats.wood -= 3;
            surf->stats.stone -= 6;
            break;
        default: break;
    }
	double time = getTimeForTask(type);
	tasks.push_back({type, target, loc, time, id});
}

PlanetSurface * getSurfaceFromLocator(SurfaceLocator loc) {
    Sector * sec = map.getSectorAt(loc.sectorX, loc.sectorY);
	if (loc.starPos < sec->numStars) {
		Star * s = &sec->stars[loc.starPos];
		if (loc.planetPos < s->num) {
			Planet * p = &s->planets[static_cast<int>(loc.planetPos)];
			PlanetSurface * surf = p->getSurface();
			return surf;
		} else {
			return nullptr;
		}
	} else {
		return nullptr;
	}
}

PlanetSurface * getSurfaceFromJson(Json::Value root) {
	SurfaceLocator loc = getSurfaceLocatorFromJson(root);
	return getSurfaceFromLocator(loc);
}

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

void taskFinished(Task &t) {
    PlanetSurface * surf = getSurfaceFromLocator(t.surface);
    surf->stats.peopleIdle++;
    switch (t.type) {
        case TaskType::FELL_TREE:
            surf->tiles[t.target] = (surf->tiles[t.target] & 0xFFFFFFFF00000000) | (int)TileType::GRASS;
            surf->stats.wood += 1;
            
        case TaskType::GATHER_MINERALS:
            surf->tiles[t.target] = (surf->tiles[t.target] & 0xFFFFFFFF00000000) | (int)TileType::GRASS;
            surf->stats.stone += 1;
            break;
            
        case TaskType::CLEAR:
            break;
            
	    case TaskType::PLANT_TREE:
	        break;
	        
	    case TaskType::BUILD_HOUSE:
	        surf->tiles[t.target] = (surf->tiles[t.target] & 0xFFFFFFFF00000000) | (int)TileType::HOUSE;
	        surf->stats.houses += 1;
	        break;
    }
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


bool threadStopped = false;

void handleTasks() {
     long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    		std::chrono::system_clock::now().time_since_epoch()).count();
    lastTime = ms;
	while (!threadStopped) {
		unsigned long long startns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();
	    
		tick();

		unsigned long long endns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();

		std::this_thread::sleep_for(std::chrono::microseconds(100000 - (endns - startns)));
	}
}

void Connection::handleRequest(Json::Value& root) {
	 std::unique_lock<std::mutex> uQ(updateQueue);
	 uint32_t id = lastID++;
	 numConnectedClients++;
	 uQ.unlock();

        Json::Value totalJson;

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

				if (surf != nullptr) {
					result["result"] = surf->asJson();
					result["status"] = (int)ErrorCode::OK;
				} else {
					result["status"] = (int)ErrorCode::OUT_OF_BOUNDS;
				}

                totalJson["results"].append(result);

			} else if (req == "change") {
                Json::Value result;
                PlanetSurface * surf = getSurfaceFromJson(requestJson);

				if (surf != nullptr) {
					int x, y;
					x = requestJson.get("x", -1).asInt();
					y = requestJson.get("y", -1).asInt();
					std::cout << "Change Tile " << x << " " << y << "\n";
					if (x < 0 || y < 0) {
						result["status"] = -3;
					} else {
						surf->tiles[y * surf->rad * 2 + x] = requestJson.get("to", 0).asInt();

						Json::Value updateJson;
						getJsonFromSurfaceLocator(getSurfaceLocatorFromJson(requestJson), updateJson);
						updateJson["x"] = x;
						updateJson["y"] = y;
						updateJson["to"] = requestJson.get("to", 0).asInt();

						uQ.lock();
						updates.push_back(Update(updateJson, id));
						uQ.unlock();
						result["status"] = (int)ErrorCode::OK;
					}
				} else {
					result["status"] = (int)ErrorCode::OUT_OF_BOUNDS;
				}

                totalJson["results"].append(result);
			} else if (req == "keepAlive") {
				//do nothing. Just so `else` doesnt fire
			} else if (req == "userAction") {
				Json::Value result;
				
				PlanetSurface * surf = getSurfaceFromJson(requestJson);
				SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
				uint32_t target = requestJson["y"].asInt() * surf->rad * 2 + requestJson["x"].asInt();
				TaskType task = (TaskType)requestJson["action"].asInt();
				
				if (surf->stats.peopleIdle > 0 && hasMaterialsFor(surf, task)) {
					surf->stats.peopleIdle--;
					int time = getTimeForTask(task);
					dispachTask((TaskType)requestJson["action"].asInt(), target, loc, surf, id);
					result["status"] = (int)ErrorCode::OK;
					result["time"] = time;
				} else {
				    if (surf->stats.peopleIdle <= 0) {
				        result["status"] = (int)ErrorCode::NO_PEOPLE_AVAILABLE;
					} else {
					    result["status"] = (int)ErrorCode::INSUFFICIENT_RESOURCES;
					}
					result["time"] = -1;
				}

				totalJson["results"].append(result);

            } else {
                logger.warn("Client sent invalid request: " + root.get("request", "NULL").asString());
                Json::Value result;
                result["status"] = (int)ErrorCode::INVALID_REQUEST;
                totalJson["results"].append(result);
            }
        }

		uQ.lock();
		for (unsigned int i = 0; i < updates.size(); i++) {
			if (std::find(updates[i].readBy.begin(), updates[i].readBy.end(), id) != updates[i].readBy.end()) {
				continue;
			}
			totalJson["updates"].append(updates[i].value);
			updates[i].readBy.push_back(id);
			updates[i].numRead++;
		}
		updates.erase(std::remove_if(updates.begin(), updates.end(), [](const Update &u) {
			return u.numRead == numConnectedClients;
			}), updates.end());

		uQ.unlock();

        asio::error_code err;
        Json::StreamWriterBuilder writeBuilder;
        writeBuilder["indentation"] = "";
        const std::string output = Json::writeString(writeBuilder, totalJson);
        asio::write(sock, asio::buffer(output + "\n"), err);
}