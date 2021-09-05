#include "server.h"
#include "logging.h"
#include "generation.h"
#include "network.h"
#include "config.h"
#ifdef QT_GUI
#include "gui.h"
#endif
#include <fstream>
//TODO possible race condition with tasks

Logger logger;
ServerInterface iface(5555);
std::string saveName = "testsavev2.0";
uint64_t ticks = 0;

void save() {
    map.saveAll(saveName);
    map.unloadAll();
    iface.saveUsers();
    //TODO save ticks
}

int main() {
    const uint32_t LEVEL_SEED = 12345;

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();
    registerTaskTypeInfo();
    iface.startServer();
}
