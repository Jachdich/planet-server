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
    virtual void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent);
    virtual void onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) {}
    std::string getTileError();
    void updatePeople(PlanetSurface *parent);
    virtual TileType getType() = 0;
    static Tile* fromType(TileType type);
    virtual ~Tile() {}

    int required_people = 0;
    int available_people = 0;
    bool lastError = false;
    bool edge = false;
};

std::string defaultTileErrorFn(Tile *t);
TileType typeAt(olc::vi2d pos, PlanetSurface *p);
std::vector<Tile*> countTiles(olc::vi2d start, PlanetSurface *p, TileType type);

//static tiles: no tick function
struct VoidTile  : public Tile { inline TileType getType() { return TILE_AIR; } };
struct GrassTile : public Tile { inline TileType getType() { return TILE_GRASS; } };
struct BushTile  : public Tile { inline TileType getType() { return TILE_BUSH; } };
struct TreeTile  : public Tile { inline TileType getType() { return TILE_TREE; } };
struct PineTile  : public Tile { inline TileType getType() { return TILE_PINE; } };
struct WaterTile : public Tile { inline TileType getType() { return TILE_WATER; } };
struct RockTile  : public Tile { inline TileType getType() { return TILE_ROCK; } };
struct PineforestTile : public Tile { inline TileType getType() { return TILE_PINEFOREST; } };
struct ForestTile : public Tile { inline TileType getType() { return TILE_FOREST; } };
struct TonkTile   : public Tile { inline TileType getType() { return TILE_TONK; } };

//dynamic tiles: tick and/or error functions
struct HouseTile : public Tile {
    inline TileType getType() { return TILE_HOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

struct FarmTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_FARM; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct GreenhouseTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_GREENHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct WaterpumpTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_WATERPUMP; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct MineTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_MINE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct BlastfurnaceTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_BLASTFURNACE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct WarehouseTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_WAREHOUSE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct ForestryTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_FORESTRY; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct CapsuleTile : public Tile {
    inline TileType getType() { return TILE_CAPSULE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
    void onPlace(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

struct RoadTile : public Tile {
    inline TileType getType() { return TILE_ROAD; }
    //void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct PipeTile : public Tile {
    inline TileType getType() { return TILE_PIPE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct CableTile : public Tile {
    inline TileType getType() { return TILE_CABLE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

struct PowerstationTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_POWERSTATION; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};

struct SolarPanelTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_SOLAR_PANEL; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct TurbineTile : public Tile {
    int required_people = 1;
    inline TileType getType() { return TILE_TURBINE; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
struct LabTile : public Tile {
    int required_people = 3;
    inline TileType getType() { return TILE_LAB; }
    void tick(uint64_t ticks, olc::vi2d pos, PlanetSurface* parent) override;
};
#endif
