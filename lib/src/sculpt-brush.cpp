#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "sculpt-brush.hpp"

struct SculptBrush :: Impl {

  SculptBrush* self;
  float        radius;
  float        detailFactor;
  float        stepWidthFactor;
  bool         subdivide;
  WingedMesh*  mesh;
  WingedFace*  face;
  bool         hasPosition;
  glm::vec3    lastPosition;
  glm::vec3   _position;

  Impl (SculptBrush* s) 
    : self        (s)
    , hasPosition (false)
    {}

  void sculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>& actions) const {
    this->self->runSculpt (faces, actions);
  }

  float subdivThreshold () const {
    return (1.0f - this->detailFactor) * this->radius;
  }

  float stepWidth () const {
    return this->stepWidthFactor * this->self->radius ();
  }

  const glm::vec3& position () const {
    assert (this->hasPosition);
    return this->_position;
  }

  glm::vec3 delta () const {
    assert (this->hasPosition);
    return this->_position - this->lastPosition;
  }

  bool updatePosition (const glm::vec3& p) {
    if (this->hasPosition) {
      if (glm::distance2 (this->_position, p) > this->stepWidth () * this->stepWidth ()) {
        this->lastPosition = this->_position;
        this->_position    =  p;
        return true;
      }
      else {
        return false;
      }
    }
    else {
      this->hasPosition  = true;
      this->lastPosition = p;
      this->_position    = p;
      return true;
    }
  }
};

DELEGATE_BIG6_SELF (SculptBrush)
  
GETTER_CONST    (float            , SculptBrush, radius)
GETTER_CONST    (float            , SculptBrush, detailFactor)
GETTER_CONST    (float            , SculptBrush, stepWidthFactor)
GETTER_CONST    (bool             , SculptBrush, subdivide)
GETTER_CONST    (WingedMesh*      , SculptBrush, mesh)
GETTER_CONST    (WingedFace*      , SculptBrush, face)
SETTER          (float            , SculptBrush, radius)
SETTER          (float            , SculptBrush, detailFactor)
SETTER          (float            , SculptBrush, stepWidthFactor)
SETTER          (bool             , SculptBrush, subdivide)
SETTER          (WingedMesh*      , SculptBrush, mesh)
SETTER          (WingedFace*      , SculptBrush, face)
DELEGATE2_CONST (void             , SculptBrush, sculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
DELEGATE_CONST  (float            , SculptBrush, subdivThreshold)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, position)
DELEGATE_CONST  (glm::vec3        , SculptBrush, delta)
DELEGATE1       (bool             , SculptBrush, updatePosition, const glm::vec3&)
