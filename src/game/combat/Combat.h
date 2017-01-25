#ifndef _COMBAT_H_
#define _COMBAT_H_

#include "Common.h"
#include "Unit.h"
#include "Army.h"
#include "CombatUtils.h"

#include <unordered_map>

class HitPoints;

namespace combat
{
  class CombatMap;
  class CombatMechanics;
  struct CombatTile;

  enum class CombatModifier : u8
  {
    MOD_HALF,
    MOD_NONE
  };

  class CombatUnit : public Propertable
  {
  private:
    Unit* unit;
    Player* player;
    Side _side;
    
  public:
    CombatUnit(Side side, Unit* unit) : unit(unit), player(unit->getArmy()->getOwner()), _side(side), moves(unit->getProperty(Property::MOVEMENT)*2), selected(false) { }
    
    Player* const getOwner() { return player; }
    
    void resetMoves() { moves = unit->getProperty(Property::MOVEMENT)*2; }
    
    Unit* getUnit() const { return unit; }
    
    s16 getBaseProperty(Property property) const override
    {
      if (property == Property::AVAILABLE_MOVEMENT)
        return moves;
      else
        return unit->getBaseProperty(property);
    }
    
    s16 getBonusProperty(Property property) const override
    {
      return unit->getBonusProperty(property);
    }
    
    void setPosition(u16 x, u16 y) { position.position = CombatCoord(x,y); }
    void setPosition(u16 x, u16 y, Dir facing) { position = CombatPosition(x,y,facing); }
    void setPosition(CombatCoord tile) { position.position = tile; }
    void setPosition(CombatPosition pos) { position = pos; }
    void setFacing(Dir facing) { position.facing = facing; }
    
    u16 x() const { return position.position.x; }
    u16 y() const { return position.position.y; }
    Dir facing() const { return position.facing; }
    Side side() const { return _side; }
    
    bool isAttacker() const { return _side == Side::ATTACKER; }
    bool isDefender() const { return _side == Side::DEFENDER; }
    
    bool hasMoves() const { return moves > 0; }
    
    /* TODO: maybe a SkillSet for combat unit is needed which wraps the native one to manage combat only effects */
    const SkillSet* skills() const { return unit->skills(); }
    SkillSet* skills() { return unit->skills(); }

    CombatPosition position;
    u16 moves;
    bool selected;
  };

  class CombatInterface
  {
  public:
    virtual CombatUnit* unitAtTile(u16 x, u16 y) = 0;
    virtual bool isPositionEmpty(u16 x, u16 y) = 0;
    
    virtual void attack(CombatUnit* u1, CombatUnit* u2) = 0;
    virtual void moveUnit(CombatUnit* u1, u16 x, u16 y) = 0;
    
    virtual void endTurn();
  };


  class Combat
  {
  private:
    cast_list spells;
    Player* players[2];
    std::vector<CombatUnit*> allUnits;
    std::list<CombatUnit*> units[2];
    
    CombatMechanics* const mechanics;
    
    CombatUnit* selectedUnit;
    Player* current;
    
    std::unique_ptr<CombatMap> _map;
    
    position_map currents, visited, incoming;
    
  public:
    static constexpr size_t W = 11;
    static constexpr size_t H = 22;
    static bool isValidTile(u16 x, u16 y) {
      return x > 0 && y > 1 && (x != 10 || (y % 2 == 0));
    }
    
    Combat(Army* a1, Army* a2, CombatMechanics* mechanics);
    ~Combat();
    
    void endTurn();
    
    
    const CombatTile* tileAt(s16 x, s16 y);
    CombatUnit* unitAtTile(u16 x, u16 y);
    bool isTileEmpty(u16 x, u16 y) { return !unitAtTile(x, y); }

    CombatMap* map() { return _map.get(); }
    const std::list<CombatUnit*>& enemyUnits(Player* player) { return player == players[0] ? units[1] : units[0]; }
    const std::list<CombatUnit*>& friendlyUnits(Player* player) { return player == players[1] ? units[0] : units[1]; }
    
    void attack(CombatUnit *u1, CombatUnit *u2);
    
    Dir relativeFacing(CombatUnit *u1, CombatUnit *u2);
    
    void moveUnit(CombatUnit *unit, u16 x, u16 y);
    
    void castEnchantment(const SpellCast &cast) { spells.push_back(cast); }
    
    const position_map& reachable(CombatUnit* unit);
    
    void deployUnits();
    
    const std::vector<CombatUnit*>& getUnits() { return allUnits; }

    void deselect()
    {
      selectedUnit->selected = false;
      selectedUnit = nullptr;
    }
    
    void select(CombatUnit *unit)
    {
      if (selectedUnit) selectedUnit->selected = false;
      
      selectedUnit = unit;
      unit->selected = true;
    }
    
    const cast_list& getSpells() const { return spells; }
  };
}

#endif
