#include "planetsurface.h"
#include "planet.h"
#include "server.h"
#include "network.h"
#include "generation.h"

#include <jsoncpp/json/json.h>
#include <iostream>
#include <stdio.h>

PlanetSurface::PlanetSurface(SurfaceLocator loc) {
    generated = false;
    this->loc = loc;
}

Tile* PlanetSurface::getType(uint8_t r, uint8_t g, uint8_t b, int32_t x, int32_t y) {
	//if (b > r * 2 && b * 1.2 > g) {
	//	return new WaterTile();
	//}

	if (getHeight(x, y) <= parent->seaLevel) {
	    return new WaterTile();
	}
	if (rand() % 10 == 0) return new RockTile();
	if (g > r && g > b * 1.5) {
        if (rand() % 3 == 0) return new GrassTile();
        double noise = (noiseGen.GetNoise((double)(x / this->noiseScl), (double)(y / this->noiseScl), (double)noiseZ) + 1) / 2;
        if (noise < 0.2) return new GrassTile();
        if (noise < 0.3) return new BushTile();
        if (noise < 0.5) return new TreeTile();
        if (noise < 0.7) return new ForestTile();
        if (noise < 0.8) return new PineTile();
        if (noise < 1.0) return new PineforestTile();
	}
	return new GrassTile();
}

int32_t PlanetSurface::getHeight(int32_t x, int32_t y) {
    double xb = x - this->rad;
	double yb = y - this->rad;
	double noise = noiseGen.GetNoise(xb / parent->generationNoise[0], yb / parent->generationNoise[0], (double)parent->generationZValues[0]);
	int32_t height = noise *= 30;
	if (height < parent->seaLevel) height = parent->seaLevel;
	//std::cout << "[" << xb << "," << yb << "," << noise << "]\n";
	return height;
}

uint32_t PlanetSurface::getTileColour(int32_t x, int32_t y) {
    if (getHeight(x, y) <= parent->seaLevel) {
        //water, return b l u e
        return parent->generationColours[0].asInt();
    }
    int32_t xb = x - parent->radius;
	int32_t yb = y - parent->radius;

	uint32_t r = 0;
	uint32_t g = 0;
	uint32_t b = 0;
	uint32_t total = 0;
	for (int i = 1; i < parent->numColours; i++) {
		if ((noiseGen.GetNoise(xb / parent->generationNoise[i], yb / parent->generationNoise[i], (double)parent->generationZValues[i]) + 1) / 2 > parent->generationChances[i]) {
			r += parent->generationColours[i].r;
			g += parent->generationColours[i].g;
			b += parent->generationColours[i].b;
			total += 1;
		}
	}
	if (total == 0) {
		r = parent->baseColour.r;
		g = parent->baseColour.g;
		b = parent->baseColour.b;
	} else {
		r /= total;
		g /= total;
		b /= total;
	}
	return ((uint8_t)r << 16) | ((uint8_t)g << 8) | (uint8_t)b;
}

Tile* PlanetSurface::getInitialTileType(int32_t x, int32_t y) {
    uint32_t colour = getTileColour(x, y);
    uint8_t r = colour >> 16;
    uint8_t g = (colour >> 8) & 0xFF;
    uint8_t b = colour & 0xFF;
	return getType(r, g, b, x, y);
}

void PlanetSurface::generate(Planet * p) {
    this->parent = p;

    int seed = p->sectorSeed + hash(p->posFromStar);
    srand(seed);
    int pos = -1;
    /*for (int i = 0; i < p->numColours; i++) {
        Pixel c = p->generationColours[i];
        if (c.b > c.r * 2 && c.b * 1.2 > c.g) {
            pos = i;
            break;
        }
    }

    double genNoise;
    int genZVal;
    double genChance;
    if (pos != -1) {
        genNoise  = p->generationNoise[pos];
        genZVal   = p->generationZValues[pos];
        genChance = p->generationChances[pos];
    } else {
        genNoise  = p->generationNoise[0];
        genZVal   = p->generationZValues[0];
        genChance = p->generationChances[0];
    }*/
    this->noiseZ = rand();//genZVal;
    //this->noiseScl = rndDouble(genConf["p_genNoisePlantsMin"].asDouble(), genConf["p_genNoisePlantsMax"].asDouble());
    this->noiseScl = rndDouble(genConf["p_genNoiseMin"].asDouble(), genConf["p_genNoiseMax"].asDouble());
    

    this->rad = p->radius;
    tiles.resize((rad * 2) * (rad * 2));
    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            int z;
            Tile* tile = getInitialTileType(j, i);
			//if (tile->getType() != TILE_WATER) {
				//int xb = i - p->radius;
				//int yb = j - p->radius;
				//float az = (1 - (noiseGen.GetNoise(xb / genNoise, yb / genNoise, genZVal) + 1) / 2) - (1 - genChance);
				//z = az * 30;
				//if (z < 0) {
				//	z = -z;
				//}
			//} else {
			//	z = -1;
			//}
			z = getHeight(j, i);
			tile->z = z;
            tiles[j * (rad * 2) + i] = tile;
        }
    }
    generated = true;
    
}

