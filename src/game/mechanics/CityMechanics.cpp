//
//  CityMechanics.cpp
//  OpenMoM
//
//  Created by Jack on 7/24/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "CityMechanics.h"

#include "Data.h"
#include "Player.h"
#include "Game.h"
#include "Race.h"
#include "Unit.h"
#include "UnitSpec.h"
#include "Buildings.h"
#include "World.h"
#include "Tile.h"
#include "common/Util.h"
#include "Messages.h"

#include <vector>

using namespace std;

const std::multimap<const Building*, const Building*> CityMechanics::buildingDependsOn = {
  {Building::CITY_WALLS, Building::BUILDERS_HALL},
  
  {Building::SHRINE, Building::BUILDERS_HALL},
  {Building::TEMPLE, Building::SHRINE},
  {Building::PARTHENON, Building::TEMPLE},
  {Building::CATHEDRAL, Building::PARTHENON},
  
  {Building::ANIMISTS_GUILD, Building::STABLE},
  {Building::ANIMISTS_GUILD, Building::UNIVERSITY},

  {Building::ORACLE, Building::PARTHENON},
  {Building::ORACLE, Building::UNIVERSITY},
  
  {Building::ALCHEMISTS_GUILD, Building::SAGES_GUILD},
  
  {Building::WIZARDS_GUILD, Building::ALCHEMISTS_GUILD},
  {Building::WIZARDS_GUILD, Building::UNIVERSITY},
  
  {Building::STABLE, Building::SMITHY},

  {Building::FANTASTIC_STABLE, Building::STABLE},
  {Building::FANTASTIC_STABLE, Building::ANIMISTS_GUILD},
  {Building::FANTASTIC_STABLE, Building::ARMORERS_GUILD},
  
  {Building::ARMORY, Building::SMITHY},
  {Building::ARMORY, Building::BARRACKS},
  
  {Building::FIGHTERS_GUILD, Building::ARMORY},
  {Building::ARMORERS_GUILD, Building::FIGHTERS_GUILD},

  {Building::WAR_COLLEGE, Building::ARMORERS_GUILD},
  {Building::WAR_COLLEGE, Building::UNIVERSITY},
  
  {Building::SHIP_YARD, Building::SHIP_WRIGHTS_GUILD},
  {Building::SHIP_YARD, Building::SAWMILL},
  
  {Building::MARITIME_GUILD, Building::SHIP_YARD},
  {Building::MARITIME_GUILD, Building::ARMORY},
  
  {Building::MARKETPLACE, Building::SMITHY},
  
  {Building::BANK, Building::MARKETPLACE},
  {Building::BANK, Building::UNIVERSITY},
  
  {Building::MERCHANTS_GUILD, Building::BANK},
  {Building::MERCHANTS_GUILD, Building::SHIP_YARD},
  
  {Building::GRANARY, Building::BUILDERS_HALL},

  {Building::FARMERS_MARKET, Building::MARKETPLACE},
  {Building::FARMERS_MARKET, Building::GRANARY},
  
  {Building::LIBRARY, Building::BUILDERS_HALL},
  
  {Building::UNIVERSITY, Building::LIBRARY},
  {Building::UNIVERSITY, Building::SAGES_GUILD},
  
  {Building::SAGES_GUILD, Building::LIBRARY},
  
  {Building::MINERS_GUILD, Building::BUILDERS_HALL},
  
  {Building::MECHANICIANS_GUILD, Building::UNIVERSITY},
  {Building::MECHANICIANS_GUILD, Building::MINERS_GUILD},
  
  {Building::FORESTERS_GUILD, Building::SAWMILL}
};
/*
const std::multimap<UnitID, const Building*> CityMechanics::unitDependsOn = {
  {UnitID::SWORDSMEN, Building::SMITHY},
  {UnitID::HALBERDIERS, Building::ARMORY},
  {UnitID::BOWMEN, Building::BARRACKS}, {UnitID::BOWMEN, Building::SAWMILL},
  {UnitID::CAVALRY, Building::BARRACKS}, {UnitID::CAVALRY, Building::STABLE},
  {UnitID::ENGINEERS, Building::BUILDERS_HALL},
  {UnitID::PIKEMEN, Building::FIGHTERS_GUILD},
  {UnitID::PRIESTS, Building::PARTHENON},
  {UnitID::SHAMANS, Building::SHRINE},
  {UnitID::MAGICIANS, Building::WIZARDS_GUILD},
  
  {UnitID::BERSERKERS, Building::ARMORERS_GUILD},
  {UnitID::ELVEN_LORDS, Building::ARMORERS_GUILD},
  {UnitID::MINOTAURS, Building::ARMORERS_GUILD},
  {UnitID::GOLEM, Building::ARMORERS_GUILD},
  
  {UnitID::STAG_BEETLE, Building::ARMORERS_GUILD}, {UnitID::STAG_BEETLE, Building::STABLE},
  {UnitID::DRAGON_TURTLE, Building::ARMORERS_GUILD}, {UnitID::DRAGON_TURTLE, Building::STABLE},
  {UnitID::WAR_MAMMOTHS, Building::ARMORERS_GUILD}, {UnitID::WAR_MAMMOTHS, Building::STABLE},
  
  {UnitID::PALADINS, Building::ARMORERS_GUILD}, {UnitID::PALADINS, Building::CATHEDRAL},
  
  {UnitID::SLINGERS, Building::ARMORY},
  
  {UnitID::RANGERS, Building::BARRACKS}, {UnitID::RANGERS, Building::ANIMISTS_GUILD},
  {UnitID::MANTICORES, Building::BARRACKS}, {UnitID::MANTICORES, Building::ANIMISTS_GUILD},
  
  {UnitID::LONGBOWMEN, Building::SAWMILL},
  
  {UnitID::DOOM_DRAKES, Building::BARRACKS}, {UnitID::DOOM_DRAKES, Building::STABLE},
  {UnitID::WOLF_RIDERS, Building::BARRACKS}, {UnitID::WOLF_RIDERS, Building::STABLE},
  {UnitID::CENTAURS, Building::BARRACKS}, {UnitID::CENTAURS, Building::STABLE},
  {UnitID::HORSEBOWMEN, Building::BARRACKS}, {UnitID::HORSEBOWMEN, Building::STABLE},
  
  {UnitID::PEGASAI, Building::FANTASTIC_STABLE},
  {UnitID::GRIFFINS, Building::FANTASTIC_STABLE},
  {UnitID::WYVERN_RIDERS, Building::FANTASTIC_STABLE},
  {UnitID::NIGHTMARES, Building::FANTASTIC_STABLE},
  
  {UnitID::JAVELINEERS, Building::FIGHTERS_GUILD},
  {UnitID::NIGHTBLADES, Building::FIGHTERS_GUILD},
  {UnitID::HAMMERHANDS, Building::FIGHTERS_GUILD},
  {UnitID::WAR_TROLLS, Building::FIGHTERS_GUILD},
  
  {UnitID::STEAM_CANNON, Building::MINERS_GUILD},
  {UnitID::AIR_SHIP, Building::SHIP_YARD},
  {UnitID::WARLOCKS, Building::WIZARDS_GUILD}
};*/

