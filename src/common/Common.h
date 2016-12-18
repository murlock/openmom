#ifndef _COMMON_H_
#define _COMMON_H_

#include <cassert>
#include <cstdint>
#include "SDL.h"

#ifdef DEBUG
  extern void debugprintf(const char* str, ...);
  #define LOGD(...) debugprintf(__VA_ARGS__);

  #if DEBUG == 2
    #define LOGD2(...) debugprintf(__VA_ARGS__);
  #else
    #define LOGD2(...) do { } while (false);
  #endif
#else
  #define LOGD(...) do { } while (false);
#endif

#define WIDTH (320)
#define HEIGHT (200)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u32 Color;

using lbx_index = s16;

constexpr Color RGB(u32 r, u32 g, u32 b) { return 0xFF000000 | (r << 16) | (g << 8) | b; }
constexpr Color RGB16(u32 r, u32 g, u32 b) { return RGB(r<<2, g<<2, b<<2); }
constexpr Color RGBA(u32 r, u32 g, u32 b, u32 a) { return (a << 24) | (r << 16) | (g << 8) | b; }
constexpr u8 GET_RED(Color color) { return (color & 0x00FF0000) >> 16; }
constexpr u8 GET_GREEN(Color color) { return (color & 0x0000FF00) >> 8; }
constexpr u8 GET_BLUE(Color color) { return (color & 0x000000FF); }

enum class I18 : u16;
enum class TextureID : u16;
enum Plane : u8;

enum SpellKind : u8;

enum MouseButton : u8
{
  BUTTON_LEFT = SDL_BUTTON_LEFT,
  BUTTON_RIGHT = SDL_BUTTON_RIGHT,
  BUTTON_MIDDLE = SDL_BUTTON_MIDDLE
};

enum class LBXID : u16;
struct LBXSpriteInfo
{
  LBXID lbx;
  u16 index;
  LBXSpriteInfo(LBXID lbx, u16 index) : lbx(lbx), index(index) { }
  
  LBXSpriteInfo relative(s16 delta) const { return LBXSpriteInfo(lbx, index+delta); }
};

enum class LBXID : u16
{
  ARMYLIST = 0,
  BACKGRND,
  BOOK,
  BUILDDAT,
  BUILDESC,
  CHRIVER,
  CITYNAME,
  CITYSCAP,
  CITYWALL,
  CMBDESRC,
  CMBDESRT,
  CMBGRASC,
  CMBGRASS,
  CMBMAGIC,
  CMBMOUNC,
  CMBMOUNT,
  CMBTCITY,
  CMBTFX,
  CMBTSND,
  CMBTUNDC,
  CMBTUNDR,
  CMBTWALL,
  COMBAT,
  COMPIX,
  CONQUEST,
  DESC,
  DESERT,
  DIPLOMAC,
  DIPLOMSG,
  EVENTMSG,
  EVENTS,
  FIGURES1,
  FIGURES2,
  FIGURES3,
  FIGURES4,
  FIGURES5,
  FIGURES6,
  FIGURES7,
  FIGURES8,
  FIGURES9,
  FIGURE10,
  FIGURE11,
  FIGURE12,
  FIGURE13,
  FIGURE14,
  FIGURE15,
  FIGURE16,
  FONTS,
  HALOFAM,
  HELP,
  HERODATA,
  HIRE,
  HLPENTRY,
  INTRO,
  INTROSFX,
  INTROSND,
  ITEMDATA,
  ITEMISC,
  ITEMPOW,
  ITEMS,
  LILWIZ,
  LISTDAT,
  LOAD,
  LOSE,
  MAGIC,
  MAIN,
  MAINSCRN,
  MAPBACK,
  MESSAGE,
  MONSTER,
  MOODWIZ,
  MUSIC,
  NAMES,
  NEWGAME,
  NEWSOUND,
  PORTRAIT,
  RELOAD,
  RESOURCE,
  SCROLL,
  SNDDRV,
  SOUNDFX,
  SPECFX,
  SPECIAL,
  SPECIAL2,
  SPELLDAT,
  SPELLOSE,
  SPELLS,
  SPELLSCR,
  SPLMASTR,
  TERRAIN,
  TERRSTAT,
  TERRTYPE,
  TUNDRA,
  UNITS1,
  UNITS2,
  UNITVIEW,
  VORTEX,
  WALLRISE,
  WIN,
  WIZARDS,
  WIZLAB,
  
  COUNT
};

#define LSI(x, y) LBXSpriteInfo(LBXID::x, y)

constexpr size_t LBX_COUNT = static_cast<size_t>(LBXID::COUNT);

using KeyboardKey = SDL_Scancode;
using KeyboardMod = SDL_Keymod;

struct Position
{
  s16 x, y;
  Plane plane;
  
  Position() { }
  Position(s16 x, s16 y, Plane plane) : x(x), y(y), plane(plane) { }
  
  bool same(const Position& p) const { return p.x == x && p.y == y && p.plane == plane; }
};

struct PositionOffset
{
  s8 x;
  s8 y;
};

struct SpriteInfo
{
  TextureID texture;
  s8 x;
  s8 y;
  
