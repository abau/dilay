#ifndef DILAY_WINGED_FACE_INTERSECTION
#define DILAY_WINGED_FACE_INTERSECTION

#include "macro.hpp"
#include "intersection.hpp"

class WingedFace;
class WingedMesh;

class WingedFaceIntersection : public Intersection {
  public:
    DECLARE_BIG6 (WingedFaceIntersection)

    WingedMesh& mesh   () const;
    WingedFace& face   () const;
    void        update ( float, const glm::vec3&, const glm::vec3&
                       , WingedMesh&, WingedFace& );

  private:
    class Impl;
    Impl* impl;
};

#endif
