#ifndef __TILE_H
#define __TILE_H
#include <vector>
#include <string>
#include "common/enums.h"
#include "planetsurface.h"
class PlanetSurface;

//troll the server with a fake vector class lol
namespace olc {
    template<typename T>
    struct Vec2 {
        T x, y;
        Vec2(T x_, T y_) : x(x_), y(y_) {}
        Vec2() : x(0), y(0) {}
        bool operator==(const Vec2<T> &other) const {
            return other.x == x && other.y == y;
        }
        
        bool operator!=(const Vec2<T> &other) const {
            return !(*this == other);
        }        
        Vec2<T> operator+=(const Vec2<T> &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2<T> operator+(const Vec2<T> &other) const {
            return {x + other.x, y + other.y};
        }
        //TODO full vector class in common?
    };
    typedef Vec2<int32_t> vi2d;
    typedef Vec2<double> vd2d;
};

class Tile {
public:
    uint32_t z = 0;
    virtual void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet);
    virtual void onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {}
    virtual std::string getTileError();
    virtual TileType getType() = 0;
    static Tile* fromType(TileType type);
    virtual ~Tile() {}

    bool hasPerson = false;
    bool lastError = false;
    bool edge = false;
};

std::string defaultTileErrorFn(Tile *t);
TileType typeAt(olc::vi2d pos, PlanetSurface *p);
std::vector<Tile*> countTiles(olc::vi2d start, PlanetSurface *p, TileType type);

struct VoidTile : public Tile {
    inline TileType getType() { return TileType::AIR; }
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
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
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
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::FARM; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct GreenhouseTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::GREENHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct WaterpumpTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::WATERPUMP; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct MineTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::MINE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct BlastfurnaceTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::BLASTFURNACE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct WarehouseTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::WAREHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct ForestryTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::FORESTRY; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct CapsuleTile : public Tile {
    inline TileType getType() { return TileType::CAPSULE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
    void onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

struct RoadTile : public Tile {
    inline TileType getType() { return TileType::ROAD; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct PipeTile : public Tile {
    inline TileType getType() { return TileType::PIPE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
struct CableTile : public Tile {
    inline TileType getType() { return TileType::CABLE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};

struct PowerstationTile : public Tile {
    inline std::string getTileError() { return defaultTileErrorFn(this); }
    inline TileType getType() { return TileType::POWERSTATION; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent, bool inRoadNet) override;
};
#endif
