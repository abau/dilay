#include <glm/glm.hpp>
#include "primitive/aabox.hpp"

struct PrimAABox::Impl {
  glm::vec3 position;
  glm::vec3 maximum;
  glm::vec3 minimum;

  Impl () {}
  Impl (const glm::vec3& pos, const glm::vec3& max, const glm::vec3& min) 
    : position (pos)
    , maximum  (max)
    , minimum  (min)
    {}
  Impl (const glm::vec3& pos, float xW, float yW, float zW)
    : Impl (pos, pos + glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f)
               , pos - glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f))
    {}
  Impl (const glm::vec3& pos, float w) : Impl (pos, w, w, w) {}

  float xWidth () const { return maximum.x - minimum.x; }
  float yWidth () const { return maximum.y - minimum.y; }
  float zWidth () const { return maximum.z - minimum.z; }

  void xWidth (float w) { 
    this->maximum.x = this->position.x + (w * 0.5f);
    this->minimum.x = this->position.x - (w * 0.5f);
  }

  void yWidth (float w) { 
    this->maximum.y = this->position.y + (w * 0.5f);
    this->minimum.y = this->position.y - (w * 0.5f);
  }

  void zWidth (float w) { 
    this->maximum.z = this->position.z + (w * 0.5f);
    this->minimum.z = this->position.z - (w * 0.5f);
  }

  void width (float w) { 
    this->xWidth (w);
    this->yWidth (w);
    this->zWidth (w);
  }
};

DELEGATE_BIG6 (PrimAABox)
DELEGATE3_CONSTRUCTOR (PrimAABox, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR (PrimAABox, const glm::vec3&, float, float, float)
DELEGATE2_CONSTRUCTOR (PrimAABox, const glm::vec3&, float)
GETTER_CONST          (const glm::vec3&, PrimAABox, position)
GETTER_CONST          (const glm::vec3&, PrimAABox, maximum)
GETTER_CONST          (const glm::vec3&, PrimAABox, minimum)
SETTER                (const glm::vec3&, PrimAABox, position)
SETTER                (const glm::vec3&, PrimAABox, maximum)
SETTER                (const glm::vec3&, PrimAABox, minimum)
DELEGATE_CONST        (float           , PrimAABox, xWidth)
DELEGATE_CONST        (float           , PrimAABox, yWidth)
DELEGATE_CONST        (float           , PrimAABox, zWidth)
DELEGATE1             (void            , PrimAABox, xWidth, float)
DELEGATE1             (void            , PrimAABox, yWidth, float)
DELEGATE1             (void            , PrimAABox, zWidth, float)
DELEGATE1             (void            , PrimAABox, width , float)
