#ifndef __TILE_H
#define __TILE_H
#include <vector>
#include "common/enums.h"
#include "planetsurface.h"
class PlanetSurface;

//give the server a fake vector class lol
namespace olc {
    template<typename T>
    struct Vec2 {
        T x, y;
        Vec2(T x, T y) {
            this->x = x;
            this->y = y;
        }
        Vec2() {
            x = 0;
            y = 0;
        }
        //TODO full vector class in common?
    };
    typedef Vec2<int32_t> vi2d;
    typedef Vec2<double> vd2d;
};

class Tile {
public:
    uint32_t z = 0;
    virtual void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {}
    virtual TileType getType() = 0;
    static Tile* fromType(TileType type);
};

struct VoidTile : public Tile {
    inline TileType getType() { return TileType::VOID; }
};
struct GrassTile : public Tile {
    inline TileType getType() { return TileType::GRASS; }
};
struct BushTile : public Tile {
    inline TileType getType() { return TileType::BUSH; }
};
struct TreeTile : public Tile {
    inline TileType getType() { return TileType::TREE; }
};
struct PineTile : public Tile {
    inline TileType getType() { return TileType::PINE; }
};
struct WaterTile : public Tile {
    inline TileType getType() { return TileType::WATER; }
};
struct RockTile : public Tile {
    inline TileType getType() { return TileType::ROCK; }
};
struct HouseTile : public Tile {
    inline TileType getType() { return TileType::HOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct PineforestTile : public Tile {
    inline TileType getType() { return TileType::PINEFOREST; }
};
struct ForestTile : public Tile {
    inline TileType getType() { return TileType::FOREST; }
};
struct TonkTile : public Tile {
    inline TileType getType() { return TileType::TONK; }
};
struct FarmTile : public Tile {
    inline TileType getType() { return TileType::FARM; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct GreenhouseTile : public Tile {
    inline TileType getType() { return TileType::GREENHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct WaterpumpTile : public Tile {
    inline TileType getType() { return TileType::WATERPUMP; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct MineTile : public Tile {
    inline TileType getType() { return TileType::MINE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct BlastfurnaceTile : public Tile {
    inline TileType getType() { return TileType::BLASTFURNACE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct WarehouseTile : public Tile {
    inline TileType getType() { return TileType::WAREHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

#endif
