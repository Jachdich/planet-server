#include "planet.h"
#include "server.h"

#include <jsoncpp/json/json.h>
#include "generation.h"
#include "planetsurface.h"

Planet::Planet() {}

Planet::Planet(SurfaceLocator loc, uint32_t sectorSeed) {
    this->sectorSeed = sectorSeed;
	this->mass = 0;
    this->theta = (rndInt(0, 360) / 180.0) * 3.14159265358979323;
    this->posFromStar = 0;
    this->radius = rndInt(genConf["p_radMin"].asInt(), genConf["p_radMax"].asInt());
    this->numColours = rndInt(genConf["p_numColoursMin"].asInt(), genConf["p_numColoursMax"].asInt()) + 1;
    this->seaLevel = rndInt(genConf["p_seaLevelMin"].asInt(), genConf["p_seaLevelMax"].asInt());
    this->generationChances = new double[this->numColours];
    this->generationColours = new Pixel[this->numColours];
    this->generationZValues = new int[this->numColours];
    this->generationNoise   = new double[this->numColours];
    
    for (int i = 0; i < this->numColours; i++) {
        this->generationChances[i] = rndDouble(genConf["p_genChanceMin"].asDouble(), genConf["p_genChanceMax"].asDouble());
        this->generationColours[i].rand();
        this->generationZValues[i] = rndInt(0, 1000000);
        this->generationNoise[i]   = rndDouble(genConf["p_genNoiseMin"].asDouble(), genConf["p_genNoiseMax"].asDouble());
        if (i == 0) {
            this->generationColours[0] = Pixel(0x202090); //b l u e
        }
    }

    this->baseColour.rand(genConf["p_baseColMin"].asInt() % 256, genConf["p_baseColMax"].asInt() % 256);

    this->surface = new PlanetSurface(loc);
}

void Planet::setPosFromStar(uint32_t pos) {
    this->posFromStar = pos;
    this->angularVelocity = 1.0 / (posFromStar * posFromStar) * genConf["p_angularVelMultiplier"].asDouble();
}

Planet::Planet(Json::Value res, SurfaceLocator loc, Planet *other_this) {
    mass = res["mass"].asDouble();
    radius = res["radius"].asInt();
    numColours = res["numColours"].asInt();
    int col = res["baseColour"].asInt();
    baseColour = Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    posFromStar = res["posFromStar"].asInt();
    theta = res["theta"].asDouble();
    angularVelocity = res["angularVelocity"].asDouble();
    owner = res["owner"].asUInt64();
    sectorSeed = res["sectorSeed"].asUInt();
    seaLevel = res["seaLevel"].asInt();

    this->generationChances = new double[this->numColours]; //TODO WILL NOT UNLOAD! VERY BAD IDEA
    this->generationColours = new Pixel[this->numColours];  //TODO FUCK THIS IS A FUCKING MEMORY LEAK COS NO DESTRUCTOR
    this->generationZValues = new int[this->numColours];    //WTF I THOUGHT I FIXED THIS
    this->generationNoise   = new double[this->numColours]; //Oh no, anyway

    for (int i = 0; i < numColours; i++) {
        int col = res["generationColours"][i].asInt();
        generationColours[i] = Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);

        generationChances[i] = res["generationChances"][i].asDouble();
        generationZValues[i] = res["generationZValues"][i].asInt();
        generationNoise[i]   = res["generationNoise"][i].asDouble();
    }
    if (res["surface"]["generated"].asBool()) {
        surface = new PlanetSurface(res["surface"], loc, other_this);
    } else {
        surface = new PlanetSurface(loc);
    }
}

PlanetSurface * Planet::getSurface() {
    if (!surface->generated) {
        surface->generate(this);
    }
    return surface;
}

Json::Value Planet::asJson() {
    Json::Value res;
    res["mass"] = mass;
    res["radius"] = radius;
    res["numColours"] = numColours;
    res["baseColour"] = baseColour.asInt();
    res["posFromStar"] = posFromStar;
    res["theta"] = theta;
    res["angularVelocity"] = angularVelocity;
    res["owner"] = (Json::UInt64)owner;
    res["sectorSeed"] = sectorSeed;
    res["seaLevel"] = seaLevel;

    for (int i = 0; i < numColours; i++) {
        res["generationColours"].append(generationColours[i].asInt());
        res["generationChances"].append(generationChances[i]);
        res["generationZValues"].append(generationZValues[i]);
        res["generationNoise"].append(generationNoise[i]);
    }

    if (surface->generated) {
        res["surface"] = surface->asJson();
    } else {
        res["surface"]["generated"] = false;
    }

    return res;
}
