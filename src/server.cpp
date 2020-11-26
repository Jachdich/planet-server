#include "server.h"
#include "logging.h"
#include "generation.h"
//TODO possible race condition with tasks

Logger logger;

int main() {

    const uint32_t LEVEL_SEED = 12345;

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();

    ServerInterface iface(5555);
}
