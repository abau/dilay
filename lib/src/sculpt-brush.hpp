#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/fwd.hpp>
#include "macro.hpp"

template <typename T> class ActionUnitOn;
class AffectedFaces;
class WingedFace;
class WingedMesh;

class SculptBrush {
  public:
    DECLARE_BIG6_VIRTUAL (SculptBrush)

    float            radius          () const;
    float            detailFactor    () const;
    float            stepWidthFactor () const;
    bool             subdivide       () const;
    WingedMesh*      mesh            () const;
    WingedFace*      face            () const;

    void             radius          (float);
    void             detailFactor    (float);
    void             stepWidthFactor (float);
    void             subdivide       (bool);
    void             mesh            (WingedMesh*);
    void             face            (WingedFace*);

    void             sculpt          (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
    float            subdivThreshold () const;
    const glm::vec3& position        () const;
    glm::vec3        delta           () const;
    bool             updatePosition  (const glm::vec3&);

    SAFE_REF_CONST (WingedMesh, mesh)
    SAFE_REF_CONST (WingedFace, face)
  private:
    IMPLEMENTATION

    virtual void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const = 0;
};

#endif
