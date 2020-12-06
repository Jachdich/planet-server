#include "server.h"
#include "logging.h"
#include "generation.h"
//TODO possible race condition with tasks

Logger logger;
ServerInterface iface(5555);
int main() {

    const uint32_t LEVEL_SEED = 12345;

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();

    iface.startServer();
}
