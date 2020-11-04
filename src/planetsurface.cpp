#include "planetsurface.h"
#include "planet.h"
#include "server.h"
#include "generation.h"

#include <jsoncpp/json/json.h>
#include <iostream>

PlanetSurface::PlanetSurface() {
    generated = false;
}

TileType PlanetSurface::getType(int r, int g, int b, int x, int y) {
	if (b > r * 2 && b * 1.2 > g) {
		return TileType::WATER;
	}
	if (rand() % 8 == 0) return TileType::ROCK;
	if (g > r && g > b * 1.5) {
        if (rand() % 5 == 0) return TileType::GRASS;
        double noise = (noiseGen.GetNoise((float)(x / this->noiseScl), (float)(y / this->noiseScl), noiseZ) + 1) / 2;
        if (noise < 0.2) return TileType::GRASS;
        if (noise < 0.3) return TileType::BUSH;
        if (noise < 0.5) return TileType::TREE;
        if (noise < 0.7) return TileType::FOREST;
        if (noise < 0.8) return TileType::PINE;
        if (noise < 1.0) return TileType::PINEFOREST;
	}
	return TileType::GRASS;
}

TileType PlanetSurface::getInitialTileType(int x, int y, Planet * p) {
	int xb = x - p->radius;
	int yb = y - p->radius;

	int r = 0;
	int g = 0;
	int b = 0;
	int total = 0;
	for (int i = 0; i < p->numColours; i++) {
		if ((noiseGen.GetNoise(xb / p->generationNoise[i], yb / p->generationNoise[i], p->generationZValues[i]) + 1) / 2 > p->generationChances[i]) {
			r += p->generationColours[i].r;
			g += p->generationColours[i].g;
			b += p->generationColours[i].b;
			total += 1;
		}
	}
	if (total == 0) {
		r = p->baseColour.r;
		g = p->baseColour.g;
		b = p->baseColour.b;
	} else {
		r /= total;
		g /= total;
		b /= total;
	}
	return getType(r, g, b, x, y);
}

void PlanetSurface::generate(Planet * p) {
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
    std::cout << this->noiseScl << "\n";

    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            int z;
            TileType type = getInitialTileType(i, j, p);
			if (type != TileType::WATER) {
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
            tiles.push_back(((uint64_t)z << 32) | (uint32_t)type);
        }
    }
    this->rad = p->radius;
    generated = true;
}

Json::Value PlanetSurface::asJson() {
    Json::Value res;
    for (unsigned int i = 0; i < tiles.size(); i++) {
        res["tiles"].append((Json::Value::UInt64)tiles[i]);
    }
    res["rad"] = rad;
    return res;
}
