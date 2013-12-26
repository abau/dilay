#ifndef DILAY_TRIANGLE
#define DILAY_TRIANGLE

#include "macro.hpp"
#include "fwd-glm.hpp"

class Ray;
class WingedMesh;
class WingedVertex;

class Triangle {
  public:
    DECLARE_BIG5 (Triangle)
    Triangle     ( const glm::vec3&, const glm::vec3&, const glm::vec3&);
    Triangle     ( const WingedMesh&, const WingedVertex&
                 , const WingedVertex&, const WingedVertex&);

    const glm::vec3&  vertex1   () const;
    const glm::vec3&  vertex2   () const;
    const glm::vec3&  vertex3   () const;

    void              vertex1   (const glm::vec3&);
    void              vertex2   (const glm::vec3&);
    void              vertex3   (const glm::vec3&);

    glm::vec3         edge1     () const;
    glm::vec3         edge2     () const;
    glm::vec3         normal    () const;
    glm::vec3         center    () const;
    glm::vec3         minimum   () const;
    glm::vec3         maximum   () const;
    float             maxExtent () const;

    bool              intersectRay (const Ray&, glm::vec3&) const;

  private:
    class Impl;
    Impl* impl;
};

#endif