/* TODO FIXME: move to yaml
const std::multimap<RaceID, const Building*> CityMechanics::disallowedBuildingsByRace = {
  {RaceID::BARBARIANS, Building::ANIMISTS_GUILD},{RaceID::BARBARIANS, Building::CATHEDRAL}, {RaceID::BARBARIANS, Building::UNIVERSITY},
  {RaceID::BEASTMEN, Building::FANTASTIC_STABLE}, {RaceID::BEASTMEN, Building::SHIP_YARD}, {RaceID::BEASTMEN, Building::WAR_COLLEGE},
  {RaceID::DARK_ELVES, Building::CATHEDRAL},
  {RaceID::DRACONIANS, Building::ANIMISTS_GUILD}, {RaceID::DRACONIANS, Building::MINERS_GUILD},
  {RaceID::DWARVES, Building::PARTHENON}, {RaceID::DWARVES, Building::SHIP_YARD}, {RaceID::DWARVES, Building::STABLE}, {RaceID::DWARVES, Building::UNIVERSITY},
  {RaceID::GNOLLS, Building::ANIMISTS_GUILD}, {RaceID::GNOLLS, Building::LIBRARY}, {RaceID::GNOLLS, Building::PARTHENON},
  {RaceID::HALFLINGS, Building::ARMORERS_GUILD}, {RaceID::HALFLINGS, Building::STABLE}, {RaceID::HALFLINGS, Building::UNIVERSITY},
  {RaceID::HIGH_ELVES, Building::PARTHENON},
  {RaceID::HIGH_MEN, Building::FANTASTIC_STABLE},
  {RaceID::KLACKONS, Building::SAGES_GUILD}, {RaceID::KLACKONS, Building::SHIP_YARD}, {RaceID::KLACKONS, Building::TEMPLE},
  {RaceID::LIZARDMEN, Building::ANIMISTS_GUILD}, {RaceID::LIZARDMEN, Building::ARMORERS_GUILD}, {RaceID::LIZARDMEN, Building::MINERS_GUILD}, {RaceID::LIZARDMEN, Building::PARTHENON}, {RaceID::LIZARDMEN, Building::SAGES_GUILD},
  {RaceID::NOMADS, Building::MARITIME_GUILD}, {RaceID::NOMADS, Building::WIZARDS_GUILD},
  // ORCS
  {RaceID::TROLLS, Building::FANTASTIC_STABLE}, {RaceID::TROLLS, Building::MINERS_GUILD}, {RaceID::TROLLS, Building::SAGES_GUILD}, {RaceID::TROLLS, Building::SHIP_YARD}, {RaceID::TROLLS, Building::UNIVERSITY}
};
 */

