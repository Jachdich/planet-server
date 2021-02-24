#ifndef __SERVER_H
#define __SERVER_H
#include <jsoncpp/json/json.h>
#include <mutex>
#include "logging.h"
#include "network.h"
#include "FastNoise.h"
#include "common/surfacelocator.h"

struct Task {
	TaskType type;
	uint32_t target;
	SurfaceLocator surface;
	double timeLeft;
};

void save();

extern std::vector<Task> tasks;
extern std::mutex m;
extern Json::Value generation_config;
extern Logger logger;
extern FastNoise noiseGen;
extern ServerInterface iface;
extern std::string saveName;
extern uint64_t ticks;
#endif
