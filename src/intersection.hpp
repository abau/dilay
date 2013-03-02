#ifndef INTERSECTION
#define INTERSECTION

#include "util.hpp"

class FaceIntersection {
  public:
    FaceIntersection (const glm::vec3& p, LinkedFace f)
      : _position (p), _face (f) {}

    const glm::vec3& position () const { return this->_position; }
    LinkedFace       face     () const { return this->_face; }

  private:
    glm::vec3        _position;
    LinkedFace       _face;
};

#endif
