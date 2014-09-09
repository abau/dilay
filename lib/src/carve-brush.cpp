#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "carve-brush.hpp"

struct CarveBrush :: Impl {

  static constexpr float falloff = 1.3f;
  static constexpr float domain  = 2.0f;

  float        width;
  float        height;
  float        detail;
  float        stepWidth;
  bool         hasPosition;
  glm::vec3    lastPosition;
  glm::vec3   _position;
  WingedMesh* _mesh;

  Impl (float w, float h, float d, float s) 
    : width       (w)
    , height      (h)
    , detail      (d)
    , stepWidth   (width * s)
    , hasPosition (false)
    {}

  float y (float x) const {
    if (x >= this->width)
      return 0.0f;
    else {
      float normX = (x * Impl::domain) / this->width;
      return glm::exp <float> (- Impl::falloff * normX * normX * normX * normX) * this->height;
    }
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
      if (glm::distance2 (this->_position, p) > this->stepWidth * this->stepWidth) {
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
GETTER_CONST    (float            , CarveBrush, width)
GETTER_CONST    (float            , CarveBrush, height)
GETTER_CONST    (float            , CarveBrush, detail)
GETTER_CONST    (float            , CarveBrush, stepWidth)
GETTER_CONST    (bool             , CarveBrush, hasPosition)
GETTER_CONST    (const glm::vec3& , CarveBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3& , CarveBrush, position)
DELEGATE_CONST  (WingedMesh&      , CarveBrush, mesh)
DELEGATE2       (bool             , CarveBrush, updatePosition, WingedMesh&, const glm::vec3&)
SETTER          (float            , CarveBrush, width)
SETTER          (float            , CarveBrush, height)
SETTER          (float            , CarveBrush, detail)
SETTER          (float            , CarveBrush, stepWidth)
