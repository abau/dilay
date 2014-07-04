#ifndef DILAY_TRANSLATABLE
#define DILAY_TRANSLATABLE

#include <glm/fwd.hpp>

class Translatable {
  public:
    virtual ~Translatable () {}

    virtual void      translate (const glm::vec3&) = 0;
    virtual void      position  (const glm::vec3&) = 0;
    virtual glm::vec3 position  () const           = 0;
};

#endif