const std::map<const Building*, const Building*> CityMechanics::buildingReplacementMap = {
  { Building::BARRACKS, Building::ARMORY },
  
  { Building::FIGHTERS_GUILD, Building::ARMORERS_GUILD },
  { Building::ARMORERS_GUILD, Building::WAR_COLLEGE },
  
  { Building::STABLE, Building::ANIMISTS_GUILD },
  
  { Building::SHIP_WRIGHTS_GUILD, Building::SHIP_YARD },
  { Building::MARITIME_GUILD, Building::SHIP_WRIGHTS_GUILD },
  
  { Building::LIBRARY, Building::SAGES_GUILD },
  
  { Building::ALCHEMISTS_GUILD, Building::UNIVERSITY },
  
  { Building::SHRINE, Building::TEMPLE },
  { Building::TEMPLE, Building::PARTHENON },
  { Building::PARTHENON, Building::CATHEDRAL },
  
  { Building::MARKETPLACE, Building::BANK },
  { Building::BANK, Building::MERCHANTS_GUILD },
  
  { Building::GRANARY, Building::FARMERS_MARKET },
};

bool CityMechanics::isBuildingCurrentlyReplaced(const City *city, const Building* building)
{
  auto it = buildingReplacementMap.find(building);
  
  /* if building can have a replacement */
  while (it != buildingReplacementMap.end())
  {
    /* if city has the replacement return true */
    if (city->hasBuilding(it->second))
      return true;
    /* otherwise traverse to the possible replacement of the replacement and try again */
    else
      it = buildingReplacementMap.find(it->second);
  }
  
  return false;
}


bool CityMechanics::isBuildingAllowed(const City* city, const Building* building)
{
  if (!isBuildingAllowedForRace(city, building) || !isBuildingAllowedForTerrain(city, building))
    return false;
  
  if (city->hasBuilding(building) || building == Building::SUMMONING_CIRCLE || building == Building::MAGE_FORTRESS)
    return false;

  auto it = buildingDependsOn.equal_range(building);
  return std::all_of(it.first, it.second, [city](decltype(*it.first)& iit) { return city->hasBuilding(iit.second); });
}

bool CityMechanics::isBuildingAllowedForTerrain(const City *city, const Building* building)
{
  vector<TileType> requirement;
  
  // TODO: check if correct
  if (building == Building::STABLE) requirement.push_back(TileType::GRASS);
  else if (building == Building::SHIP_WRIGHTS_GUILD) requirement = {TileType::SHORE, TileType::RIVER, TileType::RIVER_MOUTH, TileType::OCEAN};
  else if (building == Building::MINERS_GUILD) requirement = {TileType::HILL, TileType::MOUNTAIN, TileType::VOLCANO};
  else if (building == Building::SAWMILL) requirement = {TileType::FOREST};
  
  if (!requirement.empty())
  {
    for (auto it : requirement)
      if (countSurroundTileType(city, it) > 0)
        return true;
    
    return false;
  }
  else
    return true;
}

