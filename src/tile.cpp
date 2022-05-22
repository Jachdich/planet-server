#include "../include/tile.h"
#include "../include/network.h"

Tile* Tile::fromType(TileType type) {
    switch (type) {
    	case TILE_AIR: return new VoidTile();
    	case TILE_GRASS: return new GrassTile();
    	case TILE_BUSH: return new BushTile();
    	case TILE_TREE: return new TreeTile();
    	case TILE_PINE: return new PineTile();
    	case TILE_WATER: return new WaterTile();
    	case TILE_ROCK: return new RockTile();
    	case TILE_HOUSE: return new HouseTile();
    	case TILE_PINEFOREST: return new PineforestTile();
    	case TILE_FOREST: return new ForestTile();
    	case TILE_TONK: return new TonkTile();
    	case TILE_FARM: return new FarmTile();
    	case TILE_GREENHOUSE: return new GreenhouseTile();
    	case TILE_WATERPUMP: return new WaterpumpTile();
    	case TILE_MINE: return new MineTile();
    	case TILE_BLASTFURNACE: return new BlastfurnaceTile();
    	case TILE_WAREHOUSE: return new WarehouseTile();
    	case TILE_FORESTRY: return new ForestryTile();
    	case TILE_CAPSULE: return new CapsuleTile();
    	case TILE_ROAD: return new RoadTile();
    	case TILE_PIPE: return new PipeTile();
    	case TILE_CABLE: return new CableTile();
    	case TILE_POWERSTATION: return new PowerstationTile();
    }
    return (Tile*)0x1; //this is to get the compiler to shut the fuck up
                       //I don't wanna add a `default` case so the compiler
                       // *does* warn if a TileType is not handled, but since
                       //all types are handled there is no issue.

                       //also the 0x1 is so I can debug shit dont question it
}

#include <iostream>

void Tile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    (void)ticks;
    (void)pos;
    (void)parent;
    (void)inRoadNet;
}

std::string Tile::getTileError() {
    return "";
}

std::string defaultTileErrorFn(Tile *t) {
    if (!t->isConnected) {
        if (!t->lastError) t->edge = true; else t->edge = false;
        t->lastError = true;
        return "No road";
    } else if (!t->hasPerson) {
        if (!t->lastError) t->edge = true; else t->edge = false;
        t->lastError = true;
        return "No workers";
    } else {
        if (t->lastError) t->edge = true; else t->edge = false;
        t->lastError = false;
        return "";
    }
}

#define CHECK_ENOUGH_PEOPLE if (parent->resources.values[RES_PEOPLE_IDLE].value > 1 || (parent->resources.values[RES_PEOPLE_IDLE].value > 0 && parent->tasks.size() > 0)) { \
        hasPerson = true;\
        parent->resources.values[RES_PEOPLE_IDLE].value -= 1;\
    } else {\
        hasPerson = false;\
        return; \
    }

void HouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    parent->resources.values[RES_PEOPLE].capacity += 3;
    for (int32_t x = -1; x < 2; x++) {
        for (int32_t y = -1; y < 2; y++) {
            if (x == 0 && y == 0) continue;
            int32_t cx = pos.x + x;
            int32_t cy = pos.y + y;
            uint32_t index = (cy * parent->rad * 2) + cx;
            if (cx < 0 || cx > parent->rad * 2 || cy < 0 || cy > parent->rad * 2) continue;
            if (parent->tiles[index]->getType() == TILE_WATER) {
                parent->resources.values[RES_WATER].value += 0.3;
                return;
            }
        }
    }
}

void FarmTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    if (ticks % 25 == 0) {
        parent->resources.values[RES_FOOD].value += 5;
    }
}

void GreenhouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    if (ticks % 12 == 0) {
        parent->resources.values[RES_FOOD].value += 5;
    }
}

void WaterpumpTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    if (ticks % 10 == 0) {
        for (int32_t x = -1; x < 2; x++) {
            for (int32_t y = -1; y < 2; y++) {
                if (x == 0 && y == 0) continue;
                int32_t cx = pos.x + x;
                int32_t cy = pos.y + y;
                uint32_t index = (cy * parent->rad * 2) + cx;
                if (cx < 0 || cx > parent->rad * 2 || cy < 0 || cy > parent->rad * 2) continue;
                if (parent->tiles[index]->getType() == TILE_WATER) {
                    parent->resources.values[RES_WATER].value += 8;
                    return;
                }
            }
        }
    }
}

void MineTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    if (ticks % 150 == 0) {
        //FFUUUUUUCKCCKCKKKKKK
        uint32_t colour = parent->getTileColour(pos.y, pos.x);
        //FUCK
        TileMinerals minerals = getTileMinerals(colour);
        parent->resources.values[RES_IRON_ORE].value += minerals.iron;
        parent->resources.values[RES_COPPER_ORE].value += minerals.copper;
        parent->resources.values[RES_ALUMINIUM_ORE].value += minerals.aluminium;
        parent->resources.values[RES_SAND].value += minerals.sand;

        parent->resources.values[RES_STONE].value += 1;
        
    }
}

