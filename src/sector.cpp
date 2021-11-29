#include "sector.h"

#include <jsoncpp/json/json.h>
#include <fstream>
#include <iostream>
#include "star.h"
#include "generation.h"

/*
#include <random>

void random_integer(int a, int b) {
    thread_local auto generator = std::default_random_engine{
        std::random_device{}()};
    auto distribution = std::uniform_integer_distribution<int>{a, b};
    return distribution(generator);
}*/

Sector::Sector() {}
Sector::Sector(uint32_t sx, uint32_t sy, int sr) {
    this->x = sx;
    this->y = sy;
    this->r = sr;
}

Star * Sector::getStarAt(int x, int y) {
    for (uint32_t i = 0; i < numStars; i++) {
        int32_t dx = x - stars[i].x;
        int32_t dy = y - stars[i].y;
        if ((unsigned)(dx * dx + dy * dy) < stars[i].radius * stars[i].radius) {
            return &this->stars[i];
        }
    }
    return nullptr;
}

void Sector::generate() {
    uint32_t seed = (hash(x) + hash(y) * 5) + hash(genConf["level_seed"].asUInt());
    srand(seed);
    this->numStars = rndInt(genConf["c_numStarsMin"].asInt(), genConf["c_numStarsMax"].asInt());
    this->stars = std::vector<Star>(numStars);
    for (uint32_t i = 0; i < this->numStars; i++) {
    	SurfaceLocator loc = {0, (uint8_t)i, (int32_t)x, (int32_t)y};
        this->stars[i] = Star(rand() % this->r, rand() % this->r, loc, seed);
    }
    generated = true;
}

void Sector::generate(std::string dir) {
	std::ifstream afile;
	afile.open(dir + "/" + "s" + std::to_string((int32_t)x) + "." + std::to_string((int32_t)y) + ".json");

	std::string content((std::istreambuf_iterator<char>(afile)), (std::istreambuf_iterator<char>()));

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        content.c_str(),
        content.c_str() + content.size(),
        &root,
        &errors
    );
    delete reader;

    if (!parsingSuccessful) {
        std::cerr << "Error: could not parse JSON in sector file\n";
    }

	afile.close();

	this->x = root["x"].asUInt();
    this->y = root["y"].asUInt();
    this->r = root["r"].asInt();
    this->numStars = root["numStars"].asInt();
	this->stars = std::vector<Star>(numStars);
    for (uint32_t i = 0; i < numStars; i++) {
        SurfaceLocator loc = {0, (uint8_t)i, (int32_t)x, (int32_t)y};
        stars[i] = Star(root["stars"][i], loc);
    }

	this->generated = true;
}

bool Sector::existsInSave(std::string dir) {
	std::ifstream infile;
	infile.open(dir + "/" + "s" + std::to_string(x) + "." + std::to_string(y) + ".json");
    return infile.good();
}

Json::Value Sector::asJson() {
    Json::Value res;
    for (uint32_t i = 0; i < numStars; i++) {
        res["stars"].append(stars[i].asJson());
    }
    res["numStars"] = numStars;
    res["x"] = x;
    res["y"] = y;
    res["r"] = r;
    return res;
}

void Sector::save(std::string dir) {
	std::ofstream afile;
	afile.open(dir + "/" + "s" + std::to_string((int32_t)x) + "." + std::to_string((int32_t)y) + ".json");
	Json::StreamWriterBuilder writeBuilder;
	writeBuilder["indentation"] = "";
	afile << Json::writeString(writeBuilder, this->asJson()) << "\n";
	afile.close();
}