  SpriteInfo(TextureID texture, u8 x, u8 y) : texture(texture), x(x), y(y) { }
};

struct ScreenCoord
{
  s16 x;
  s16 y;
  
  ScreenCoord(s16 x, s16 y) : x(x), y(y) { }
  bool operator==(const ScreenCoord& o) const { return x == o.x && y == o.y; }
};

using Coord = ScreenCoord;

class Palette;

class SpriteSheet
{
public:
  virtual Color at(u16 x, u16 y, u16 c = 0, u16 r = 0) const { return 0x00000000; }
  virtual void set(u16 x, u16 y, Color c) { }
  virtual ~SpriteSheet() { }
  virtual u16 tw() const = 0;
  virtual u16 th() const = 0;
  
  virtual u16 sw(u16 r, u16 c) const = 0;
  virtual u16 sh(u16 r, u16 c) const = 0;
  
  virtual void lock() const { }
  virtual void unlock() const { }
  
  virtual const Palette* getPalette() const { return nullptr; }
};

class Upkeep
{
  public:
    mutable s16 gold, mana, food;
    
  public:
    Upkeep(s16 gold = 0, s16 mana = 16, s16 food = 0) : gold(gold), mana(mana), food(food) { }
    
  public:
    void add(const Upkeep &upkeep)
    {
      gold += upkeep.gold;
      mana += upkeep.mana;
      food += upkeep.food;
    }
};

enum ViewID
{
  VIEW_MAIN,
  VIEW_SPELL_BOOK,
  VIEW_MAGIC,
  VIEW_RESEARCH,
  VIEW_ALCHEMY,
  VIEW_ARMIES,
  VIEW_ARMIES_ITEMS,
  VIEW_ITEM_CRAFT,
  VIEW_CITY,
  VIEW_UNIT,
  VIEW_ARMY,
  VIEW_MESSAGE,
  VIEW_MIRROR,
  VIEW_COMBAT,
  VIEW_PRODUCTION,
  VIEW_OUTPOST,
  VIEW_NEW_GAME,
  
  VIEW_LBX,
  
  VIEW_COUNT
};



#include <vector>

enum School : s8
{
  ARCANE = 0,
  CHAOS,
  DEATH,
  LIFE,
  NATURE,
  SORCERY,
  
  SCHOOL_COUNT,
  
  NO_SCHOOL = -1
};

enum PlayerColor : u8
{
  GREEN = 0,
  BLUE,
  RED,
  PURPLE,
  YELLOW,
  NEUTRAL
};

enum WizardID : u8
{
  MERLIN = 0,
  RAVEN,
  SHAREE,
  LO_PAN,
  JAFAR,
  OBERIC,
  RJAK,
  SSS_RA,
  TAURON,
  FREYA,
  HORUS,
  ARIEL,
  TLALOC,
  KALI
};

struct Wizard
{
  WizardID ident;
  I18 name;
};

enum class TraitID : u8
{
  ALCHEMY = 0,
  WARLORD,
  CHANNELER,
  ARCHMAGE,
  ARTIFICER,
  CONJURER,
  SAGE_MASTER,
  MYRRAN,
  DIVINE_POWER,
  FAMOUS,
  RUNEMASTER,
  CHARISMATIC,
  CHAOS_MASTERY,
  NATURE_MASTERY,
  SORCERY_MASTERY,
  INFERNAL_POWER,
  MANA_FOCUSING,
  NODE_MASTERY
};

struct Trait
{
  const TraitID ident;
  const u8 cost;
  const School school;
};


#pragma mark Map Related

enum Plane : u8
{
  ARCANUS = 0,
  MYRRAN,
  
  PLANE_COUNT
};

enum TileType : u8
{
  TILE_GRASS = 0,
  TILE_WATER,
  TILE_SHORE,
  TILE_MOUNTAIN,
  TILE_VOLCANO,
  TILE_HILL,
  TILE_FOREST,
  TILE_SWAMP,
  TILE_DESERT,
  TILE_TUNDRA,
  TILE_RIVER,
  TILE_RIVER_MOUTH,
  
  TILE_TYPES
};

enum TileGfxType : u8
{
  TILE_GFX_NONE = 0,
  TILE_GFX_ANIMATED,
  TILE_GFX_BORDER,
  TILE_GFX_PLAIN
};

enum class Resource : u8
{
  ADAMANTIUM = 0,
  COAL,
  CRYSX_CRYSTAL,
  GEMS,
  GOLD,
  IRON_ORE,
  MITHRIL,
  NIGHT_SHADE,
  QOURK_CRYSTAL,
  SILVER,
  WILD_GAME,
  NONE
};

enum PlaceType : u8
{
  PLACE_TOWER_OF_WIZARDRY = 0,
  PLACE_TOWER_OF_WIZARDRY_EMPTY,
  PLACE_CAVE,
  PLACE_RUINS,
  PLACE_KEEP,
  PLACE_TEMPLE,
  PLACE_ANCIENT_RUINS
};


class Data
{
  public:
    static const Trait& trait(const TraitID trait);
    static const Wizard& wizard(const WizardID wizard);
};

#endif
