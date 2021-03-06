#include "Platform.h"

#include <sys/stat.h>

#include "Path.h"

class PlatformUnix : public Platform
{
public:
  Path getResourcePath() const override
  {
    return "../..";
  }
  
  bool exists(const Path& path) const override
  {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
  }
  
  Path absolute(const Path& path) const override
  {
    // TODO
    return path;
  }
};

Platform* Platform::instance()
{
  static PlatformUnix platform;
  return &platform;
}

