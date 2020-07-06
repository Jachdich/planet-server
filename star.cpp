#include "star.h"

#include <jsoncpp/json/json.h>
#include "generation.h"
#include "planet.h"

Star::Star() {}

Star::Star(int x, int y) {
    this->x = x;
    this->y = y;

    this->num = rand() % 8;
    this->radius = rand() % 7 + 5;
    this->colour = Pixel(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);

    this->planets = new Planet[this->num];
    for (int i = 0; i < this->num; i++) {
        this->planets[i] = Planet(rand() % 200 + this->radius * 6 + 20);
    }
}

Json::Value Star::asJson() {
    Json::Value res;
    res["x"] = x;
    res["y"] = y;
    res["num"] = num;
    res["colour"] = colour.asInt();
    res["radius"] = radius;
    for (int i = 0; i < this->num; i++) {
        res["planets"].append(planets[i].asJson());
    }
    return res;
}
