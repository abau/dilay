#include <glm/glm.hpp>
#include "winged/face-intersection.hpp"

struct WingedFaceIntersection::Impl {
  bool        isIntersection;
  float       distance;
  glm::vec3   position;
  WingedMesh* _mesh;
  WingedFace* _face;

  Impl () : isIntersection (false) {}

  void update (float d, const glm::vec3& p, WingedMesh& m, WingedFace& f) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      this->_mesh          = &m;
      this->_face          = &f;
    }
  }

  void reset () {
    this->isIntersection = false;
  }

  WingedMesh& mesh () const {
    assert (this->isIntersection);
    return *this->_mesh;
  }

  WingedFace& face () const {
    assert (this->isIntersection);
    return *this->_face;
  }
};

DELEGATE_BIG6  (WingedFaceIntersection)
DELEGATE4      (void            , WingedFaceIntersection, update, float, const glm::vec3&, WingedMesh&, WingedFace&)
DELEGATE       (void            , WingedFaceIntersection, reset)
DELEGATE_CONST (WingedMesh&     , WingedFaceIntersection, mesh)
DELEGATE_CONST (WingedFace&     , WingedFaceIntersection, face)
GETTER_CONST   (bool            , WingedFaceIntersection, isIntersection)
GETTER_CONST   (float           , WingedFaceIntersection, distance)
GETTER_CONST   (const glm::vec3&, WingedFaceIntersection, position)
