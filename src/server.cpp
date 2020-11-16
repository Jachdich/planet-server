#include "server.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "generation.h"
#include "logging.h"
#include "sector.h"
#include "sectormap.h"
#include "FastNoise.h"
#include "common/enums.h"
#include "common/surfacelocator.h"
#include "random_helper.h"

//return codes
// -1: malformed JSON
// -2: invalid request
// -3: out of bounds

//TODO possible race condition with tasks
using asio::ip::tcp;

class Tile;

Logger logger;

using asio::ip::tcp;


#include <sys/time.h>
#include <sys/resource.h>

int main() {

    const uint32_t LEVEL_SEED = 12345;
/*
    int x = 36;
    int y = 391;

    std::cout << mix(x, y) << ", " << hash(x) << ", " << hash(y) << "\n";

    return 0;*/

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();

    ServerInterface iface(5555);
}
