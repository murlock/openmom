#pragma once

#include "common/Common.h"

#include <vector>

template<typename T, typename S> class DrawElement;

template<typename T, typename S = std::less<std::unique_ptr<T>>>
class DrawQueue
{
private:
  bool _dirty;
  std::vector<std::unique_ptr<T>> _elements;

public:
  DrawQueue()
  {
    static_assert(std::is_base_of<DrawElement<T,S>, T>::value, "Template type must be subclass of DrawElement");
  }
  
  void sort()
  {
    std::sort(_elements.begin(), _elements.end(), S());
    _dirty = false;
  }
  
  void sortIfNeeded()
  {
    if (_dirty)
      sort();
  }
  
  void clear()
  {
    _elements.clear();
    _dirty = false;
  }
  
  void add(T* element)
  {
    _elements.push_back(std::unique_ptr<T>(element));
    _dirty = true;
  }
  
  void draw()
  {
    if (_dirty)
      sort();
    
    for (const auto& element : _elements)
      if (element->isVisible())
        element->draw();
  }
  
  void setDirty() { _dirty = true; }

  using type = T;
};

template<typename T, typename S = std::less<std::unique_ptr<T>>>
class DrawElement
{
private:
  bool visible;
  DrawQueue<T,S>* queue;
  
protected:
  DrawElement() : visible(true) { }
  
public:
  void setDirty() { queue->setDirty(); }
  
  bool isVisible() const { return visible; }
  void setVisible(bool visible) { this->visible = visible; }
};
