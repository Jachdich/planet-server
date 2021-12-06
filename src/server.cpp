#include "../include/server.h"
#include "../include/logging.h"
#include "../include/generation.h"
#include "../include/network.h"
#include "../include/config.h"
#ifdef QT_GUI
#include "gui.h"
#endif
#include <fstream>
//TODO possible race condition with tasks

Logger logger;
ServerInterface iface(5555);
std::string saveName = "universe";
uint64_t ticks = 0;

void save() {
    map.saveAll(saveName);
    map.unloadAll();
    iface.saveUsers();
    //TODO save ticks
}

int main() {
    noiseGen.SetNoiseType(FastNoise::SimplexFractal);
    noiseGen.SetFractalType(FastNoise::FBM);
    noiseGen.SetFractalOctaves(5);
    noiseGen.SetFractalLacunarity(2);
    noiseGen.SetFractalGain(0.5);
    
    loadConfig();
    registerTaskTypeInfo();
    iface.startServer();
}
