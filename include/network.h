#ifndef __NETWORK_H
#define __NETWORK_H
#include "sectormap.h"

extern SectorMap map;

#include "server.h"
#include "common/surfacelocator.h"

void sendResourcesChangeRequest(const Resources *resources, const SurfaceLocator *loc);
void registerTaskTypeInfo();
void sendTileChangeRequest(uint32_t pos, TileType type, const SurfaceLocator *loc);
void sendTileErrorSetRequest(const SurfaceLocator *loc, uint32_t index, std::string err);
void sendSetTimerRequest(double time, uint32_t target, const SurfaceLocator *loc);

#endif
