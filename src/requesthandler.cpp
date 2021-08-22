#include "network.h"
#include "server.h"
#include "common/enums.h"
#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectormap.h"
#include "common/surfacelocator_test.h"
#define bade nullptr

std::mutex m;

int lastID;
int numConnectedClients;

SectorMap map;
FastNoise noiseGen;

void sendTileErrorSetRequest(SurfaceLocator loc, uint32_t index, std::string err) {
    Json::Value root;
    Json::Value tileError;
	getJsonFromSurfaceLocator(loc, root);
    root["serverRequest"] = "updateTileError";
    tileError["pos"] = index;
    tileError["msg"] = err;
    root["tileError"] = tileError;
	PlanetSurface * s = getSurfaceFromLocator(loc);
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
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
    olc::vi2d vpos;
    vpos.x = pos % (s->rad * 2);
    vpos.y = pos / (s->rad * 2);
    s->tiles[pos]->onPlace(ticks, vpos, s);
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
            for (Task &t : surf->tasks) {
                totalJson["serverRequest"] = "setTimer";
                totalJson["time"] = t.timeLeft;
                totalJson["tile"] = t.target;
                getJsonFromSurfaceLocator(t.surface, totalJson);
            }
            if (surf != bade) {
	            result["result"] = surf->asJson(true);
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

            ErrorCode code = dispachTask((TaskType)requestJson["action"].asInt(), target, loc, surf);
        	result["status"] = (int)code.type;
        	if (code.type != ErrorCode::OK) {
                result["error_message"] = code.message;
        	}
        	
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
	logger.info("Client disconnected");
    for (PlanetSurface *surf : surfacesLoaded) {
        surf->connectedClients.erase(std::remove(surf->connectedClients.begin(), surf->connectedClients.end(), this), surf->connectedClients.end()); 
    }
    iface.connections.erase(std::remove(iface.connections.begin(), iface.connections.end(), shared_from_this()), iface.connections.end()); 

}
