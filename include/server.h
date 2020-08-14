#ifndef __SERVER_H
#define __SERVER_H
#include <jsoncpp/json/json.h>
#include <mutex>
#include "logging.h"
#include "FastNoise.h"

extern std::mutex m;
extern Json::Value generation_config;
extern Logger logger;
extern FastNoise noiseGen;
#endif
