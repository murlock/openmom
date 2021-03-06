//
//  CombatMechanics.h
//  OpenMoM
//
//  Created by Jack on 7/30/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _COMBAT_MECHANICS_H_
#define _COMBAT_MECHANICS_H_

#include "common/Common.h"
#include "Unit.h"
#include "CombatUtils.h"

enum class CombatModifier : u8;

class Game;

//TODO: this maybe not a good idea but for the moment it makes sense
using damage_value = value_t;
static constexpr damage_value FATAL_DAMAGE = std::numeric_limits<damage_value>::max();

namespace combat
{
  struct CombatTile;
  class CombatMap;
  struct CombatEnvironment;
  enum class CombatObject;
  
  struct gaze_strength
  {
    value_t strength;
    bool fatal;
    
    gaze_strength(bool alwaysFatal) : strength(0), fatal(true) { }
    gaze_strength(value_t strength) : strength(strength), fatal(false) { }
    damage_value toDamage() const { return fatal ? FATAL_DAMAGE: strength; }
  };

  class CombatFormulas
  {
  private:
    static value_t passingRolls(value_t count, value_t chance);
    static value_t passingRollsf(value_t count, float chance);
  public:

    /* School is used but this for no school damage we're using School::NO_SCHOOL which is not a good design */
    damage_value computeAreaDamage(value_t toHit, value_t strength, count_t figures, value_t hitPoints, value_t toDefend, value_t defense);
    damage_value computePhysicalDamage(value_t toHit, value_t strength, const HitPoints& hitPoints, value_t toDefend, value_t defense);
    unit_figure_value computeGazeDamage(gaze_strength strength, count_t figures, value_t resistance, value_t bonus);
  };
  
  class CombatMechanics : public CombatFormulas
  {
  private:
    Game* const game;

  public:
    CombatMechanics(Game* game) : game(game) { }
    
    /* attack related functions */
    bool canMeleeAttack(const Combat* combat, const CombatUnit* attacker, const CombatUnit* defender) { return true; }
    bool canRangedAttack(const Combat* combat, const CombatUnit* attacker, const CombatUnit* defender) { return true; }
    
    /* map related functions */
    u16 movementCostForTile(const CombatUnit* unit, const CombatTile* tile, Dir from);
    CombatPosition positionForDeployedUnit(CombatMap* map, const CombatUnit* unit, u16 index);
    CombatCoord defaultPositionForObject(CombatObject object);
    bool isTileBlocked(const CombatTile* tile, const CombatUnit* unit);
    combat_pathfind_info reachableTiles(const Combat* combat, const CombatUnit* unit, s16 movement);
    
    /* spells related functions */
    void castCombatInstant(const SpellCast& cast, const CombatUnit* unit);    
  };

  class Damage
  {
  public:
    const enum class Type {
      SINGLE,
      EACH_SAME,
      EACH_DIFFERENT,
      EACH_FATAL
    } type;
    Damage(Type type) : type(type) { }
    virtual void apply(Unit* unit) const = 0;
  };
  
  class DamageSingle : public Damage
  {
    const s16 amount;
  public:
    DamageSingle(s16 amount) : Damage(Type::SINGLE), amount(amount) { }
    void apply(Unit* unit) const override
    {
      unit->health()->applyDamage(amount);
    }
  };
  
  class DamageEachSame : public Damage
  {
    const s16 amount;
  public:
    DamageEachSame(s16 amount) : Damage(Type::EACH_SAME), amount(amount) { }
    void apply(Unit* unit) const override
    {
      unit->health()->applySameDamageToEachFigure(amount);
    }
  };
  
  class DamageEachDifferent : public Damage
  {
    const unit_figure_value amounts;
  public:
    DamageEachDifferent(unit_figure_value&& amounts) : Damage(Type::EACH_SAME), amounts(amounts) { }
    void apply(Unit* unit) const override
    {
      unit->health()->applyDifferentDamageToEachFigure(amounts);
    }
  };
  
  class DamageEachFatal : public Damage
  {
    const unit_figure_flag amounts;
  public:
    DamageEachFatal(unit_figure_flag&& amounts) : Damage(Type::EACH_FATAL), amounts(amounts) { }
    void apply(Unit* unit) const override
    {
      unit->health()->killFigures(amounts);
    }
  };

  
  
  enum class DamageType : u8
  {
    RANGED_MAGICAL_CHAOS,
    RANGED_MAGICAL_NATURE,
    RANGED_MAGICAL_LIFE,
    RANGED_MAGICAL_SORCERY,
    RANGED_MAGICAL_DEATH,
    
    RANGED_BOULDER,
    RANGED_NORMAL,
    
    THROWN,
    
    PHYSICAL_MELEE_ATTACK,
    PHYSICAL_MAGICAL_ATTACK,
    
    AREA_DAMAGE,
  };
  
  enum class AttackType
  {
    THROWN
  };
  
  enum class AttackPriority : s8
  {
    GAZE_DEFENDER = 0,
    THROWN_ATTACK = 0,
    BREATH_ATTACK = 0,
    
    GAZE_ATTACKER = 1,
    
    FIRST_STRIKE_ATTACK = 3,
    
    MELEE_ATTACK = 4,
    TOUCH_ATTACK = 4,
    
    NOT_AVAILABLE = -1
  };
  
  struct Attack
  {
    const CombatUnit* attacker;
    const CombatUnit* defender;
    
    bool isCounter;
    
    AttackType type;
    s16 strength;
    
    virtual void resolve();
  };
}

#endif
