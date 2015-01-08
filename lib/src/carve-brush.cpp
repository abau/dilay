#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "carve-brush.hpp"

struct CarveBrush :: Impl {

  float        radius;
  float        detailFactor;
  float        intensityFactor;
  float        stepWidthFactor;
  bool         subdivide;
  bool         hasPosition;
  glm::vec3    lastPosition;
  glm::vec3   _position;
  WingedMesh* _mesh;
  WingedFace* _face;

  Impl (float r, float d, float i, float s, bool sd) 
    : radius          (r)
    , detailFactor    (d)
    , intensityFactor (i)
    , stepWidthFactor (s)
    , subdivide       (sd)
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
    return (1.0f - this->detailFactor) * this->radius;
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

  WingedFace& face () const {
    assert (this->hasPosition);
    return *this->_face;
  }

  bool updatePosition (WingedMesh& m, WingedFace& f, const glm::vec3& p) {
    if (this->hasPosition) {
      if (glm::distance2 (this->_position, p) > this->stepWidth () * this->stepWidth ()) {
        this->lastPosition = this->_position;
        this->_position    =  p;
        this->_mesh        = &m;
        this->_face        = &f;
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
      this->_face       = &f;
      return true;
    }
  }
};

DELEGATE5_BIG6 (CarveBrush, float, float, float, float, bool)
  
GETTER_CONST    (float            , CarveBrush, radius)
GETTER_CONST    (float            , CarveBrush, detailFactor)
GETTER_CONST    (float            , CarveBrush, intensityFactor)
GETTER_CONST    (float            , CarveBrush, stepWidthFactor)
GETTER_CONST    (bool             , CarveBrush, subdivide)
SETTER          (float            , CarveBrush, radius)
SETTER          (float            , CarveBrush, detailFactor)
SETTER          (float            , CarveBrush, intensityFactor)
SETTER          (float            , CarveBrush, stepWidthFactor)
SETTER          (bool             , CarveBrush, subdivide)
DELEGATE1_CONST (float            , CarveBrush, y, float)
DELEGATE_CONST  (float            , CarveBrush, subdivThreshold)
DELEGATE_CONST  (float            , CarveBrush, intensity)
DELEGATE_CONST  (float            , CarveBrush, stepWidth)
GETTER_CONST    (bool             , CarveBrush, hasPosition)
GETTER_CONST    (const glm::vec3& , CarveBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3& , CarveBrush, position)
DELEGATE_CONST  (WingedMesh&      , CarveBrush, mesh)
DELEGATE_CONST  (WingedFace&      , CarveBrush, face)
DELEGATE3       (bool             , CarveBrush, updatePosition, WingedMesh&, WingedFace&, const glm::vec3&)
