#include <glm/glm.hpp>
#include "intersection.hpp"
#include "winged-face.hpp"
#include "macro.hpp"

struct FaceIntersectionImpl {
  glm::vec3  position;
  LinkedFace face;

  FaceIntersectionImpl () {}
  FaceIntersectionImpl (const glm::vec3& p, LinkedFace f)
    : position (p), face (f) {}
};

DELEGATE_BIG4         (FaceIntersection)
DELEGATE2_CONSTRUCTOR (FaceIntersection,const glm::vec3&, LinkedFace)

GETTER (const glm::vec3&, FaceIntersection, position)
GETTER (LinkedFace      , FaceIntersection, face)
SETTER (const glm::vec3&, FaceIntersection, position)
SETTER (LinkedFace      , FaceIntersection, face)
