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
    if (ticks % 25 == 0) {
        parent->resources["food"] += 5;
    }
}

void GreenhouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 12 == 0) {
        parent->resources["food"] += 5;
    }
}

void WaterpumpTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 10 == 0) {
        parent->resources["water"] += 10;
    }
}

void MineTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 100 == 0) {
        //FFUUUUUUCKCCKCKKKKKK
        uint32_t colour = parent->getTileColour(pos.y, pos.x);
        //FUCK
        TileMinerals minerals = getTileMinerals(colour);
        parent->resources["ironOre"] += minerals.iron;
        parent->resources["copperOre"] += minerals.copper;
        parent->resources["aluminiumOre"] += minerals.aluminium;
        parent->resources["sand"] += minerals.sand;

        parent->resources["stone"] += 1;
        
    }
}

std::string getProduct(std::string n) {
    if (n == "ironOre") return "iron";
    if (n == "copperOre") return "copper";
    if (n == "aluminiumOre") return "aluminium";
    if (n == "sand") return "glass";
    return "";
}

void BlastfurnaceTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 50 == 0) {
        if (parent->resources["wood"] < 1) return;
        std::vector<std::string> choices;
        if (parent->resources["ironOre"] >= 1) choices.push_back("ironOre");
        if (parent->resources["aluminiumOre"] >= 1) choices.push_back("aluminiumOre");
        if (parent->resources["copperOre"] >= 1) choices.push_back("copperOre");
        if (parent->resources["sand"] >= 1) choices.push_back("sand");
        if (choices.size() > 0) {
            uint8_t choice = rand() % choices.size();
            parent->resources[choices[choice]] -= 1;
            parent->resources[getProduct(choices[choice])] += 1;
            parent->resources["wood"] -= 1;
        }
    }
}

void WarehouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    
}