bool CityMechanics::isBuildingAllowedForRace(const City* city, const Building* building)
{
  // TODO FIXME: reactivate, removed to introduc new races management with YAML
  /*auto it = disallowedBuildingsByRace.equal_range(city->race->ident);
  
  for (auto iit = it.first; iit != it.second; ++iit)
    if (iit->second == building)
      return false;*/

  return true;
}

bool CityMechanics::isUnitAllowed(const City* city, const RaceUnitSpec *unit)
{
  const auto it = Data::requiredBuildingsForUnit(unit);
  return std::all_of(it.first, it.second, [city](const decltype(it.first)::value_type& entry) { return city->hasBuilding(entry.second); });
}

const list<const RaceUnitSpec*> CityMechanics::availableUnits(const City* city)
{
  vector<const RaceUnitSpec*> allUnits = Data::unitsForRace(city->race);
  list<const RaceUnitSpec*> units;

  copy_if(allUnits.begin(), allUnits.end(), back_inserter(units), [&](const RaceUnitSpec* spec) { return isUnitAllowed(city, spec); });
  
  return units;
}

const list<const Building*> CityMechanics::availableBuildings(const City* city)
{
  list<const Building*> buildings;
  
  for (size_t i = 0; i < Building::COUNT; ++i)
    if (isBuildingAllowed(city, Building::buildings[i]))
      buildings.push_back(Building::buildings[i]);
  
  return buildings;
}


// FIXME: something is wrong here, must be redesigned to use yaml in any case
list<const Productable*> CityMechanics::itemsUnlockedByBuilding(const Building* building, const City *city)
{
  LOGD3("[city] computing unlocked entries by %s", i18n::c(building->name));
  
  list<const Productable*> items;
  
  //TODO: remove ancestors
  for (const auto it : buildingDependsOn)
  {
    auto iit = buildingDependsOn.equal_range(it.first); // all the dependencies of current building

    // if building was a dependency of it
    for (auto iitb = iit.first; iitb != iit.second; ++iitb)
    {
      if (iitb->second == building)
      {
        bool wholeTree = true;
        list<const Building*> dependTree;
        dependTree.push_back(it.first);
        
        while (!dependTree.empty())
        {
          const Building* cb = dependTree.front();
          dependTree.pop_front();
          
          if (!isBuildingAllowedForRace(city, cb) || !isBuildingAllowedForTerrain(city, cb))
          {
            wholeTree = false;
            break;
          }
          
          auto dependsOn = buildingDependsOn.equal_range(cb);
          for (auto dit = dependsOn.first; dit != dependsOn.second; ++dit)
            dependTree.push_back(dit->second);
        }
        
        if (wholeTree)
          items.push_back(it.first);
        
        break;
      }
    }
  }
  
  vector<const RaceUnitSpec*> units = Data::unitsForRace(city->race);
  // TODO: check behavior
  for (auto unit : units)
  {
    auto iit = Data::requiredBuildingsForUnit(unit);    
    for (auto iit2 = iit.first; iit2 != iit.second; ++iit2)
      if (iit2->second == building)
        items.push_back(unit);
  }
  // TODO: check behavior

  items.erase(std::unique(items.begin(), items.end()), items.end());

  return items;
}

void CityMechanics::lambdaOnCitySurroundings(const City* city, const std::function<void(Tile*)>& functor)
{
  for (int x = -2; x <= 2; ++x)
    for (int y = -2; y <= 2; ++y)
      if ((x != 2 && x != -2) || (y != 2 && y != -2))
      {
        Tile* tile = game->world->get(city->position.x + x, city->position.y + y, city->position.plane);
        if (tile)
          functor(tile);
      }
}


s16 CityMechanics::countSurroundTileType(const City* city, TileType type)
{
  s16 count = 0;
  lambdaOnCitySurroundings(city, [&count, type](const Tile* tile) {
    count += tile->type == type ? 1 : 0;
  });
  return count;
}

s16 CityMechanics::countSurroundResource(const City* city, Resource type)
{
  s16 count = 0;
  lambdaOnCitySurroundings(city, [&count, type](const Tile* tile) {
    count += tile->resource == type ? 1 : 0;
  });
  return count;
}

