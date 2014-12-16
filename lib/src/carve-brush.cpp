#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "carve-brush.hpp"

struct CarveBrush :: Impl {

  float        radius;
  float        detail;
  float        intensityFactor;
  float        stepWidthFactor;
  bool         hasPosition;
  glm::vec3    lastPosition;
  glm::vec3   _position;
  WingedMesh* _mesh;

  Impl (float r, float d, float i, float s) 
    : radius          (r)
    , detail          (d)
    , intensityFactor (i)
    , stepWidthFactor (s)
    , hasPosition     (false)
    {}

  float y (float x) const {
    if (x >= this->radius)
      return 0.0f;
    else {
      const float n     = 4.0f;
      const float normX = x / this->radius;
      return this->intensity ()
           * (((n-1.0f) * glm::pow (normX, n)) - (n * glm::pow (normX, n-1.0f)) + 1.0f);
    }
  }

  float subdivThreshold () const {
    return (1.0f - this->detail) * this->radius;
  }

  float intensity () const {
    return this->intensityFactor * this->radius;
  }

  float stepWidth () const {
    return this->stepWidthFactor * this->radius;
  }

  const glm::vec3& position () const {
    assert (this->hasPosition);
    return this->_position;
  }

  WingedMesh& mesh () const {
    assert (this->hasPosition);
    return *this->_mesh;
  }

  bool updatePosition (WingedMesh& m, const glm::vec3& p) {
    if (this->hasPosition) {
      if (glm::distance2 (this->_position, p) > this->stepWidth () * this->stepWidth ()) {
        this->lastPosition = this->_position;
        this->_position    =  p;
        this->_mesh        = &m;
        return true;
      }
      else {
        return false;
      }
    }
    else {
      this->hasPosition =  true;
      this->_position   =  p;
      this->_mesh       = &m;
      return true;
    }
  }
};

DELEGATE4_BIG6 (CarveBrush, float, float, float, float)
  
DELEGATE1_CONST (float            , CarveBrush, y, float)
GETTER_CONST    (float            , CarveBrush, radius)
GETTER_CONST    (float            , CarveBrush, detail)
DELEGATE_CONST  (float            , CarveBrush, subdivThreshold)
GETTER_CONST    (float            , CarveBrush, intensityFactor)
DELEGATE_CONST  (float            , CarveBrush, intensity)
GETTER_CONST    (float            , CarveBrush, stepWidthFactor)
DELEGATE_CONST  (float            , CarveBrush, stepWidth)
GETTER_CONST    (bool             , CarveBrush, hasPosition)
GETTER_CONST    (const glm::vec3& , CarveBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3& , CarveBrush, position)
DELEGATE_CONST  (WingedMesh&      , CarveBrush, mesh)
DELEGATE2       (bool             , CarveBrush, updatePosition, WingedMesh&, const glm::vec3&)
SETTER          (float            , CarveBrush, radius)
SETTER          (float            , CarveBrush, detail)
SETTER          (float            , CarveBrush, intensityFactor)
SETTER          (float            , CarveBrush, stepWidthFactor)
