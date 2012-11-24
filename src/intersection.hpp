#include "linked-list.hpp"

#ifndef INTERSECTION
#define INTERSECTION

class WingedFace;

class FaceIntersection {
  private:
    glm::vec3                   _position;
    LinkedElement <WingedFace>& _face;

  public:
    FaceIntersection (const glm::vec3& p, LinkedElement <WingedFace>& f)
      : _position (p), _face (f) {}

    const glm::vec3&            position () const { return this->_position; }
    LinkedElement <WingedFace>& face     () const { return this->_face; }
};

#endif
