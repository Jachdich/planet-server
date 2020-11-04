#include "server.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "generation.h"
#include "logging.h"
#include "sector.h"
#include "sectormap.h"
#include "FastNoise.h"
#include "common/enums.h"
#include "common/surfacelocator.h"
#include "random_helper.h"

//return codes
// -1: malformed JSON
// -2: invalid request
// -3: out of bounds

//TODO possible race condition with tasks
using asio::ip::tcp;

std::mutex m;
std::mutex updateQueue;

int lastID;
int numConnectedClients;

class Tile;

Logger logger;

using asio::ip::tcp;

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

//int lastID;
//int numConnectedClients;

//Logger logger;

SectorMap map;
FastNoise noiseGen;

void dispachTask(TaskType type, uint32_t target, SurfaceLocator loc, uint32_t id) {
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

void handleClient(tcp::socket sock) {
	std::unique_lock<std::mutex> uQ(updateQueue);
	uint32_t id = lastID++;
	numConnectedClients++;
	uQ.unlock();
    while (true) {
        asio::error_code error;
        asio::streambuf buf;
        /*size_t len = */asio::read_until(sock, buf, "\n", error);
        std::istream is(&buf);
        std::string request;
        std::getline(is, request);
        if (error && error != asio::error::eof) {
			//TODO warn instead
            throw asio::system_error(error);
        } else if (error && error == asio::error::eof) {
			uQ.lock();
			numConnectedClients--;
			uQ.unlock();
            return;
        }

        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value root;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            request.c_str(),
            request.c_str() + request.size(),
            &root,
            &errors
        );
        delete reader;

        if (!parsingSuccessful) {
            logger.warn("Client sent malformed JSON request: " + request + ". Full error: " + errors);
            asio::error_code ign_error;
            asio::write(sock, asio::buffer("{\"status\": -1}\n"), ign_error);
            continue;
        }

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
				/*
				PlanetSurface * surf = getSurfaceFromJson(requestJson);
				SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
				uint32_t target = requestJson["y"].asInt() * surf->rad * 2 + requestJson["x"].asInt();

				if (surf->stats.peopleIdle > 0) {
					surf->stats.peopleIdle--;
					int time = getTimeForTask((TaskType)requestJson["action"].asInt());
					dispachTask((TaskType)requestJson["action"].asInt(), target, loc, id);
					result["status"] = (int)ErrorCode::OK;
					result["time"] = time;
				} else {
					result["status"] = (int)ErrorCode::NO_PEOPLE_AVAILABLE;
					result["time"] = -1;
				}*/

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
}

void taskFinished(Task &t) {
    PlanetSurface * surf = getSurfaceFromLocator(t.surface);
    switch (t.type) {
        case TaskType::FELL_TREE:
            surf->tiles[t.target] = (surf->tiles[t.target] & 0xFFFFFFFF00000000) | (int)TileType::GRASS;
            
        case TaskType::GATHER_MINERALS:
            break;
        case TaskType::CLEAR:
            break;
	    case TaskType::PLANT_TREE:
	        break;
	    case TaskType::BUILD_HOUSE:
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

#include <sys/time.h>
#include <sys/resource.h>

int main() {

    const uint32_t LEVEL_SEED = 12345;
/*
    int x = 36;
    int y = 391;

    std::cout << mix(x, y) << ", " << hash(x) << ", " << hash(y) << "\n";

    return 0;*/

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();

    asio::io_context io_context;
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), 5555));
    std::thread taskThread(handleTasks);
    while (true) {
        std::thread(handleClient, a.accept()).detach();
    }
    threadStopped = true;
    taskThread.join();
}
