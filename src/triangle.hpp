#ifndef DILAY_TRIANGLE
#define DILAY_TRIANGLE

#include "fwd-glm.hpp"

class TriangleImpl;
class Ray;
template <class T> class Maybe;

class Triangle {
  public:
     Triangle            (const glm::vec3&, const glm::vec3&, const glm::vec3&);
     Triangle            ();
     Triangle            (const Triangle&);
     Triangle& operator= (const Triangle&);
    ~Triangle            ();

    const glm::vec3&  vertex1 () const;
    const glm::vec3&  vertex2 () const;
    const glm::vec3&  vertex3 () const;

    void              vertex1 (const glm::vec3&);
    void              vertex2 (const glm::vec3&);
    void              vertex3 (const glm::vec3&);

    glm::vec3         edge1   () const;
    glm::vec3         edge2   () const;
    glm::vec3         normal  () const;

    bool              intersectRay (const Ray&, glm::vec3&) const;

  private:
    TriangleImpl* impl;
};

#endif
