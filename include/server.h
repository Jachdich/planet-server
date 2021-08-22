#ifndef __SERVER_H
#define __SERVER_H
#include <jsoncpp/json/json.h>
#include <mutex>
#include "logging.h"
#include "serverinterface.h"
#include "FastNoise.h"
#include "common/surfacelocator.h"

void save();

extern std::mutex m;
extern Json::Value generation_config;
extern Logger logger;
extern FastNoise noiseGen;
extern ServerInterface iface;
extern std::string saveName;
extern uint64_t ticks;
#endif
