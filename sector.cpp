#include "sector.h"

#include <jsoncpp/json/json.h>
#include "star.h"

Sector::Sector() {}
Sector::Sector(int sx, int sy, int sr) {
    this->x = sx;
    this->y = sy;
    this->r = sr;
}

Star * Sector::getStarAt(int x, int y) {
    for (int i = 0; i < numStars; i++) {
        int dx = x - stars[i].x;
        int dy = y - stars[i].y;
        if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
            return &this->stars[i];
        }
    }
    return nullptr;
}

void Sector::generate() {
    this->numStars = rand() % 8 + 2;
    this->stars = std::vector<Star>(numStars);
    for (int i = 0; i < this->numStars; i++) {
        this->stars[i] = Star(rand() % this->r, rand() % this->r);
    }
    generated = true;
}

Json::Value Sector::asJson() {
    Json::Value res;
    for (int i = 0; i < numStars; i++) {
        res["stars"].append(stars[i].asJson());
    }
    res["numStars"] = numStars;
    res["x"] = x;
    res["y"] = y;
    res["r"] = r;
    return res;
}
