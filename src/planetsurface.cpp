#include "planetsurface.h"
#include "planet.h"
#include "server.h"

#include <jsoncpp/json/json.h>
#include <iostream>

PlanetSurface::PlanetSurface() {
    generated = false;
}

int PlanetSurface::getType(int r, int g, int b) {
	if (g > r && g > b * 1.5) { //foliage
		return 1;
	}
	if (b > r * 2 && b * 1.2 > g) {
		return 2; //water
	}
	return 0; //default land tile
}

int PlanetSurface::getInitialTileType(int x, int y, Planet * p) {
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
	int tileType = getType(r, g, b);
	switch (tileType) {
		case 0:
			return (rand() % 6 == 0) ? 3 : 0; //random chance of having a rock
		case 1:
			if (rand() % 2 == 0) {
				return (rand() % 6 == 0) ? 3 : 0; //random chance of having a rock
			}
			return 1; //just tree
		case 2:
			return 2;
	}
	return -1;
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

    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            int z;
            int type = getInitialTileType(i, j, p);
			if (type != 2) {
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
            tiles.push_back(((uint64_t)z << 32) | type);
        }
    }
    this->rad = p->radius;
    generated = true;
}

Json::Value PlanetSurface::asJson() {
    Json::Value res;
    for (int i = 0; i < tiles.size(); i++) {
        res["tiles"].append((Json::Value::UInt64)tiles[i]);
    }
    res["rad"] = rad;
    return res;
}
