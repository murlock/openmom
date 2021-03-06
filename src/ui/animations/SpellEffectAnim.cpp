#include "SpellEffectAnim.h"

#include "Viewport.h"
#include "GfxData.h"
#include "Font.h"

#include "Unit.h"
#include "ui/views/CombatView.h"

#include "LBX.h"

using namespace std;
using namespace anims;

#pragma mark SpellEffect

SpellEffect::SpellEffect(LocalPlayer* player, SpriteInfo effectId, Position tile) : DiscreteAnimation(TICKS_PER_FRAME*effectId.count()), effect(effectId), frame(0), counter(0)
{
  u16 w = effectId.sw(), h = effectId.sh();
  
  coords = Viewport::screenCoordsForTile(player, tile);
  coords.x = coords.x - w/2 + Viewport::tileWidth/2;
  coords.y = coords.y - h/2 + Viewport::tileHeight/2;
  
  force = coords.isValid();
}

SpellEffect::SpellEffect(SpriteInfo effectId, combat::CombatCoord tile) : DiscreteAnimation(TICKS_PER_FRAME*effectId.count()), effect(effectId), frame(0), counter(0), force(false)
{
  u16 w = effectId.sw(), h = effectId.sh();

  coords = CombatView::coordsForTile(tile.x, tile.y) + Point(CombatView::TILE_WIDTH/2 - w/2 - 1, - h + CombatView::TILE_HEIGHT);
}

SpellEffect::SpellEffect(LocalPlayer* player, SpriteInfo effectId, int tx, int ty)
: DiscreteAnimation(TICKS_PER_FRAME*effectId.count()), effect(effectId), frame(0), counter(0), force(false)
{
  coords = Point(tx-15, ty-25);
}

void SpellEffect::step()
{
  if (!force)
  {
    Gfx::draw(effect.frame(elapsed / TICKS_PER_FRAME), coords);
  }
}

#pragma mark SummonEffect

SummonAnimation::SummonAnimation(const Wizard* wizard, const SummonSpec* spec)
: ContinuousEndlessAnimation(5000), spec(spec), wizard(GfxData::wizardGfx(wizard).summonPose), creature(GfxData::unitGfx(spec).fantastic.summonFigure),
  palette(GfxData::schoolGfxSpec(spec->as<SummonSpec>()->school).summonPalette.palette())
{
}

void SummonAnimation::step()
{
  Point base = Point(30, 42);
  u16 creatureTopY = 18;
  u16 creatureBottomY = 100;

  float percent = position();
  u16 creatureY = creatureBottomY - (creatureBottomY - creatureTopY)*percent;
  
  Gfx::draw(LSI(SPELLSCR,9), palette, base.x, base.y); // BG
  
  u16 frame = (Gfx::fticks % 15); //TODO: should use directly drawAnimated but it's a SpriteSheet not sprite info
  
  Gfx::drawClipped(wizard, base.x + 8, base.y + 3 + 5, 0, 5, wizard.sw(), wizard.sh()-5);
  Gfx::draw(LSI(SPELLSCR,10), palette, base.x + 55, base.y + 0+97-43, frame); // flame 2
  Gfx::drawClipped(creature, base.x + 76, base.y + creatureY, 0, 0, creature.sw(), std::min((int)creatureBottomY - creatureY, (int)creature.sh()));
  Gfx::draw(LSI(SPELLSCR,11), palette, base.x + 65, base.y + 26+97-43, frame); // flame1
  
  //TODO: palette is not the same, single pixels different
  Fonts::drawString(fmt::sprintf("%s Summoned", spec->productionName()), FontFaces::Serif::GOLD_SHADOW, base.x + 90, base.y + 116, ALIGN_CENTER);
}

bool SummonAnimation::mouseReleased(u16 x, u16 y, MouseButton b) { finish(); return true; }

#pragma mark SpellDiscover

SpellDiscoverAnimation::SpellDiscoverAnimation(const Wizard* wizard, School school)
: ContinuousEndlessAnimation(1000), wizard(GfxData::wizardGfx(wizard).researchPose), familiar(GfxData::schoolGfxSpec(school).researchFamiliar)
{
}

void SpellDiscoverAnimation::step()
{
  SpriteInfo bg = LSI(WIZLAB, 19);
  const Palette* palette = bg.palette();
  
  Gfx::draw(bg, 0, 0);

  u16 frame = (Gfx::fticks % 10); //TODO: should use directly drawAnimated but it's a SpriteSheet not sprite info
  
  Gfx::draw(wizard, 69, 75);
  
  Gfx::bindPalette(palette);
  Gfx::draw(LSI(WIZLAB,21).frame(frame), 132, -3); // beam
  Gfx::draw(LSI(WIZLAB,20), 149, 133); // podium
  Gfx::draw(familiar, 190, 160); // familiar
  Gfx::unbindPalette();
}


bool SpellDiscoverAnimation::mouseReleased(u16 x, u16 y, MouseButton b) { finish(); return true; }

