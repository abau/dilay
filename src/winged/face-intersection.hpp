#ifndef DILAY_WINGED_FACE_INTERSECTION
#define DILAY_WINGED_FACE_INTERSECTION

#include <glm/fwd.hpp>
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class WingedFaceIntersection {
  public:
    DECLARE_BIG6 (WingedFaceIntersection)

    bool             isIntersection () const;
    float            distance       () const;
    const glm::vec3& position       () const;
    WingedMesh&      mesh           () const;
    WingedFace&      face           () const;
    void             update         (float, const glm::vec3&, WingedMesh&, WingedFace&);
    void             reset          ();

  private:
    class Impl;
    Impl* impl;
};

#endif
