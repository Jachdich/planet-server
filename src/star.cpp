#include "star.h"

#include <jsoncpp/json/json.h>
#include <iostream>
#include "generation.h"
#include "planet.h"

Star::Star() {}

Star::Star(uint32_t x, uint32_t y, SurfaceLocator loc) {
    this->x = x;
    this->y = y;

    this->num = rand() % 4 + 2;
    this->radius = rand() % 7 + 5;
    this->colour = Pixel(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);

    this->planets = new Planet[this->num];
    uint32_t lastPosFromStar = rand() % 100 + this->radius * 6 + 20;
    for (uint32_t i = 0; i < this->num; i++) {
    	loc.planetPos = i;
    	uint32_t planet_rad = 0;
        this->planets[i] = Planet(loc);
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
    colour = Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    radius = root["radius"].asInt();
	this->planets = new Planet[this->num];
    for (uint32_t i = 0; i < num; i++) {
    	loc.planetPos = i;
        planets[i] = Planet(root["planets"][i], loc);
    }
}

Json::Value Star::asJson() {
    Json::Value res;
    res["x"] = x;
    res["y"] = y;
    res["num"] = num;
    res["colour"] = colour.asInt();
    res["radius"] = radius;
    for (uint32_t i = 0; i < this->num; i++) {
        res["planets"].append(planets[i].asJson());
    }
    return res;
}