s16 CityMechanics::countSurroundManaNode(const City* city, School school)
{
  s16 count = 0;
  lambdaOnCitySurroundings(city, [&count, school](const Tile* tile) {
    count += tile->node() && tile->node()->school == school ? 1 : 0;
  });
  return count;
}

float CityMechanics::resourceBonus(const City* city, Resource resource, float value)
{
  s16 count = countSurroundResource(city, resource);
  
  float ret = value*count;
  
  if (city->hasBuilding(Building::MINERS_GUILD))
    ret += ret*0.5f;
  
  return ret;
}


s16 CityMechanics::computeInitialPopulationGrowth(const City* city)
{
  s16 populationDelta = 0;
  
  // let's compute the chance to increase outpost population
  float increaseChance = 0;
  
  Tile* t = game->world->get(city->position);
  
  // according to race there is a base chance of growing
  increaseChance += city->race->outpostGrowthChance;
  // chance is increased according to maximum population of city (+1% per point of maximum size)
  increaseChance += computeMaxPopulationForTile(t) / 100.0;
  
  // increase chance according to city resources
  // TODO: halved down resources only 1/2 bonus growth?
  lambdaOnCitySurroundings(city, [&increaseChance] (const Tile* tile) {
    if (tile->resource != Resource::NONE)
    {
      switch (tile->resource)
      {
        case Resource::IRON_ORE: increaseChance += 0.05f; break;
        case Resource::SILVER: increaseChance += 0.05f; break;
        case Resource::COAL: increaseChance += 0.10f; break;
        case Resource::GEMS: increaseChance += 0.10f; break;
        case Resource::GOLD: increaseChance += 0.10f; break;
        case Resource::MITHRIL: increaseChance += 0.10f; break;
        case Resource::ADAMANTIUM: increaseChance += 0.10f; break;
        case Resource::QOURK_CRYSTAL: increaseChance += 0.10f; break;
        case Resource::CRYSX_CRYSTAL: increaseChance += 0.10f; break;
      }
    }
  });
  
  // TODO: if city has Stream of Life +10% - if city has Gaia's Blessing +20%
  
  if (Math::chance(increaseChance))
    populationDelta += Math::randomIntInclusive(1, 3)*100;
  
  
  // let's compute the chance to decrease output population
  float decreaseChance = 0.05f;
  
  // TODO:
  // +5% if evil presence spell
  // +10% if pestilence spell
  // +10% if famine spell
  // +10% if chaos rift spell
  
  if (Math::chance(decreaseChance))
    populationDelta -= Math::randomIntInclusive(1, 2)*100;
  
  return populationDelta;
}


Upkeep CityMechanics::computeUpkeep(const City* city)
{
  Upkeep u;
  for (auto b : city->buildings)
    u += b->upkeep;
  return u;
}

s16 CityMechanics::baseGold(const City* city)
{
  // compute base gold income from taxes
  s16 bgi = (s16)std::floor((city->population/1000) * city->owner->getTaxRate().goldPerCitizen);
  
  // add bonus gold from resources surrounding the city
  int bonus = 0;
  bonus += resourceBonus(city, Resource::GEMS, 5);
  bonus += resourceBonus(city, Resource::GOLD, 3);
  bonus += resourceBonus(city, Resource::SILVER, 2);
  
  // if city has miner's guild then bonus from ores is increased by 50%
  if (city->hasBuilding(Building::MINERS_GUILD))
    bonus = (s16)std::floor(bonus + bonus*0.50);
  
  bgi += bonus;
  
  // dwarvens double their tax income
  bgi *= city->race->taxIncomeMultiplier;

  return bgi;
}

s16 CityMechanics::computeGold(const City *city)
{
  // first compute base gold income
  s16 base = baseGold(city);
  s16 totalGold = base;
  
  // boost gold income according to buildings
  if (city->hasBuilding(Building::MARKETPLACE))
    totalGold += (s16)std::floor(base*0.5f);
  if (city->hasBuilding(Building::BANK))
    totalGold += (s16)std::floor(base*0.5f);
  if (city->hasBuilding(Building::MERCHANTS_GUILD))
    totalGold += base;
  
  // TODO: shore/river/river mouth/road bonuses
  
  // TODO: trade goods
  
  return totalGold;
}

