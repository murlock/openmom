#pragma once

#include "Common.h"
#include "CombatUtils.h"

/* walls are always made by 12 sections on a 4x4 grid around the town,
 they are enumerated starting from north corner in clockwise manner.
 */
enum class WallType
{
  NORTH_CORNER = 0,
  NORTH_EAST_WALL1,
  NORTH_EAST_WALL2,
  EAST_CORNER,
  SOUTH_EAST_WALL1,
  SOUTH_EAST_GATE,
  SOUTH_CORNER,
  SOUTH_WEST_WALL1,
  SOUTH_WEST_WALL2,
  WEST_CORNER,
  NORTH_WEST_WALL1,
  NORTH_WEST_WALL2,
  NO_WALL = -1
};

class CombatMap;

struct CombatTile
{
  CombatMap* map;
  CombatCoord coords;

  s16 type;
  
  WallType stoneWall;
  WallType fireWall;
  WallType darknessWall;
  
  bool isStoneWallDestroyed;

  CombatTile() = default;
  CombatTile(CombatMap* map, s8 x, s8 y, s16 type) : map(map), type(type), coords(x,y),
  stoneWall(WallType::NO_WALL), fireWall(WallType::NO_WALL), darknessWall(WallType::NO_WALL), isStoneWallDestroyed(false) { }
  
  CombatTile* neighbour(Dir facing);
  
  s16 x() const { return coords.x; }
  s16 y() const { return coords.y; }
};

class CombatMap
{
private:
  CombatTile* tiles;
  const int W, H;

  /* this util function is used to be able to place different kind of walls with a single function */
  void placeWall(u16 x, u16 y, std::function<void(CombatTile*,WallType)> lambda);
  
public:
  CombatMap(u16 width, u16 height);
  ~CombatMap() { delete [] tiles; }
  
  const CombatTile* tileAt(u16 x, u16 y) const { return &tiles[x + y*W]; }
  CombatTile* tileAt(u16 x, u16 y) { return &tiles[x + y*W]; }
  
  void placeStoneWall(u16 x, u16 y);
  void placeFireWall(u16 x, u16 y);
  void placeDarknessWall(u16 x, u16 y);

};
