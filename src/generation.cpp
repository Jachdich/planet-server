#include "generation.h"

#include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <fstream>

Json::Value genConf;

void setDefault(std::string value) {
    if (value == "p") {
        //TODO do it
    }
}

void loadConfig() {
    std::ifstream generation_config_file("generation.json", std::ifstream::binary);
    generation_config_file >> genConf;
}

void saveConfig() {
    Json::StreamWriterBuilder writeBuilder;
    writeBuilder["indentation"] = "";
    const std::string output = Json::writeString(writeBuilder, genConf);
}

int rndInt(int min, int max) {
    return rand() % (max - min) + min;
}

double rndDouble(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

Pixel::Pixel() {}
Pixel::Pixel(int r, int g, int b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Pixel::Pixel(uint32_t n) {
    r = (n >> 16) & 0xFF;
    g = (n >>  8) & 0xFF;
    b = (n >>  0) & 0xFF;
}

void Pixel::rand() {
    r = rndInt(0, 255);
    g = rndInt(0, 255);
    b = rndInt(0, 255);
}

void Pixel::rand(int min, int max) {
    r = rndInt(min, max);
    g = rndInt(min, max);
    b = rndInt(min, max);
}

int Pixel::asInt() {
    return (r << 16) | (g << 8) | b;
}