s16 CityMechanics::baseProduction(const City *city)
{
  int multiplier = city->race->baseProduction;
  
  return (city->workers*multiplier) + (s16)std::ceil(city->farmers/2.0);
}

s16 CityMechanics::computeProductionBonus(const City *city)
{
  float bonus = 0.0f;
  
  // bonuses given by buildings
  if (city->hasBuilding(Building::SAWMILL))
    bonus += 0.25f;
  if (city->hasBuilding(Building::FORESTERS_GUILD))
    bonus += 0.25f;
  if (city->hasBuilding(Building::MINERS_GUILD))
    bonus += 0.50f;
  if (city->hasBuilding(Building::MECHANICIANS_GUILD))
    bonus += 0.50f;
  
  // bonuses given by terrain features
  bonus += countSurroundTileType(city, TileType::HILL)*0.03f; // TODO: broken, check
  bonus += countSurroundTileType(city, TileType::HILL)*0.03f;
  bonus += countSurroundTileType(city, TileType::HILL)*0.03f;
  bonus += countSurroundTileType(city, TileType::HILL)*0.05f;
  bonus += countSurroundManaNode(city, CHAOS)*0.05f;
  bonus += countSurroundManaNode(city, NATURE)*0.03f;
  
  return bonus;
}

s16 CityMechanics::computeProduction(const City* city)
{
  float production = baseProduction(city);
  float bonus = computeProductionBonus(city);
  
  production += production*bonus;
  
  return (s16)std::floor(production);
}

s16 CityMechanics::computeMana(const City *city)
{
  s16 totalMana = 0;
  
  // if city has fortress and it's on myrran then +5 mana bonus
  if (city->hasBuilding(Building::MAGE_FORTRESS))
  {
    totalMana += city->owner->book()->totalBooks();
    if (city->position.plane == MYRRAN)
      totalMana += 5;
  }
  
  // mana bonuses given by racial traits
  totalMana += (s16)std::floor((city->population/1000)*city->race->manaProducedPerCitizen);
  
  // mana bonuses given by buildings
  if (city->hasBuilding(Building::SHRINE))
    totalMana += 1;
  if (city->hasBuilding(Building::TEMPLE))
    totalMana += 2;
  if (city->hasBuilding(Building::PARTHENON))
    totalMana += 3;
  if (city->hasBuilding(Building::CATHEDRAL))
    totalMana += 4;
  if (city->hasBuilding(Building::ALCHEMISTS_GUILD))
    totalMana += 3;
  
  // mana bonuses given by resources surrounding the city
  s16 bonus = 0;
  bonus += (s16)std::floor(resourceBonus(city, Resource::CRYSX_CRYSTAL, 5));
  bonus += (s16)std::floor(resourceBonus(city, Resource::QOURK_CRYSTAL, 3));
  bonus += (s16)std::floor(resourceBonus(city, Resource::ADAMANTIUM, 2));
  bonus += (s16)std::floor(resourceBonus(city, Resource::MITHRIL, 1));
  
  // dwarvens gets double bonuses from these resources
  bonus *= city->race->miningBonusMultiplier;
  
  return totalMana + bonus;
}

s16 CityMechanics::computeKnowledge(const City *city)
{
  int knowledge = 0;
  
  if (city->hasBuilding(Building::LIBRARY))
    knowledge += 2;
  if (city->hasBuilding(Building::SAGES_GUILD))
    knowledge += 3;
  if (city->hasBuilding(Building::UNIVERSITY))
    knowledge += 5;
  if (city->hasBuilding(Building::WIZARDS_GUILD))
    knowledge += 8;
  
  return knowledge;
}

s16 CityMechanics::baseGrowthRate(const City *city)
{
  return (s16)std::ceil((city->maxPopulation - city->population/1000 + 1) / 2.0);
}

