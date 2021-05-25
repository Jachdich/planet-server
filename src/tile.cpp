#include "tile.h"

Tile* Tile::fromType(TileType type) {
    switch (type) {
    	case TileType::AIR:
    	    return new VoidTile(); break;
    	case TileType::GRASS:
    	    return new GrassTile(); break;
    	case TileType::BUSH:
    	    return new BushTile(); break;
    	case TileType::TREE:
    	    return new TreeTile(); break;
    	case TileType::PINE:
    	    return new PineTile(); break;
    	case TileType::WATER:
    	    return new WaterTile(); break;
    	case TileType::ROCK:
    	    return new RockTile(); break;
    	case TileType::HOUSE:
    	    return new HouseTile(); break;
    	case TileType::PINEFOREST:
    	    return new PineforestTile(); break;
    	case TileType::FOREST:
    	    return new ForestTile(); break;
    	case TileType::TONK:
    	    return new TonkTile(); break;
    	case TileType::FARM:
    	    return new FarmTile(); break;
    	case TileType::GREENHOUSE:
    	    return new GreenhouseTile(); break;
    	case TileType::WATERPUMP:
    	    return new WaterpumpTile(); break;
    	case TileType::MINE:
    	    return new MineTile(); break;
    	case TileType::BLASTFURNACE:
    	    return new BlastfurnaceTile(); break;
    	case TileType::WAREHOUSE:
    	    return new WarehouseTile(); break;
    }
    return (Tile*)0x1; //this is to get the compiler to shut the fuck up
                       //I don't wanna add a `default` case so the compiler
                       //*does* warn if a TileType is not handled, but since
                       //all types are handled there is no issue.

                       //also the 0x1 is so I can debug shit dont question it
}

uint32_t rgb2hsv(uint8_t r_, uint8_t g_, uint8_t b_) {
    double h, s, v, r, g, b;
    r = (double)r_ / 256;
    g = (double)g_ / 256;
    b = (double)b_ / 256;
    
    double min, max, delta;

    min = r < g ? r : g;
    min = min < b ? min : b;

    max = r > g ? r : g;
    max = max > b ? max : b;

    v = max; // v
    delta = max - min;
    if (delta < 0.00001)
    {
        s = 0;
        h = 0; // undefined, maybe nan?
        return ((int)(h / 360 * 256) << 16) | ((int)(s * 256) << 8) | (int)(v * 256);
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        s = 0.0;
        h = 0.0;
        return ((int)(h / 360 * 256) << 16) | ((int)(s * 256) << 8) | (int)(v * 256);
    }
    if( r >= max )                           // > is bogus, just keeps compilor happy
        h = ( g - b ) / delta;        // between yellow & magenta
    else
    if( g >= max )
        h = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        h = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    h *= 60.0;                              // degrees

    if( h < 0.0 )
        h += 360.0;

    return ((int)h << 24) | ((int)(s * 256) << 8) | (int)(v * 256);
}

#include <iostream>

void HouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    parent->resources["peopleSlots"] += 3;
    for (int32_t x = -1; x < 2; x++) {
        for (int32_t y = -1; y < 2; y++) {
            if (x == 0 && y == 0) continue;
            int32_t cx = pos.x + x;
            int32_t cy = pos.y + y;
            int32_t index = (cy * parent->rad * 2) + cx;
            if (index > (parent->rad * 2) * (parent->rad * 2) || index < 0) continue;
            if (parent->tiles[index]->getType() == TileType::WATER) {
                parent->resources["water"] += 0.3;
                return;
            }
        }
    }
}

void FarmTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 50 == 0) {
        parent->resources["food"] += 5;
    }
}

void GreenhouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 25 == 0) {
        parent->resources["food"] += 5;
    }
}

void WaterpumpTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 10 == 0) {
        parent->resources["water"] += 10;
    }
}

void MineTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    uint32_t colour = parent->getTileColour(pos.x, pos.y);
    uint8_t r = colour >> 16;
    uint8_t g = (colour >> 8) & 0xFF;
    uint8_t b = colour & 0xFF;
    uint32_t hsv = rgb2hsv(r, g, b);
    uint16_t h = hsv >> 16;
    uint8_t s = (hsv >> 8) & 0xFF;
    uint8_t v = hsv & 0xFF;
    if (h >= 47 && h <= 61 && s >= 64 && v >= 205) {
        
    }
}

void BlastfurnaceTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    
}

void WarehouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    
}