void PlanetSurface::resetPeopleIdle() {
    resources.values[RES_PEOPLE_IDLE].value = resources.values[RES_PEOPLE].value;
}

void PlanetSurface::tick(double elapsedTime) {
    if (lastTicks == (uint32_t)-1) {
        lastTicks = ticks - 1; //TODO save and load lastTicks and also shouldn't this be set to zero?
    }
	uint64_t deltaTicks = ticks - lastTicks;

	Resources originalResources = res_clone(&resources);

	//this bit is, ironically, the road tick function
    std::vector<Tile*> foundTiles;
    olc::vi2d pos = olc::vi2d{0, 0};


    //TODO cache this lol
    for (uint16_t y = 0; y < rad * 2; y++) {
        for (uint16_t x = 0; x < rad * 2; x++) {
            uint32_t index = (y * rad * 2) + x;
            if (tiles[index]->getType() == TILE_CAPSULE) {
                pos = olc::vi2d{x, y};
                break;
            }
        }
    }

    for (olc::vi2d offset : {olc::vi2d{1, 0}, olc::vi2d{-1, 0}, olc::vi2d{0, 1}, olc::vi2d{0, -1}}) {
        if (typeAt(pos + offset, this) == TILE_ROAD) {
            std::vector<Tile*> newTiles = countTiles(pos + olc::vi2d{1, 0}, this, TILE_ROAD);
            foundTiles.insert(foundTiles.end(), newTiles.begin(), newTiles.end());
        }
    }

    for (uint64_t i = 0; i < deltaTicks; i++) {
        uint64_t tileTicks = lastTicks + i;
    	resetPeopleIdle();

    	for (Task &t : tasks) {
            t.timeLeft -= elapsedTime;
            if (taskTypeInfos[t.type].requiresPeople) {
                resources.values[RES_PEOPLE_IDLE].value--;
            }
            if (t.timeLeft <= 0) {
                taskFinished(t, this);
            }
        }

        tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
    	[](Task& t) {
    		return t.timeLeft <= 0;
    	}), tasks.end());
    	for (uint32_t i = 0; i < NUM_RESOURCES; i++) {
            resources.values[i].capacity = 0;
    	}
    	for (uint16_t y = 0; y < rad * 2; y++) {
    		for (uint16_t x = 0; x < rad * 2; x++) {
    	    	uint32_t index = (y * rad * 2) + x;
    	        tiles[index]->tick(tileTicks, olc::vi2d(x, y), this, std::find(foundTiles.begin(), foundTiles.end(), tiles[index]) != foundTiles.end());
    	        std::string err = tiles[index]->getTileError();
    	        //if new error to send to client
    	        if (tiles[index]->edge) { 
                    sendTileErrorSetRequest(&loc, index, err);
    	        }
    	    }
    	}
        //enough places for people to live?
    	if (resources.values[RES_PEOPLE].value < resources.values[RES_PEOPLE].capacity && resources.values[RES_FOOD].value > 0 && resources.values[RES_PEOPLE].value > 0) {
    	    DEBUG("Possibility of reproduction");
    	    //r e p r o d u c e
    	    if (rndDouble(0.0, 1.0) > 0.9) {
    			resources.values[RES_PEOPLE].value += 1;
    			resources.values[RES_PEOPLE_IDLE].value += 1;
    		}
    	}

        //not enough places to live?
    	if (resources.values[RES_PEOPLE].value > resources.values[RES_PEOPLE].capacity) {
    	    DEBUG("Not enough houses");
    	    //d i e
            uint32_t delta = resources.values[RES_PEOPLE].value - resources.values[RES_PEOPLE].capacity;
    	    resources.values[RES_PEOPLE].value = resources.values[RES_PEOPLE].capacity;
    	    
    	    if (((int32_t)resources.values[RES_PEOPLE_IDLE].value) - delta < 0) resources.values[RES_PEOPLE].capacity = 0;
    	    else resources.values[RES_PEOPLE_IDLE].value -= delta;
    	}

        //not enough food or water?
    	if (resources.values[RES_FOOD].value <= 0 || resources.values[RES_WATER].value <= 0) {
    	    DEBUG("Not enough food or water");
            //d i e
            if (resources.values[RES_FOOD].value < 0) resources.values[RES_FOOD].value = 0;
            if (resources.values[RES_WATER].value < 0) resources.values[RES_WATER].value = 0;
            if (resources.values[RES_PEOPLE].value > 0) resources.values[RES_PEOPLE].value -= 1;
            if (resources.values[RES_PEOPLE_IDLE].value > 0) resources.values[RES_PEOPLE_IDLE].value -= 1;
    	}


    	resources.values[RES_FOOD].value  -= 0.1 * resources.values[RES_PEOPLE].value;
    	resources.values[RES_WATER].value -= 0.1 * resources.values[RES_PEOPLE].value;
    	DEBUG("People slots, people, food, water tick: " + 
    	    std::to_string(resources.values[RES_PEOPLE].capacity) + ", " +
    	    std::to_string(resources.values[RES_PEOPLE].value) + ", " +
    	    std::to_string(resources.values[RES_FOOD].value) + ", " +
    	    std::to_string(resources.values[RES_WATER].value) + ", " +
    	    std::to_string(tileTicks));
        for (uint32_t i = 0; i < NUM_RESOURCES; i++) {
            if (resources.values[i].value > resources.values[i].capacity) {
                if (i == RES_PEOPLE_IDLE) continue; //I think, probably best not to do that?
                DEBUG("Too much " + std::string(res_names[i]));
                resources.values[i].value = resources.values[i].capacity;
            }
        }
        //std::cout << "\n";
	}

 	if (res_ne(&resources, &originalResources) || ticks % 100 == 0) {
 	    //ticks % 100 is to just make sure it is updated every now and then
 	    //in case it gets out of sync for any godforsaken reason
		sendResourcesChangeRequest(&resources, &loc);
	}
	lastTicks = ticks;
}