s16 CityMechanics::computeGrowthRate(const City *city)
{
  if (city->population == 25000)
    return 0;
  
  // base growth rate
  s16 growthRate = baseGrowthRate(city)*10 + city->race->growthBonus;
  
  // bonus from buildings
  if (city->hasBuilding(Building::GRANARY))
    growthRate += 20;
  if (city->hasBuilding(Building::FARMERS_MARKET))
    growthRate += 30;
  
  // housing bonus
  if (city->production == Building::HOUSING)
  {
    float ratio = city->workers / (city->population/1000);
    
    if (city->hasBuilding(Building::SAWMILL))
      ratio += 0.10f;
    if (city->hasBuilding(Building::BUILDERS_HALL))
      ratio += 0.15f;
    
    growthRate += 10*((s16)(ratio*(growthRate)/10));
  }
  
  return growthRate;
}

s16 CityMechanics::computeMaxPopulationForTile(const Tile* tile)
{
  float maxPop = 0.0f;
  
  for (int x = -2; x <= 2; ++x)
    for (int y = -2; y <= 2; ++y)
      if ((x != 2 && x != -2) || (y != 2 && y != -2))
      {
        if (tile)
        {
          Tile* t = game->world->get(tile->position.x + x, tile->position.y + y, tile->position.plane);
          if (t->resource == Resource::WILD_GAME)
            maxPop += 2.0f;
          else if (t->node() && (t->node()->school == SORCERY || t->node()->school == NATURE))
            maxPop += 2.0f;
          
          if (!t->node())
            switch (t->type)
            {
              case TileType::GRASS: maxPop += 1.5f; break;
              case TileType::RIVER: maxPop += 2.0f; break;
              case TileType::RIVER_MOUTH: maxPop += 2.0f; break;
              case TileType::HILL: maxPop += 0.5f; break;
              case TileType::SWAMP: maxPop += 0.5f; break;
              case TileType::FOREST: maxPop += 0.5f; break;
              case TileType::SHORE: maxPop += 0.5f; break;
            }
        }
      }
  
  return std::min(static_cast<int>(std::ceil(maxPop)), 25);
}

float CityMechanics::getRacialUnrest(const Race *cityRace, const Race *ownerRace)
{
  return 0.1f;
}

s16 CityMechanics::computeUnrest(const City *city)
{
  float globalMultiplier = 1.0f;
  float percentUnrest = 0.0f;
  s16 flatUnrest = 0;
  
  /* unrest given by competition between city race and fortress city race */
  float racialUnrest = getRacialUnrest(city->race, city->owner->cityWithFortress()->race);
  /* unrest given by taxes */
  float taxUnrest = city->owner->getTaxRate().unrestPercent;
  
  percentUnrest = std::max(0.0f, racialUnrest + taxUnrest);
  
  /* subtract a flat unrest for each 2 normal units in city */
  const Army* army = game->getArmyAtTile(city->getPosition());
  if (army)
  {
    const auto normalUnits = army->getUnits([](const Unit* unit) { return !unit->isFantastic(); });
    s16 suppressionFromArmy = normalUnits.size() / 2;
    
    flatUnrest -= suppressionFromArmy;
  }
  
  /*
   dark rituals +1
   cursed lands +1
   perstilence +2
   famine +25% (on global multiplier? )
   great wasting +1
   armageddon +2 ( not cumulative with great wasting )
   
   shrine parthenon animist's guild -1
   oracle -2
   temple cathedral -1 (-2 with divine power of infernal power retort )
   
   just cause -1 all towns
   stream of life forces unrest to 0
   gaia's blessing -2
   */
  
  /* ( (population * percentUnrest) + flatUnrest ) * globalMultiplier */
  return std::max(0, (int)std::floor(((city->getPopulationInThousands()*percentUnrest) + flatUnrest) * globalMultiplier));
}

s16 CityMechanics::computeMaxPopulation(const City *city)
{
  float maxPop = computeMaxPopulationForTile(game->world->get(city->position));
  
  if (city->hasBuilding(Building::GRANARY))
    maxPop += 2.0f;
  if (city->hasBuilding(Building::FARMERS_MARKET))
    maxPop += 3.0f;
  
  return std::min(static_cast<int>(std::ceil(maxPop)), 25);
}

void CityMechanics::partitionPopulation(City* city)
{
  // TODO: check why this was done with Animists' Guild
  //int foodPerPopulation = (city->race->ident == RaceID::HALFLINGS) || city->hasBuilding(Building::ANIMISTS_GUILD) ? 3 : 2;
  int foodPerCitizen = city->race->foodProductionPerFarmer;
  
  /* TODO: for now it just removes required farmers and set the remainders half workers and half farmers */
  city->reservedPopulation =  city->necessaryFood / foodPerCitizen;
  city->farmers = (city->population/1000 - city->reservedPopulation)/2;
  city->workers = (city->population/1000 - city->reservedPopulation)/2;
  city->unrest = 0;
}


