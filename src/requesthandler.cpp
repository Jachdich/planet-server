#include "../include/network.h"
#include "../include/serverinterface.h"
#include "../include/server.h"
#include "../include/common/enums.h"
#include "../include/planet.h"
#include "../include/star.h"
#include "../include/sector.h"
#include "../include/sectormap.h"
#include "../include/user.h"
#include "../include/common/surfacelocator_test.h"
#define bade nullptr

std::mutex m;

int lastID;
int numConnectedClients;

SectorMap map;
FastNoiseLite noiseGen;

void sendTileErrorSetRequest(const SurfaceLocator *loc, uint32_t index, std::string err) {
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

void sendResourcesChangeRequest(const Resources *resources, const SurfaceLocator *loc) {
	Json::Value root;
	getJsonFromSurfaceLocator(loc, root);
	root["resources"] = res_to_json(resources);
	PlanetSurface * s = getSurfaceFromLocator(loc);
	root["serverRequest"] = "statsChange";
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
}

void sendTileChangeRequest(uint32_t pos, TileType type, const SurfaceLocator *loc) {
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
    root["type"] = type;
    root["serverRequest"] = "changeTile";
    getJsonFromSurfaceLocator(loc, root);
    for (Connection *conn: s->connectedClients) {
        conn->sendMessage(root);
    }
}

void sendSetTimerRequest(double time, uint32_t target, const SurfaceLocator *loc) {
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
            result["status"] = ERR_OK;
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
                getJsonFromSurfaceLocator(&t.surface, totalJson);
            }
            if (surf != bade) {
	            result["result"] = surf->asJson(true);
	            result["status"] = ERR_OK;
	            Sector * sec = map.getSectorAt(requestJson["secX"].asInt(), requestJson["secT"].asInt());
	            sec->save(saveName);
            } else {
	            result["status"] = ERR_OUT_OF_BOUNDS;
            }

            totalJson["results"].append(result);
        } else if (req == "unloadSurface") {
            PlanetSurface * surf = getSurfaceFromJson(requestJson);
            surf->connectedClients.erase(std::remove(surf->connectedClients.begin(), surf->connectedClients.end(), this), surf->connectedClients.end()); 
            surfacesLoaded.erase(std::remove(surfacesLoaded.begin(), surfacesLoaded.end(), surf), surfacesLoaded.end()); 
        } else if (req == "userAction") {
            Json::Value result;

            //if logged in
            if (uuid != 0) {
                PlanetSurface * surf = getSurfaceFromJson(requestJson);
                //TODO check they have perms in the surface
                SurfaceLocator loc = getSurfaceLocatorFromJson(requestJson);
                uint32_t target = requestJson["y"].asInt() * surf->rad * 2 + requestJson["x"].asInt();
    
                ErrorCode code = dispachTask((TaskType)requestJson["action"].asInt(), target, loc, surf);
            	result["status"] = code.type;
            	if (code.type != ERR_OK) {
                    result["error_message"] = code.message;
            	}
            } else {
                result["status"] = ERR_NOT_AUTHENTICATED;
            }

            totalJson["results"].append(result);

        } else if (req == "login") {
            std::string username = requestJson["username"].asString();
            std::string password = requestJson["password"].asString();

            //does the username exist?
            if (iface->nameToUUID.find(username) != iface->nameToUUID.end()) {
                this->uuid = iface->nameToUUID[username];
            }
            
            //check if account doesn't exist
            if (uuid == 0 || iface->accounts.find(uuid) == iface->accounts.end()) {
                UserMetadata user(username, password);
                uuid = user.uuid;
                iface->accounts[uuid] = user;
                iface->nameToUUID[username] = uuid;
            }

            Json::Value res;

            UserMetadata &meta = iface->accounts[uuid];
            if (!meta.isPasswordCorrect(password)) {
                //Make it clear that we're not logged in
                uuid = 0;
                res["status"] = ERR_INVALID_CREDENTIALS;
            } else {
                res["status"] = ERR_OK;
            }
            totalJson["results"].append(res);

        } else {
            logger.warn("Client sent invalid request: " + root.get("request", "NULL").asString());
            Json::Value result;
            result["status"] = ERR_INVALID_REQUEST;
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
    iface->connections.erase(std::remove(iface->connections.begin(), iface->connections.end(), shared_from_this()), iface->connections.end()); 

}
