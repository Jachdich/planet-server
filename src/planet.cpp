#include "planet.h"

#include <jsoncpp/json/json.h>
#include "generation.h"
#include "planetsurface.h"

Planet::Planet() {}

Planet::Planet(int posFromStar) {
	this->mass = 0;
    this->theta = (rndInt(0, 360) / 180.0) * 3.14159265358979323;
    this->posFromStar = posFromStar;
    this->radius = rndInt(genConf["p_radMin"].asInt(), genConf["p_radMax"].asInt());
    this->numColours = (this->radius - 5) / (genConf["p_radMax"].asInt() - 5.0) * 3;

	if (this->numColours == 0) {
		this->generationChances = new double[1];
	    this->generationColours = new Pixel[1];
	    this->generationZValues = new int[1];
	    this->generationNoise   = new double[1];
		this->generationChances[0] = rndDouble(genConf["p_genChanceMin"].asDouble(), genConf["p_genChanceMax"].asDouble());
        this->generationColours[0].rand();
        this->generationZValues[0] = rndInt(0, 1000000);
        this->generationNoise[0]   = rndDouble(genConf["p_genNoiseMin"].asDouble(), genConf["p_genNoiseMax"].asDouble());
	} else {
	    this->generationChances = new double[this->numColours];
	    this->generationColours = new Pixel[this->numColours];
	    this->generationZValues = new int[this->numColours];
	    this->generationNoise   = new double[this->numColours];

	    for (int i = 0; i < this->numColours; i++) {
	        this->generationChances[i] = rndDouble(genConf["p_genChanceMin"].asDouble(), genConf["p_genChanceMax"].asDouble());
	        this->generationColours[i].rand();
	        this->generationZValues[i] = rndInt(0, 1000000);
	        this->generationNoise[i]   = rndDouble(genConf["p_genNoiseMin"].asDouble(), genConf["p_genNoiseMax"].asDouble());
	    }
	}

    this->baseColour.rand(genConf["p_baseColMin"].asInt() % 256, genConf["p_baseColMax"].asInt() % 256);
    this->angularVelocity = 1.0 / (posFromStar * posFromStar) * genConf["p_angularVelMultiplier"].asDouble();

    this->surface = new PlanetSurface();
}

Planet::Planet(Json::Value res) {
    mass = res["mass"].asDouble();
    radius = res["radius"].asInt();
    numColours = res["numColours"].asInt();
    int col = res["baseColour"].asInt();
    baseColour = Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    posFromStar = res["posFromStar"].asInt();
    theta = res["theta"].asDouble();
    angularVelocity = res["angularVelocity"].asDouble();

    this->generationChances = new double[this->numColours];
    this->generationColours = new Pixel[this->numColours];
    this->generationZValues = new int[this->numColours];
    this->generationNoise   = new double[this->numColours];

    for (int i = 0; i < numColours; i++) {
        int col = res["generationColours"][i].asInt();
        generationColours[i] = Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);

        generationChances[i] = res["generationChances"][i].asDouble();
        generationZValues[i] = res["generationZValues"][i].asInt();
        generationNoise[i]   = res["generationNoise"][i].asDouble();
    }
    surface = new PlanetSurface();
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

    for (int i = 0; i < numColours; i++) {
        res["generationColours"].append(generationColours[i].asInt());
        res["generationChances"].append(generationChances[i]);
        res["generationZValues"].append(generationZValues[i]);
        res["generationNoise"].append(generationNoise[i]);
    }

    return res;
}
