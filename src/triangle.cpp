#include "triangle.hpp"
#include "util.hpp"

glm::vec3 Triangle :: edge1 () const { return this->_vertex2 - this->_vertex1; }
glm::vec3 Triangle :: edge2 () const { return this->_vertex3 - this->_vertex1; }

glm::vec3 Triangle :: normal () const { 
  return glm::cross (this->edge1 (), this->edge2 ());
}

Maybe <glm::vec3> Triangle :: intersectRay (const Ray& ray) const {
  glm::vec3 e1 = this->edge1 ();
  glm::vec3 e2 = this->edge2 ();

  glm::vec3 s1  = glm::cross (ray.direction (), e2);
  float divisor = glm::dot (s1, e1);

  if (divisor < Util :: epsilon) 
    return Maybe <glm::vec3> :: nothing ();

  float     invDivisor = 1.0f / divisor;
  glm::vec3 d          = ray.origin () - this->vertex1 ();
  glm::vec3 s2         = glm::cross (d,e1);
  float     b1         = glm::dot (d,s1)                 * invDivisor;
  float     b2         = glm::dot (ray.direction (), s2) * invDivisor;
  float     t          = glm::dot (e2, s2)               * invDivisor;

  if (b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f || t < 0.0f) 
    return Maybe <glm::vec3> :: nothing ();

  return Maybe <glm::vec3> (ray.pointAt (t));
}
