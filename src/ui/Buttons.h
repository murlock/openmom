//
//  Buttons.h
//  OpenMoM
//
//  Created by Jack on 7/19/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include "SDL.h"
#include "common/Common.h"
#include "Texture.h"

#include <string>
#include <functional>
#include <vector>
#include <memory>

class FontSpriteSheet;

typedef std::function<void()> Action;

class ClickableGrid
{
  s32 x, y, w, h;
  s32 r, c;

public:
  ClickableGrid(s32 x, s32 y, s32 w, s32 h, s32 r, s32 c) :
  x(x), y(y), w(w), h(h), r(r), c(c) { }
  
  bool isInside(const Point& p) const
  {
    return p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h);
  }
  
  Point getCell(const Point& p)
  {
    if (isInside(p))
      return Point((p.x - x) / c, (p.y - y) / r);
    else
      return Point(-1, -1);
  }
};

class Clickable
{
protected:
  u16 x, y, w, h;
  const MouseButton button;
  Action action;
  Action onEnter;
  Action onExit;
  bool active;
  
public:
  Clickable(u16 x, u16 y, u16 w, u16 h, MouseButton b = BUTTON_LEFT) :
    x(x), y(y), w(w), h(h), button(b), active(true),
    onEnter([](){}), onExit([](){})
  { }
  
  bool isCorrectButton(MouseButton b) { return b == button; }
  bool isInside(u16 x, u16 y) { return x >= this->x && x <= this->x+w && y >= this->y && y <= this->y+h; }
  virtual bool isActive() { return active; }
  inline void activate() { active = true; }
  inline void deactivate() { active = false; }
  inline void activateIf(bool condition) { active = condition; }
  
  virtual void setPosition(u16 x, u16 y) { this->x = x; this->y = y; }
  
  virtual inline Clickable* setAction(Action action) { this->action = action; return this; }
  inline Action getAction() { return action; }
  
  void setOnEnterAction(Action action) { this->onEnter = action; }
  void setOnExitAction(Action action) { this->onExit = action; }
  
  inline Action getOnEnter() { return onEnter; }
  inline Action getOnExit() { return onExit; }
  
  virtual void draw();
};

template<typename T>
class clickable_list
{
private:
  std::vector<std::unique_ptr<T>> areas;
  
public:
  clickable_list() { }
  clickable_list(size_t r) { areas.resize(r); }
  
  size_t size() const { return areas.size(); }
  void emplace_back(T* area) { areas.emplace_back(area); }
  void clear() { areas.clear(); }
  
  void draw() { for (const auto& area : areas) area->draw(); }
  bool handleEvent(u16 x, u16 y, MouseButton b)
  {
    for (const auto& area : areas)
      if (area->isActive() && area->isCorrectButton(b) && area->isInside(x,y))
      {
        area->getAction()();
        return true;
      }
    
    return false;
  }
  
  using iterator = typename decltype(areas)::iterator;
  using const_iterator = typename decltype(areas)::const_iterator;
  
  const_iterator begin() const { return areas.begin(); }
  const_iterator end() const { return areas.end(); }
};

struct ButtonGfx
{
  const Palette* palette;
  optional<SpriteInfo> normal;
  optional<SpriteInfo> pressed;
  optional<SpriteInfo> inactive;
  bool shouldOffsetNormal;
  
  void draw(u16 x, u16 y, bool isActive, bool isPressed) const;
  
  ButtonGfx() : palette(nullptr) { }
  ButtonGfx(SpriteInfo normal, bool shouldOffsetNormal = false) : normal(normal), palette(nullptr), shouldOffsetNormal(shouldOffsetNormal) { }
  ButtonGfx(SpriteInfo normal, SpriteInfo pressed) : normal(normal), pressed(pressed), palette(nullptr) { }
  ButtonGfx(SpriteInfo normal, SpriteInfo pressed, SpriteInfo inactive) : normal(normal), pressed(pressed), inactive(inactive), palette(nullptr) { }
};