void CityMechanics::buyProduction(City *city)
{
  city->owner->spendGold((city->production->productionCost() - city->productionPool)*4);
  city->productionPool = city->production->productionCost();
}

bool CityMechanics::isProductionBuyable(const City *city)
{
  return city->production != Building::HOUSING && city->production != Building::TRADE_GOODS && city->owner->totalGoldPool() > (city->production->productionCost() - city->productionPool)*4;
}

void CityMechanics::updateProduction(City *c)
{
  if (c->isStillOutpost || c->production == Building::HOUSING || c->production == Building::TRADE_GOODS)
    return;
  
  // update with new production
  c->productionPool += c->work;
  
  // check if unit/building is ready and actually generate it
  if (c->productionPool > c->production->productionCost())
  {
    if (c->production->productionType() == Productable::Type::BUILDING)
    {
      c->addBuilding(static_cast<const Building*>(c->production));
      c->getOwner()->send(new msgs::NewBuilding(c, static_cast<const Building*>(c->production)));
      c->production = Building::HOUSING;
      c->productionPool = 0;
      updateValues(c);
    }
    else
    {
      Tile* t = game->world->get(c->position);
      Army* a = t->army;
      
      if (!a)
        t->placeArmy(new Army(c->owner, {new RaceUnit(static_cast<const RaceUnitSpec*>(c->production))}));
      else if (a && a->size() < 9)
      {
        a->add(new RaceUnit(static_cast<const RaceUnitSpec*>(c->production)));
        a->unpatrol();
      }
      else
      {
        bool done = false;
        
        for (const auto& delta : Util::DIRS)
        {
          t = game->world->get(c->position, delta);
          a = t->army;
          
          if (a && a->getOwner() == c->owner && a->size() < 9)
          {
            a->add(new RaceUnit(static_cast<const RaceUnitSpec*>(c->production)));
            done = true;
            break;
          }
        }
        
        if (!done)
        {
          for (const auto& delta : Util::DIRS)
          {
            t = game->world->get(c->position, delta);
            a = t->army;
            
            if (!a)
            {
              t->placeArmy(new Army(c->owner, {new RaceUnit(static_cast<const RaceUnitSpec*>(c->production))}));
              done = true;
              break;
            }
          }
        }
      }
      
      c->productionPool = 0;
      updateValues(c);
      game->playerMechanics.updatePools(c->owner);
      game->playerMechanics.updateGlobalGains(c->owner);
    }
  }
}

u16 CityMechanics::turnsRequiredForProduction(const City* city)
{
  // TODO: if housing or trade goods should return 1?
  
  if (!city->production)
    return 0;
  else
  {
    int missing = city->production->productionCost() - city->productionPool;
    return std::min(1, Math::roundWithMod(missing, city->work));
  }
}

void CityMechanics::growCity(City *c)
{
  if (!c->isStillOutpost)
    c->population = std::min(c->population + c->growthRate, 25000);
  else
  {
    c->population += computeInitialPopulationGrowth(c);
    
    if (c->population > 1000)
    {
      c->isStillOutpost = false;
      c->getOwner()->send(new msgs::Error(fmt::sprintf(i18n::s(I18::MESSAGE_OUTPOST_GROWN_TO_CITY), c->getName())));
    }
  }
}

bool CityMechanics::canRoadBeBuiltOnTile(const Tile* tile)
{
  //TODO: check
  return (tile && tile->isSolidLand());
}

bool CityMechanics::canCityBeBuiltOnTile(const Tile *t)
{
  World* w = game->world;
  
  if (!t || t->city || (t->type == TileType::OCEAN || t->type == TileType::SHORE))
    return false;
  
  for (int i = -3; i <= 3; ++i)
    for (int j = -3; j <= 3; ++j)
    {
      Tile* t2 = w->get(t->position.x+i, t->position.y+j, t->position.plane);
      
      if (t2 && t2->city)
        return false;
    }
  
  return true;
}
