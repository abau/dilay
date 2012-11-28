#include <glm/glm.hpp>
#include "maybe.hpp"
#include "ray.hpp"

#ifndef TRIANGLE
#define TRIANGLE

class Triangle {
  public:
    Triangle (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) 
      : _vertex1 (v1), _vertex2 (v2), _vertex3 (v3) {}

    const glm::vec3&  vertex1 () const { return this->_vertex1; }
    const glm::vec3&  vertex2 () const { return this->_vertex2; }
    const glm::vec3&  vertex3 () const { return this->_vertex3; }

    glm::vec3         edge1   () const;
    glm::vec3         edge2   () const;
    glm::vec3         normal  () const;

    Maybe <glm::vec3> intersectRay (const Ray&) const;

  private:
    const glm::vec3 _vertex1;
    const glm::vec3 _vertex2;
    const glm::vec3 _vertex3;
};

#endif