struct TextInfo
{
  std::string label;
  const FontSpriteSheet* font;
  Point position;
  
  TextInfo() = default;
  TextInfo(const std::string& label, const FontSpriteSheet* font) : label(label), font(font) { }
  TextInfo(const std::string& label, Point position) : label(label), font(nullptr), position(position) { }
  TextInfo(const std::string& label, const FontSpriteSheet* font, Point position) : label(label), font(font), position(position) { }

};

class Button : public Clickable
{
protected:
  const std::string name;
  bool pressed, visible;
  
  ButtonGfx gfx;
  optional<TextInfo> labelGfx;
  uintptr_t data;
  
public:
  Button(const std::string& name, u16 x, u16 y, u16 w, u16 h) : Clickable(x, y, w, h), name(name), pressed(false), visible(true) { }
  Button(const std::string& name, u16 x, u16 y, SpriteInfo normal, bool shouldOffsetNormal) : Clickable(x, y, normal.sw(), normal.sh()), name(name), pressed(false), visible(true), gfx(normal, shouldOffsetNormal) { }
  Button(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo pressed) : Clickable(x, y, normal.sw(), normal.sh()), name(name), pressed(false), visible(true), gfx(normal, pressed) { }
  Button(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo pressed, SpriteInfo inactive) : Clickable(x, y, normal.sw(), normal.sh()), name(name), pressed(false), visible(true), gfx(normal, pressed, inactive) { }

  virtual void setTextInfo(const TextInfo& info);
  virtual void setLabel(const std::string& string);
  
  void draw() override;
  void setPosition(u16 x, u16 y) override;
  
  void setPalette(const Palette* palette) { gfx.palette = palette; }
  
  inline void execute() { if (action) action(); }
  virtual void click() { execute(); }
  
  inline void showIf(bool condition) { visible = condition;}
  inline Button* hide() { visible = false; return this; }
  inline Button* show() { visible = true; return this; }
  inline bool isVisible() { return visible;}
  
  bool isActive() override { return active && visible; }
  inline Button* setAction(Action action) override { this->action = action; return this; }

  template<typename T> T* as() { return static_cast<T*>(this); }
  
  inline void press() { pressed = true;}
  inline void release() { pressed = false; }
  
  template<typename T, typename std::enable_if<std::is_integral<T>::value && sizeof(T) <= sizeof(uintptr_t), int>::type = 0> inline void setData(T data) { this->data = data; }
  template<typename T, typename std::enable_if<std::is_integral<T>::value && sizeof(T) <= sizeof(uintptr_t), int>::type = 0> inline void getData() const { return static_cast<T>(data); }
  
  static Button* buildSimple(const std::string& name, u16 x, u16 y, SpriteInfo normal) { return new Button(name, x, y, normal, false); }
  
  static Button* buildPressedOnly(const std::string& name, u16 x, u16 y, SpriteInfo pressed) {
    auto* bt = new Button(name, x, y, pressed.sw(), pressed.sh());
    bt->gfx.pressed = pressed;
    return bt;
  }
  
  static Button* buildOffsetted(const std::string& name, u16 x, u16 y, SpriteInfo normal) { return new Button(name, x, y, normal, true); }
  static Button* buildOffsetted(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo inactive) { auto* bt = new Button(name, x, y, normal, true); bt->gfx.inactive = inactive; return bt; }
  
  static Button* buildBistate(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo pressed) { return new Button(name, x, y, normal, pressed); }
  static Button* buildBistate(const std::string& name, u16 x, u16 y, SpriteInfo normal) { return new Button(name, x, y, normal, normal.frame(1)); }
  static Button* buildBistate(const std::string& name, u16 x, u16 y, SpriteInfo normal, const std::string& label, const FontSpriteSheet* font) {
    auto* bt = new Button(name, x, y, normal, normal.frame(1));
    bt->setTextInfo(TextInfo(label, font));
    return bt;
  }

  
  static Button* buildTristate(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo pressed, SpriteInfo inactive) { return new Button(name, x, y, normal, pressed, inactive); }
  static Button* buildTristate(const std::string& name, u16 x, u16 y, SpriteInfo normal, SpriteInfo inactive) { return new Button(name, x, y, normal, normal.frame(1), inactive); }
  static Button* buildTristate(const std::string& name, u16 x, u16 y, SpriteInfo normal) { return new Button(name, x, y, normal, normal.frame(1), normal); }
  
};

