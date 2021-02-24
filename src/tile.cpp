#include "tile.h"

Tile* Tile::fromType(TileType type) {
    switch (type) {
    	case TileType::VOID:
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
    return nullptr; //this is to get the compiler to shut the fuck up
                    //I don't wanna add a `default` case so the compiler
                    //*does* warn if a TileType is not handled, but since
                    //all types are handled there is no issue.
}

void HouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    parent->resources["peopleSlots"] += 3;
    
    parent->resources["water"] -= 1;
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
    if (abs(r - g) < 32 && r + g > b && r + g + b > 512) {
        
    }
}

void BlastfurnaceTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    
}

void WarehouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    
}
