//
//  Effects.h
//  OpenMoM
//
//  Created by Jack on 7/21/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _SKILL_EFFECTS_H_
#define _SKILL_EFFECTS_H_

#include "Common.h"

#include <vector>

class SkillEffect;
typedef const std::vector<const SkillEffect*> effect_list;
typedef const std::initializer_list<const SkillEffect*> effect_init_list;

enum class Property : u8;
class Unit;

class SkillEffect
{
public:
  const enum class Type : u8
  {
    MOVEMENT,
    ABILITY,
    IMMUNITY,
    MAGIC_WEAPONS,
    
    PROPERTY_BONUS,
    ARMY_BONUS,
    
    SPECIAL_ATTACK
  } type;
  
  SkillEffect(Type type) : type(type) { }
};

class SimpleEffect : public SkillEffect
{
public:
  const enum class Type : u16
  {
    FORESTWALK = 0,
    FLYING,
    UNDERGROUND,
    MOUNTAINWALK,
    NON_CORPOREAL,
    PATH_FINDER,
    PLANAR_TRAVEL,
    TELEPORT,
    SWIMMING,
    WINDWALK,
    SAILING,
    DESERTWALK,
    SWAMPWALK,
    
    IMMUNITY_MAGIC,
    IMMUNITY_ILLUSIONS,
    IMMUNITY_MISSILE,
    
    
    POISON,
    LIFE_STEALING,
    STONE_TOUCH,
    
    IMMOLATION,
    
    FIRE_BREATH,
    LIGHTING_BREATH,
    
    THROWN_ATTACK,
    
    DOOM_GAZE,
    DEATH_GAZE,
    STONE_GAZE,
    
    
    
    CREATE_OUTPOST,
    CREATE_ROAD,
    MELD_NODE,
    WALL_CRUSHING,
    PURIFY,
    
    FIRST_STRIKE,
    NEGATE_FIRST_STRIKE,
    ARMOR_PIERCING,
    LONG_RANGE,
    ILLUSIONARY_ATTACK
  } effect;
  
  SimpleEffect(SkillEffect::Type type, Type effect) : SkillEffect(type), effect(effect) { }
};

class MovementEffect : public SimpleEffect
{
public:
  MovementEffect(SimpleEffect::Type movement, bool shared) : SimpleEffect(SkillEffect::Type::MOVEMENT, movement), shared(shared) { }
  const bool shared;
};

class SpecialAttackEffect : public SimpleEffect
{
public:
  SpecialAttackEffect(SimpleEffect::Type attack, s16 strength = 0) : SimpleEffect(SkillEffect::Type::SPECIAL_ATTACK, attack), strength(strength) { }
  const s16 strength;
};

class PropertyBonus : public SkillEffect
{
protected:
  PropertyBonus(Property property, s16 value) : SkillEffect(SkillEffect::Type::PROPERTY_BONUS), property(property), value(value) { }
  
public:
 
  const Property property;
  const s16 value;
  
  virtual const s16 getValue(Unit* unit) const { return value; }
  
  // TODO: finish
};

class UnitBonus : public PropertyBonus
{
public:
  UnitBonus(Property property, s16 value) : PropertyBonus(property, value) { }
  
  static effect_list  build(std::initializer_list<Property> properties, s16 value)
  {
    std::vector<const SkillEffect*> effects;
    effects.resize(properties.size());
    std::transform(properties.begin(), properties.end(), effects.begin(), [&] (const Property& property) { return new UnitBonus(property, value); });
    return effects;
  }
};

class ArmyBonus : public PropertyBonus
{
public:
  const enum class Type { WHOLE_ARMY, NORMAL_UNITS } target;
  
  ArmyBonus(Property property, s16 value, Type target) : PropertyBonus(property, value), target(target) { }
  
  const s16 getValue(Unit* unit) const;
};





class Effects
{
public:
  static const MovementEffect FORESTWALK, FLYING, UNDERGROUND, MOUNTAINWALK, NON_CORPOREAL, PATH_FINDER, PLANAR_TRAVEL, TELEPORT, SWIMMING, WINDWALK, SAILING, DESERTWALK, SWAMPWALK;
  
  constexpr static const MovementEffect* const MOVEMENT_EFFECTS[] = {&FORESTWALK, &FLYING, &UNDERGROUND, &MOUNTAINWALK, &NON_CORPOREAL, &PATH_FINDER, &PLANAR_TRAVEL, &TELEPORT, &SWIMMING, &WINDWALK, &SAILING, &DESERTWALK, &SWAMPWALK};
};



#endif
