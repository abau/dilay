#include <glm/glm.hpp>
#include "primitive/aabox.hpp"

struct PrimAABox::Impl {
  glm::vec3 maximum;
  glm::vec3 minimum;

  Impl () {}
  Impl (const glm::vec3& min, const glm::vec3& max) 
    : maximum  (min)
    , minimum  (max)
    {}
  Impl (const glm::vec3& pos, float xW, float yW, float zW)
    : Impl ( pos + glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f)
           , pos - glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f))
    {}
  Impl (const glm::vec3& pos, float w) : Impl (pos, w, w, w) {}

  float xWidth () const { return maximum.x - minimum.x; }
  float yWidth () const { return maximum.y - minimum.y; }
  float zWidth () const { return maximum.z - minimum.z; }

  void xWidth (float w) { 
    float p = this->position ().x;
    this->maximum.x = p + (w * 0.5f);
    this->minimum.x = p - (w * 0.5f);
  }

  void yWidth (float w) { 
    float p = this->position ().y;
    this->maximum.y = p + (w * 0.5f);
    this->minimum.y = p - (w * 0.5f);
  }

  void zWidth (float w) { 
    float p = this->position ().z;
    this->maximum.z = p + (w * 0.5f);
    this->minimum.z = p - (w * 0.5f);
  }

  void width (float w) { 
    this->xWidth (w);
    this->yWidth (w);
    this->zWidth (w);
  }

  glm::vec3 position () const {
    return (this->maximum + this->minimum) * 0.5f;
  }

  void position (const glm::vec3& p) {
    glm::vec3 oldPos = this->position ();

    this->maximum = p + (this->maximum - oldPos);
    this->minimum = p + (this->minimum - oldPos);
  }
};

DELEGATE_BIG6 (PrimAABox)
DELEGATE2_CONSTRUCTOR (PrimAABox, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR (PrimAABox, const glm::vec3&, float, float, float)
DELEGATE2_CONSTRUCTOR (PrimAABox, const glm::vec3&, float)
GETTER_CONST          (const glm::vec3&, PrimAABox, maximum)
GETTER_CONST          (const glm::vec3&, PrimAABox, minimum)
SETTER                (const glm::vec3&, PrimAABox, maximum)
SETTER                (const glm::vec3&, PrimAABox, minimum)
DELEGATE_CONST        (float           , PrimAABox, xWidth)
DELEGATE_CONST        (float           , PrimAABox, yWidth)
DELEGATE_CONST        (float           , PrimAABox, zWidth)
DELEGATE1             (void            , PrimAABox, xWidth, float)
DELEGATE1             (void            , PrimAABox, yWidth, float)
DELEGATE1             (void            , PrimAABox, zWidth, float)
DELEGATE1             (void            , PrimAABox, width , float)
DELEGATE_CONST        (glm::vec3       , PrimAABox, position)
DELEGATE1             (void            , PrimAABox, position, const glm::vec3&)