class TwoPartButton : public Button
{
public:
  struct Clip { u16 start, length; };

private:
  ButtonGfx gfx2;
  Clip clip1, clip2;
  
public:
  TwoPartButton(const std::string& name, u16 x, u16 y, SpriteInfo normal1, SpriteInfo normal2, SpriteInfo pressed1, SpriteInfo pressed2, Clip clip1, Clip clip2) :
    Button(name, x, y, normal1, pressed1), gfx2(normal2, pressed2), clip1(clip1), clip2(clip2) { }
  
  void draw() override;
};

template<typename T>
class RadioButtonGroup
{
  using value_type = T*;
  
private:
  bool allowsNoSelection;
  std::vector<T*> buttons;
  T* current;
  std::function<void(T*)> lambda;
  std::function<bool(T*)> canSelectPredicate;
public:
  RadioButtonGroup(bool allowsNoSelection = false)
    : current(nullptr), allowsNoSelection(allowsNoSelection), canSelectPredicate([](T*){return true;}) { }
  
  T* getCurrent() const { return current; }
  void set(u16 index) { current = buttons[index]; }
  void click(T* button)
  {
    if (current != button)
    {
      if (button && !canSelectPredicate(button))
        return;
      
      current = button;
      if (lambda)
        lambda(current);
    }
    else if (allowsNoSelection)
    {
      current = nullptr;
    }
    
    for (const auto& button : buttons)
      button->setToggled(button == current);
  }
  
  void add(T* button)
  {
    buttons.push_back(button);
  }
  
  void setCanSelectPredicate(std::function<bool(T*)> predicate) { this->canSelectPredicate = predicate; }
  void setAction(std::function<void(T*)> lambda) { this->lambda = lambda; }
};

template<typename T>
class RadioButton : public Button
{
  using group_t = RadioButtonGroup<RadioButton<T>>;
private:
  group_t* group;
  
  ButtonGfx toggledGfx;
  bool toggled;
  
  T data;
  
  RadioButton(const std::string name, T data, RadioButtonGroup<RadioButton<T>>* group, u16 x, u16 y, SpriteInfo normal, SpriteInfo pressed, SpriteInfo normalToggled, SpriteInfo pressedToggled)
  : Button(name, x, y, normal, pressed), data(data), toggledGfx(normalToggled, pressedToggled), toggled(false), group(group) { }
  
public:
  void draw() override;
  
  const T getData() const { return data; }
  
  void click() override { group->click(this); }
  void setToggled(bool t) { this->toggled = t; }
  
  static RadioButton<T>* build(const std::string name, T data, RadioButtonGroup<RadioButton<T>>* group, u16 x, u16 y, SpriteInfo untoggled, SpriteInfo toggled)
  {
    return new RadioButton(name, data, group, x, y, untoggled, untoggled.frame(1), toggled, toggled.frame(1));
  }
};

template<typename T>
class RadioClickable : public Clickable
{
  using group_t = RadioButtonGroup<RadioClickable<T>>;

protected:
  group_t* group;
  bool toggled;
  T data;

public:
  RadioClickable(T data, group_t* group, u16 x, u16 y, u16 w, u16 h) : Clickable(x, y, w, h), group(group), toggled(false), data(data) {
    setAction([this](){ this->group->click(this); });
  }
  
  const group_t* getGroup() const { return group; }
  void setToggled(bool t) { this->toggled = t; }
};

#endif
