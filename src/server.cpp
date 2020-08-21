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

//return codes
// -1: malformed JSON
// -2: invalid request
// -3: out of bounds

using asio::ip::tcp;

std::mutex m;
std::mutex updateQueue;

int lastID;
int numConnectedClients;

Logger logger;

SectorMap map;
FastNoise noiseGen;

struct Update {
	Json::Value value;
	std::vector<int> readBy;
	int numRead = 1; //1 because the thread that instantiated the object has read it
	Update(Json::Value v, int id) {
		value = v;
		readBy.push_back(id);
	}
};

PlanetSurface * getSurfaceFromJson(Json::Value root) {
	SurfaceLocator loc = getSurfaceLocatorFromJson(root);
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

std::vector<Update> updates;

void handleClient(tcp::socket sock) {
	std::unique_lock<std::mutex> uQ(updateQueue);
	int id = lastID++;
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

			} else if (req == "changeTile") {
                Json::Value result;
                PlanetSurface * surf = getSurfaceFromJson(requestJson);

				if (surf != nullptr) {
					int x, y;
					x = requestJson.get("x", -1).asInt();
					y = requestJson.get("y", -1).asInt();
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

#include <sys/time.h>
#include <sys/resource.h>

int main() {
    const unsigned int LEVEL_SEED = 12345;
    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();

    asio::io_context io_context;
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), 5555));
    while (true) {
        std::thread(handleClient, a.accept()).detach();
    }
}