int getSmeltingProduct(int n) {
    if (n == RES_IRON_ORE) return RES_IRON;
    if (n == RES_COPPER_ORE) return RES_COPPER;
    if (n == RES_ALUMINIUM_ORE) return RES_ALUMINIUM;
    if (n == RES_SAND) return RES_GLASS;
    return -1;
}

void BlastfurnaceTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    if (ticks % 128 == 0) {
        
        if (parent->resources.values[RES_WOOD].value < 1) return;
        std::vector<int> choices;
        if (parent->resources.values[RES_IRON_ORE].value >= 1) choices.push_back(RES_IRON_ORE);
        if (parent->resources.values[RES_ALUMINIUM_ORE].value >= 1) choices.push_back(RES_ALUMINIUM_ORE);
        if (parent->resources.values[RES_COPPER_ORE].value >= 1) choices.push_back(RES_COPPER_ORE);
        if (parent->resources.values[RES_SAND].value >= 1) choices.push_back(RES_SAND);
        if (choices.size() > 0) {
            uint8_t choice = rand() % choices.size();
            parent->resources.values[choices[choice]].value -= 1;
            parent->resources.values[getSmeltingProduct(choices[choice])].value += 1;
            parent->resources.values[RES_WOOD].value -= 1;
        }
    }
}

void WarehouseTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
    CHECK_ENOUGH_PEOPLE
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (i == RES_PEOPLE || i == RES_PEOPLE_IDLE) continue;
        parent->resources.values[i].capacity += 100;
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
        case 0: return TILE_TREE;
        case 1: return TILE_PINE;
        case 2: return TILE_FOREST;
        case 3: return TILE_PINEFOREST;
        default: return TILE_TREE; //should never run, just to shut the compiler up
    }
}

void ForestryTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
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

        if (parent->tiles[pos]->getType() == TILE_GRASS && (g > r && g > b * 1.5)) {
            sendTileChangeRequest(pos, genRandomTree(), &parent->loc);
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
            sendTileChangeRequest(pos, TILE_GRASS, &parent->loc);
            parent->resources.values[RES_WOOD].value += 1;
        }
    }
}

void CapsuleTile::onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {
    parent->resources.values[RES_PEOPLE].value += 1;
    parent->resources.values[RES_PEOPLE].capacity += 1;
    parent->resources.values[RES_PEOPLE_IDLE].value += 1;
    parent->resources.values[RES_WATER].value += 5;
    parent->resources.values[RES_FOOD].value += 5;
    isConnected = true;
}

void CapsuleTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {
    parent->resources.values[RES_PEOPLE].capacity += 1;
    if (parent->resources.values[RES_PEOPLE].value > 0) {
        parent->resources.values[RES_WATER].value += 0.1;
        parent->resources.values[RES_FOOD].value += 0.1;
    }
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (i == RES_PEOPLE || i == RES_PEOPLE_IDLE) continue;
        parent->resources.values[i].capacity += 100;
	}

	//Lol ironically, all the shit I said was gonna be in here is actually in
	//the PlanetSurface tick function, for some reason
	//future me here: wtf??
}

TileType typeAt(olc::vi2d pos, PlanetSurface *p) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= p->rad * 2 || pos.y >= p->rad * 2) return TILE_AIR;
    return p->tiles[pos.y * p->rad * 2 + pos.x]->getType();
}

TileType typeAt(uint32_t pos, PlanetSurface *p) {
    return p->tiles[pos]->getType();
}

Tile* tileAt(olc::vi2d pos, PlanetSurface *p) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= p->rad * 2 || pos.y >= p->rad * 2) return nullptr;
    return p->tiles[pos.y * p->rad * 2 + pos.x];
}

bool shouldCountTile(TileType type) {
    switch (type) {
        case TILE_AIR:
        case TILE_GRASS:
        case TILE_TREE:
        case TILE_FOREST:
        case TILE_PINE:
        case TILE_PINEFOREST:
        case TILE_ROAD:
            return false;
        default:
            return true; //it doesn't really matter if we count tiles that we shouldn't, it's just not as quick and memory efficient
    }
}

std::vector<Tile*> countTilesRecursive(olc::vi2d start, PlanetSurface *p, TileType type, std::vector<olc::vi2d> &searched) {
    std::vector<Tile*> found;
    std::vector<olc::vi2d> to_search;
    to_search.push_back(start);
    while (to_search.size() == 1) {
        olc::vi2d pos = to_search.back();
        to_search.pop_back();
        for (olc::vi2d offset : {olc::vi2d{1, 0}, olc::vi2d{-1, 0}, olc::vi2d{0, 1}, olc::vi2d{0, -1}}) {
            if (std::find(searched.begin(), searched.end(), pos + offset) != searched.end()) continue;
            if (shouldCountTile(typeAt(pos + offset, p))) {
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

//void RoadTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {
    //lol ironically, all the heavy lifting for the road shit is handled in the capsule tile
//}

void PipeTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {

}

void CableTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {

}

void PowerstationTile::tick(uint64_t ticks, olc::vi2d pos, PlanetSurface *parent, bool inRoadNet) {
    this->isConnected = inRoadNet;
    if (!inRoadNet) return;
}
