#ifndef DILAY_INTERSECTION
#define DILAY_INTERSECTION

#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

class FaceIntersectionImpl;

class FaceIntersection {
  public:
     FaceIntersection            ();
     FaceIntersection            (const glm::vec3&, LinkedFace);
     FaceIntersection            (const FaceIntersection&);
     FaceIntersection& operator= (const FaceIntersection&);
    ~FaceIntersection            ();

    const glm::vec3& position () const;
    LinkedFace       face     () const;

    void             position (const glm::vec3&);
    void             face     (LinkedFace);

  private:
    FaceIntersectionImpl* impl;
};

#endif
