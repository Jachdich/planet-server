#include "tile.h"
#include "network.h"

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
    	case TileType::FORESTRY:
    	    return new ForestryTile(); break;
    	case TileType::CAPSULE:
    	    return new CapsuleTile(); break;
    }
    return (Tile*)0x1; //this is to get the compiler to shut the fuck up
                       //I don't wanna add a `default` case so the compiler
                       // *does* warn if a TileType is not handled, but since
                       //all types are handled there is no issue.

                       //also the 0x1 is so I can debug shit dont question it
}

#include <iostream>

void HouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    parent->resources.getCapacity("people") += 3;
    for (int32_t x = -1; x < 2; x++) {
        for (int32_t y = -1; y < 2; y++) {
            if (x == 0 && y == 0) continue;
            int32_t cx = pos.x + x;
            int32_t cy = pos.y + y;
            uint32_t index = (cy * parent->rad * 2) + cx;
            if (cx < 0 || cx > parent->rad * 2 || cy < 0 || cy > parent->rad * 2) continue;
            if (parent->tiles[index]->getType() == TileType::WATER) {
                parent->resources["water"] += 0.3;
                return;
            }
        }
    }
}

void FarmTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 25 == 0) {
        if (!has_person) {
            if (parent->resources["peopleIdle"] > 0) {
                parent->resources["peopleIdle"] -= 1;
                has_person = true;
            }
        } else if (parent->resources["peopleIdle"] < 1) {
            has_person = false;
            parent->resources["peopleIdle"] += 1;
        }
        if (has_person) {
            parent->resources["food"] += 5;
        }
    }
}

void GreenhouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 12 == 0) {
        if (!has_person) {
            if (parent->resources["peopleIdle"] > 0) {
                parent->resources["peopleIdle"] -= 1;
                has_person = true;
            }
        } else if (parent->resources["peopleIdle"] < 1) {
            has_person = false;
            parent->resources["peopleIdle"] += 1;
        }
        if (has_person) {
            parent->resources["food"] += 5;
        }
    }
}

void WaterpumpTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 10 == 0) {
        if (!has_person) {
            if (parent->resources["peopleIdle"] > 0) {
                parent->resources["peopleIdle"] -= 1;
                has_person = true;
            }
        } else if (parent->resources["peopleIdle"] < 1) {
            has_person = false;
            parent->resources["peopleIdle"] += 1;
        }
        if (!has_person) return;
        for (int32_t x = -1; x < 2; x++) {
            for (int32_t y = -1; y < 2; y++) {
                if (x == 0 && y == 0) continue;
                int32_t cx = pos.x + x;
                int32_t cy = pos.y + y;
                uint32_t index = (cy * parent->rad * 2) + cx;
                if (cx < 0 || cx > parent->rad * 2 || cy < 0 || cy > parent->rad * 2) continue;
                if (parent->tiles[index]->getType() == TileType::WATER) {
                    parent->resources["water"] += 8;
                    return;
                }
            }
        }
    }
}

void MineTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    if (ticks % 150 == 0) {
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
    if (ticks % 128 == 0) {
        if (!has_person) {
            if (parent->resources["peopleIdle"] > 0) {
                parent->resources["peopleIdle"] -= 1;
                has_person = true;
            }
        } else if (parent->resources["people"] < 1) {
            has_person = false;
            parent->resources["peopleIdle"] += 1;
        }
        if (!has_person) return;
        
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
    for (auto &elem: parent->resources.data) {
        if (elem.first == "people") continue;
        //if (elem.first == "water") continue;
        elem.second.capacity += 100;
	}
}

std::vector<uint32_t> getTilesInRadius(PlanetSurface *parent, olc::vi2d centre, olc::vi2d size, TileType type) {
    int32_t xn = size.x / 2;
    int32_t yn = size.y / 2;

    std::vector<uint32_t> available_pos;
    for (int32_t x = -xn; x < xn + 1; x++) {
        for (int32_t y = -yn; y < yn + 1; y++) {
            int32_t cx = centre.x + x;
            int32_t cy = centre.y + y;
            uint32_t index = (cy * parent->rad * 2) + cx;
            if (cx < 0 || cx > parent->rad * 2 || cy < 0 || cy > parent->rad * 2) continue;
            if (parent->tiles[index]->getType() == type) {
                available_pos.push_back(index);
            }
        }
    }
    return available_pos;
}

void ForestryTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {
    if (ticks % 32 == 0) {
        if (!has_person) {
            if (parent->resources["peopleIdle"] > 1) {
                parent->resources["peopleIdle"] -= 1;
                has_person = true;
            }
        } else if (parent->resources["peopleIdle"] < 1) {
            has_person = false;
            parent->resources["peopleIdle"] += 1;
        }
        if (!has_person) return;
    }
    
    if (ticks % 64 == 0) {
        //std::vector<uint32_t> available_pos = getTilesInRadius(parent, pos, {5, 5}, TileType::GRASS);
        //if (available_pos.size() == 0) return;
        //uint32_t pos = available_pos[rand() % available_pos.size()];
        int32_t cx = pos.x - 5 / 2 + rand() % 5;
        int32_t cy = pos.y - 5 / 2 + rand() % 5;
        uint32_t pos = (cy * parent->rad * 2) + cx;

        uint32_t col = parent->getTileColour(cy, cx);
        uint8_t r = (col >> 16) & 0xFF;
        uint8_t g = (col >> 8) & 0xFF;
        uint8_t b = col & 0xFF;

        if (parent->tiles[pos]->getType() == TileType::GRASS && (g > r && g > b * 1.5)) {
            sendTileChangeRequest(pos, TileType::TREE, parent->loc);
        }
    }

    if ((ticks + 32) % 64 == 0) {
    /*
        std::vector<uint32_t> available_pos = getTilesInRadius(parent, pos, {5, 5}, TileType::TREE);
        if (available_pos.size() == 0) return;
        uint32_t pos = available_pos[rand() % available_pos.size()];
        sendTileChangeRequest(pos, TileType::GRASS, parent->loc);*/
        int32_t cx = pos.x - 5 / 2 + rand() % 5;
        int32_t cy = pos.y - 5 / 2 + rand() % 5;
        uint32_t pos = (cy * parent->rad * 2) + cx;
        if (parent->tiles[pos]->getType() == TileType::TREE) {
            sendTileChangeRequest(pos, TileType::GRASS, parent->loc);
            parent->resources["wood"] += 1;
        }
    }
}

void CapsuleTile::onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    parent->resources["people"] += 1;
    parent->resources["peopleIdle"] += 1;
    parent->resources["water"] += 0.1;
    parent->resources["food"] += 0.1;
}

void CapsuleTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {
    parent->resources.getCapacity("people") += 1;
    if (parent->resources["people"] > 0) {
        parent->resources["water"] += 0.1;
        parent->resources["food"] += 0.1;
    }
    for (auto &elem: parent->resources.data) {
        if (elem.first == "people") continue;
        elem.second.capacity += 100;
	}
}
