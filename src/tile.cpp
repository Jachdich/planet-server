#include "tile.h"
#include "network.h"

Tile* Tile::fromType(TileType type) {
    switch (type) {
    	case TileType::AIR: return new VoidTile();
    	case TileType::GRASS: return new GrassTile();
    	case TileType::BUSH: return new BushTile();
    	case TileType::TREE: return new TreeTile();
    	case TileType::PINE: return new PineTile();
    	case TileType::WATER: return new WaterTile();
    	case TileType::ROCK: return new RockTile();
    	case TileType::HOUSE: return new HouseTile();
    	case TileType::PINEFOREST: return new PineforestTile();
    	case TileType::FOREST: return new ForestTile();
    	case TileType::TONK: return new TonkTile();
    	case TileType::FARM: return new FarmTile();
    	case TileType::GREENHOUSE: return new GreenhouseTile();
    	case TileType::WATERPUMP: return new WaterpumpTile();
    	case TileType::MINE: return new MineTile();
    	case TileType::BLASTFURNACE: return new BlastfurnaceTile();
    	case TileType::WAREHOUSE: return new WarehouseTile();
    	case TileType::FORESTRY: return new ForestryTile();
    	case TileType::CAPSULE: return new CapsuleTile();
    	case TileType::ROAD: return new RoadTile();
    	case TileType::PIPE: return new PipeTile();
    	case TileType::CABLE: return new CableTile();
    	case TileType::POWERSTATION: return new PowerstationTile();
    }
    return (Tile*)0x1; //this is to get the compiler to shut the fuck up
                       //I don't wanna add a `default` case so the compiler
                       // *does* warn if a TileType is not handled, but since
                       //all types are handled there is no issue.

                       //also the 0x1 is so I can debug shit dont question it
}

#include <iostream>

void Tile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
}

std::string Tile::getTileError() {
    return "";
}

std::string defaultTileErrorFn(Tile *t) {
    if (!t->hasPerson) {
        if (!t->lastError) t->edge = true; else t->edge = false;
        t->lastError = true;
        return "Noone";
    } else {
        if (t->lastError) t->edge = true; else t->edge = false;
        t->lastError = false;
        return "";
    }
}

#define CHECK_ENOUGH_PEOPLE if (parent->resources["peopleIdle"] > 1) { hasPerson = true; parent->resources["peopleIdle"] -= 1; } else { hasPerson = false; return; }

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
    CHECK_ENOUGH_PEOPLE
    if (ticks % 25 == 0) {
        parent->resources["food"] += 5;
    }
}

void GreenhouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    CHECK_ENOUGH_PEOPLE
    if (ticks % 12 == 0) {
        parent->resources["food"] += 5;
    }
}

void WaterpumpTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    CHECK_ENOUGH_PEOPLE
    if (ticks % 10 == 0) {
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
    CHECK_ENOUGH_PEOPLE
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
    CHECK_ENOUGH_PEOPLE
    if (ticks % 128 == 0) {
        
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
    CHECK_ENOUGH_PEOPLE
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

TileType genRandomTree() {
    switch (rand() % 4) {
        case 0: return TileType::TREE;
        case 1: return TileType::PINE;
        case 2: return TileType::FOREST;
        case 3: return TileType::PINEFOREST;
        default: return TileType::TREE; //should never run, just to shut the compiler up
    }
}

void ForestryTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {
    CHECK_ENOUGH_PEOPLE
    
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
            sendTileChangeRequest(pos, genRandomTree(), parent->loc);
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
        if (isTree(parent->tiles[pos]->getType())) {
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

TileType typeAt(olc::vi2d pos, PlanetSurface *p) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= p->rad * 2 || pos.y >= p->rad * 2) return TileType::AIR;
    return p->tiles[pos.y * p->rad * 2 + pos.x]->getType();
}

TileType typeAt(uint32_t pos, PlanetSurface *p) {
    return p->tiles[pos]->getType();
}

Tile* tileAt(olc::vi2d pos, PlanetSurface *p) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= p->rad * 2 || pos.y >= p->rad * 2) return nullptr;
    return p->tiles[pos.y * p->rad * 2 + pos.x];
}

std::vector<Tile*> countTilesRecursive(olc::vi2d start, PlanetSurface *p, TileType type, std::vector<olc::vi2d> &searched) {
    //logger.info("countTilesRecursive called on " + std::to_string(start.x) + "," + std::to_string(start.y));
    std::vector<Tile*> found;
    std::vector<olc::vi2d> to_search;
    to_search.push_back(start);
    while (to_search.size() == 1) {
        olc::vi2d pos = to_search.back();
        to_search.pop_back();
        for (olc::vi2d offset : {olc::vi2d{1, 0}, olc::vi2d{-1, 0}, olc::vi2d{0, 1}, olc::vi2d{0, -1}}) {
            if (std::find(searched.begin(), searched.end(), pos + offset) != searched.end()) continue;
            if (typeAt(pos + offset, p) == TileType::ROCK) {
                found.push_back(tileAt(pos + offset, p));
            }
            if (typeAt(pos + offset, p) == type) {
                to_search.push_back(pos + offset);
            }
            searched.push_back(pos + offset);
        }
    }
    if (to_search.size() != 0) {
        for (olc::vi2d &offset: to_search) {
            std::vector<Tile*> res = countTilesRecursive(offset, p, type, searched);
            found.insert(found.end(), res.begin(), res.end());
        }
    }
    return found;
}

std::vector<Tile*> countTiles(olc::vi2d start, PlanetSurface *p, TileType type) {
    std::vector<olc::vi2d> searched = {start};
    return countTilesRecursive(start, p, type, searched);
}

void RoadTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {
    
}

void PipeTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {
    sendTileChangeRequest(pos.y * parent->rad * 2 + pos.x + 1, TileType::ROAD, parent->loc);
    std::vector<Tile*> foundTiles = countTiles(pos + olc::vi2d{1, 0}, parent, TileType::ROAD);
    logger.info(std::to_string(foundTiles.size()));
    for (uint32_t i = 0; i < parent->rad * parent->rad * 4; i++) {
        TileType ty = typeAt(i, parent);
        if (ty == TileType::GRASS || ty == TileType::ROCK) {
            sendTileChangeRequest(i, TileType::ROAD, parent->loc);
        }
    }
}

void CableTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {

}

void PowerstationTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent) {

}