PlanetSurface::PlanetSurface(Json::Value root, SurfaceLocator loc, Planet *parent) {
    rad = root["rad"].asInt();
    tiles.resize((rad * 2) * (rad * 2));
    resources = res_from_json(root["resources"]);
    this->parent = parent;
    for (uint32_t i = 0; i < (rad * 2) * (rad * 2); i++) {
        uint64_t tile = root["tiles"][i].asUInt64();
        uint32_t z = (tile & 0xFFFFFFFF00000000) >> 32;
        TileType type = (TileType)(tile & 0xFFFFFFFF);
        tiles[i] = Tile::fromType(type);
        tiles[i]->z = z;
    }

    for (Json::Value task : root["tasks"]) {
        tasks.push_back({
            (TaskType)task["type"].asInt(),
            task["target"].asUInt(),
            loc,
            task["timeLeft"].asDouble()
        });
    }
    generated = true;
    this->loc = loc;
}

Json::Value PlanetSurface::asJson(bool addErrors) {
    Json::Value res;
    for (unsigned int i = 0; i < tiles.size(); i++) {
        uint64_t n = (uint32_t)tiles[i]->getType() | ((uint64_t)tiles[i]->z << 32);
        res["tiles"].append((Json::Value::UInt64)n);
        if (addErrors && tiles[i]->getTileError() != "") {
            Json::Value err;
            err["pos"] = i;
            err["msg"] = tiles[i]->getTileError();
            res["tileErrors"].append(err);
        }
    }
    for (Task& t : tasks) {
        Json::Value value;
        value["type"] = (int)t.type;
        value["target"] = t.target;
        value["timeLeft"] = t.timeLeft;
        res["tasks"].append(value);
    }
    res["rad"] = rad;
    res["resources"] = res_to_json(&resources);
    res["generated"] = true;
    
    return res;
}
