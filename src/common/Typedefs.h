#pragma once

#include <cstdint>

#pragma mark Base integral types
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using byte = u8;
using index_t = u32;

#pragma mark Common integral types
using experience_t = s32;
using prop_value = s32;
using value_t = s32;
using count_t = u32;

#pragma mark Forward Declarations
/* forward declarations */

class Race;
class Game;
class City;
class Army;
class Hero;
class ManaNode;
class Unit;

namespace combat
{
  class Combat;
  class CombatUnit;
}

namespace anims
{
  class Animation;
}

namespace msgs
{
  class Message;
}
