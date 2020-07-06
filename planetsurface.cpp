#include "planetsurface.h"
#include "planet.h"
#include "server.h"

#include <jsoncpp/json/json.h>

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
}

void PlanetSurface::generate(Planet * p) {
    generated = true;
    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            tiles.push_back(getInitialTileType(j, i, p));
        }
    }
    this->rad = p->radius;
}

Json::Value PlanetSurface::asJson() {
    Json::Value res;
    for (int i = 0; i < tiles.size(); i++) {
        res["tiles"].append(tiles[i]);
    }
    res["rad"] = rad;
    return res;
}
