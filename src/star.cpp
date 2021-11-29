#include "star.h"

#include <jsoncpp/json/json.h>
#include <iostream>
#include <math.h>
#include "generation.h"
#include "planet.h"

//This is not my function lol
uint32_t kToRGB(uint32_t k) {
    float temp = k / 100.0;
    int16_t r, g, b;

    if (temp <= 66) {
    	r = 255;
    } else {
    	r = temp - 60;
    	r = 329.698727446 * pow(r, -0.1332047592);
    	if (r < 0) r = 0;
    	if (r > 255) r = 255;
    }
        
    if (temp <= 66) {
    	g = temp;
    	g = 99.4708025861 * log(g) - 161.1195681661;
    	if (g < 0) g = 0;
    	if (g > 255) g = 255;
    } else {
    	g = temp - 60;
    	g = 288.1221695283 * pow(g, -0.0755148492);
    	if (g < 0) g = 0;
    	if (g > 255) g = 255;
    }
    
    if (temp >= 66) {
    	b = 255;
    } else {
    	if (temp <= 19) {
    		b = 0;
    	} else {
    		b = temp - 10;
    		b = 138.5177312231 * log(b) - 305.0447927307;
    		if (b < 0) b = 0;
    		if (b > 255) b = 255;
    	}
    }

    return (r & 0xff) << 16 | (g & 0xff) << 8 | (b & 0xff);
}


Star::Star() {}

Star::Star(uint32_t x, uint32_t y, SurfaceLocator loc, uint32_t sectorSeed) {
    this->x = x;
    this->y = y;

    this->num = rndInt(genConf["s_numPlanetsMin"].asInt(), genConf["s_numPlanetsMax"].asInt());
    this->radius = rndInt(genConf["s_radMin"].asInt(), genConf["s_radMax"].asInt());
    uint32_t temp = rndInt(genConf["s_tempMin"].asInt(), genConf["s_tempMax"].asInt());
    this->colour = kToRGB(temp);

    noiseScl    = rndDouble(genConf["s_noiseSclMin"].asDouble(), genConf["s_noiseSclMax"].asDouble());
    noiseEffect = rndDouble(genConf["s_noiseEffectMin"].asDouble(), genConf["s_noiseEffectMax"].asDouble());
    noiseZ = rndDouble(0, 100000); //just anything big for a random place in 3d noise

    this->planets = new Planet[this->num];
    uint32_t lastPosFromStar = rand() % 100 + this->radius * 6 + 20;
    for (uint32_t i = 0; i < this->num; i++) {
    	loc.planetPos = i;
        this->planets[i] = Planet(loc, sectorSeed);
        lastPosFromStar += this->planets[i].radius * 2;
    	this->planets[i].setPosFromStar(lastPosFromStar);
    	lastPosFromStar += rand() % 100;
    }
}

Star::Star(Json::Value root, SurfaceLocator loc) {
	x = root["x"].asInt();
    y = root["y"].asInt();
    num = root["num"].asInt();
    int col = root["colour"].asInt();
    colour = col;
    radius = root["radius"].asInt();
	this->planets = new Planet[this->num];
    for (uint32_t i = 0; i < num; i++) {
    	loc.planetPos = i;
        planets[i] = Planet(root["planets"][i], loc, &planets[i]); //WTF?
    }
    
    noiseScl = root["noiseScl"].asDouble();
    noiseZ = root["noiseZ"].asDouble();
    noiseEffect = root["noiseEffect"].asDouble();
}

Json::Value Star::asJson() {
    Json::Value res;
    res["x"] = x;
    res["y"] = y;
    res["num"] = num;
    res["colour"] = colour;
    res["radius"] = radius;
    res["noiseZ"] = noiseZ;
    res["noiseScl"] = noiseScl;
    res["noiseEffect"] = noiseEffect;
    for (uint32_t i = 0; i < this->num; i++) {
        res["planets"].append(planets[i].asJson());
    }
    return res;
}
