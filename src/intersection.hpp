#include "linked-list.hpp"

#ifndef INTERSECTION
#define INTERSECTION

class WingedFace;

class FaceIntersection {
  public:
    FaceIntersection (const glm::vec3& p, LinkedElement <WingedFace>& f)
      : _position (p), _face (f) {}

    const glm::vec3&            position () const { return this->_position; }
    LinkedElement <WingedFace>& face     () const { return this->_face; }

  private:
    glm::vec3                   _position;
    LinkedElement <WingedFace>& _face;
};

#endif
