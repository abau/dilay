#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "sculpt-brush.hpp"

struct SculptBrush :: Impl {

  SculptBrush* self;
  float        radius;
  float        detailFactor;
  bool         subdivide;
  bool         wasUpdated;
  glm::vec3   _position;
  WingedMesh* _mesh;
  WingedFace* _face;

  Impl (SculptBrush* s) 
    : self       (s)
    , wasUpdated (false)
    {}

  void sculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>& actions) const {
    this->self->runSculpt (faces, actions);
  }

  float subdivThreshold () const {
    return (1.0f - this->detailFactor) * this->radius;
  }

  const glm::vec3& position () const {
    assert (this->wasUpdated);
    return this->_position;
  }

  WingedMesh& mesh () const {
    assert (this->wasUpdated);
    return *this->_mesh;
  }

  WingedFace& face () const {
    assert (this->wasUpdated);
    return *this->_face;
  }

  bool update (WingedMesh& m, WingedFace& f, const glm::vec3& p) {
    return this->self->runUpdate (m,f,p);
  }

  void updateBase (WingedMesh& m, WingedFace& f, const glm::vec3& p) {
    this->wasUpdated =  true;
    this->_position  =  p;
    this->_mesh      = &m;
    this->_face      = &f;
  }

  bool runUpdate (WingedMesh& m, WingedFace& f, const glm::vec3& p) {
    this->updateBase (m,f,p);
    return true;
  }
};

DELEGATE_BIG6_SELF (SculptBrush)
  
GETTER_CONST    (float            , SculptBrush, radius)
GETTER_CONST    (float            , SculptBrush, detailFactor)
GETTER_CONST    (bool             , SculptBrush, subdivide)
SETTER          (float            , SculptBrush, radius)
SETTER          (float            , SculptBrush, detailFactor)
SETTER          (bool             , SculptBrush, subdivide)
DELEGATE2_CONST (void             , SculptBrush, sculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
DELEGATE_CONST  (float            , SculptBrush, subdivThreshold)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, position)
DELEGATE_CONST  (WingedMesh&      , SculptBrush, mesh)
DELEGATE_CONST  (WingedFace&      , SculptBrush, face)
DELEGATE3       (bool             , SculptBrush, update, WingedMesh&, WingedFace&, const glm::vec3&)
GETTER_CONST    (bool             , SculptBrush, wasUpdated)
DELEGATE3       (void             , SculptBrush, updateBase, WingedMesh&, WingedFace&, const glm::vec3&)
DELEGATE3       (bool             , SculptBrush, runUpdate, WingedMesh&, WingedFace&, const glm::vec3&)
