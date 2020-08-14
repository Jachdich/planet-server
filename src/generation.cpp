#include "generation.h"

#include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <fstream>

//int p_radMin = 8;
//int p_radMax = 28;
//double p_genChanceMin = 0.4d;
//double p_genChanceMax = 0.7d;
//double p_genNoiseMin = 0.1d;
//double p_genNoiseMax = 1.0d;
//int p_baseColMin = 80;
//int p_baseColMax = 255;
//double p_angularVelMultiplier = 6000.0d;
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
