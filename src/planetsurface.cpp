#include "planetsurface.h"
#include "planet.h"
#include "server.h"
#include "network.h"
#include "generation.h"
//#include "common/"

#include <jsoncpp/json/json.h>
#include <iostream>

PlanetSurface::PlanetSurface(SurfaceLocator loc) {
    generated = false;
    this->loc = loc;
}

Tile* PlanetSurface::getType(uint8_t r, uint8_t g, uint8_t b, int32_t x, int32_t y) {
	if (b > r * 2 && b * 1.2 > g) {
		return new WaterTile();
	}
	if (rand() % 8 == 0) return new RockTile();
	if (g > r && g > b * 1.5) {
        if (rand() % 5 == 0) return new GrassTile();
        double noise = (noiseGen.GetNoise((float)(x / this->noiseScl), (float)(y / this->noiseScl), noiseZ) + 1) / 2;
        if (noise < 0.2) return new GrassTile();
        if (noise < 0.3) return new BushTile();
        if (noise < 0.5) return new TreeTile();
        if (noise < 0.7) return new ForestTile();
        if (noise < 0.8) return new PineTile();
        if (noise < 1.0) return new PineforestTile();
	}
	return new GrassTile();
}

uint32_t PlanetSurface::getTileColour(int32_t x, int32_t y) {
    int32_t xb = x - parent->radius;
	int32_t yb = y - parent->radius;

	uint32_t r = 0;
	uint32_t g = 0;
	uint32_t b = 0;
	uint32_t total = 0;
	for (int i = 0; i < parent->numColours; i++) {
		if ((noiseGen.GetNoise(xb / parent->generationNoise[i], yb / parent->generationNoise[i], parent->generationZValues[i]) + 1) / 2 > parent->generationChances[i]) {
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
    this->resources["people"] = 4;
    this->resources["peopleIdle"] = 4;
    this->resources["food"] = 1000;
    this->resources["water"] = 1000;
    int pos = -1;
    for (int i = 0; i < p->numColours; i++) {
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
    }
    this->noiseZ = genZVal;
    this->noiseScl = rndDouble(genConf["p_genNoisePlantsMin"].asDouble(), genConf["p_genNoisePlantsMax"].asDouble());

    this->rad = p->radius;
    tiles.resize((rad * 2) * (rad * 2));
    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            int z;
            Tile* tile = getInitialTileType(j, i);
			if (tile->getType() != TileType::WATER) {
				int xb = i - p->radius;
				int yb = j - p->radius;
				float az = (1 - (noiseGen.GetNoise(xb / genNoise, yb / genNoise, genZVal) + 1) / 2) - (1 - genChance);
				z = az * 30;
				if (z < 0) {
					z = -z;
				}
			} else {
				z = -1;
			}
			tile->z = z;
            tiles[j * (rad * 2) + i] = tile;
        }
    }
    generated = true;
    
}

void PlanetSurface::tick(double elapsedTime) {
    if (lastTicks == -1) {
        lastTicks = ticks - 1;
    }
	uint64_t deltaTicks = ticks - lastTicks;

	Resources originalResources = resources.clone();

    for (uint64_t i = 0; i < deltaTicks; i++) {
        uint64_t tileTicks = lastTicks + i;
    	resources["peopleSlots"] = 0;
    	for (uint16_t y = 0; y < rad * 2; y++) {
    		for (uint16_t x = 0; x < rad * 2; x++) {
    	    	uint32_t index = (y * rad * 2) + x;
    	        tiles[index]->tick(tileTicks, olc::vi2d(x, y), this);
    	    }
    	}
    	//DEBUG
        if (resources["peopleSlots"] < 4) resources["peopleSlots"] = 4;
    	///DEBUG

        //enough places for people to live?
    	if (resources["people"] < resources["peopleSlots"] && resources["food"] > 0) {
    	    //r e p r o d u c e
    	    if (rndDouble(0.0, 1.0) > 0.9) {
    			resources["people"] += 1;
    			resources["peopleIdle"] += 1;
    		}
    	}

        //not enough places to live?
    	if (resources["people"] > resources["peopleSlots"]) {
    	    //d i e
            uint32_t delta = resources["people"] - resources["peopleSlots"];
    	    resources["people"] = resources["peopleSlots"];
    	    
    	    if (((int32_t)resources["peopleIdle"]) - delta < 0) resources["peopleIdle"] = 0;
    	    else resources["peopleIdle"] -= delta;
    	}

        //not enough food or water?
    	if (resources["food"] <= 0 || resources["water"] <= 0) {
            //d i e
            if (resources["food"] < 0) resources["food"] = 0;
            if (resources["water"] < 0) resources["water"] = 0;
            if (resources["people"] > 0) resources["people"] -= 1; //TODO this is based off of the tickrate!
            if (resources["peopleIdle"] > 0) resources["peopleIdle"] -= 1;
    	}


    	resources["food"]  -= 0.1 * resources["people"];
    	resources["water"] -= 0.1 * resources["people"];
    	std::cout << resources["peopleSlots"] << ", " << resources["people"] << ", " << resources["food"] << ", " << resources["water"] << ", " << "\n";
	}

 	if (resources != originalResources || ticks % 100 == 0) {
 	    //ticks % 100 is to just make sure it is updated every now and then in case it gets out of sync for any reason
		sendResourcesChangeRequest(resources, loc);
	}
	lastTicks = ticks;
}

PlanetSurface::PlanetSurface(Json::Value root, SurfaceLocator loc) {
    rad = root["rad"].asInt();
    tiles.resize((rad * 2) * (rad * 2));
    resources = getResourcesFromJson(root["resources"]);
    for (uint32_t i = 0; i < (rad * 2) * (rad * 2); i++) {
        uint64_t tile = root["tiles"][i].asUInt64();
        uint32_t z = (tile & 0xFFFFFFFF00000000) >> 32;
        TileType type = (TileType)(tile & 0xFFFFFFFF);
        tiles[i] = Tile::fromType(type);
        tiles[i]->z = z;
    }
    generated = true;
    this->loc = loc;
}

Json::Value PlanetSurface::asJson() {
    Json::Value res;
    for (unsigned int i = 0; i < tiles.size(); i++) {
        uint64_t n = (uint32_t)tiles[i]->getType() | ((uint64_t)tiles[i]->z << 32);
        res["tiles"].append((Json::Value::UInt64)n);
    }
    res["rad"] = rad;
    res["resources"] = getJsonFromResources(resources);
    res["generated"] = true;
    
    return res;
}
