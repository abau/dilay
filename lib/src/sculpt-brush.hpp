#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/fwd.hpp>
#include "macro.hpp"

class ActionUnitOnWMesh;
class AffectedFaces;
class WingedFace;
class WingedMesh;

class SculptBrush {
  public:
    DECLARE_BIG6_VIRTUAL (SculptBrush)

    float            radius              () const;
    float            detailFactor        () const;
    float            stepWidthFactor     () const;
    bool             subdivide           () const;
    WingedMesh*      mesh                () const;
    WingedFace*      face                () const;

    void             radius              (float);
    void             detailFactor        (float);
    void             stepWidthFactor     (float);
    void             subdivide           (bool);
    void             mesh                (WingedMesh*);
    void             face                (WingedFace*);

    float            subdivThreshold     () const;
    bool             hasPosition         () const;
    const glm::vec3& lastPosition        () const;
    const glm::vec3& position            () const;
    glm::vec3        delta               () const;
    void             setPosition         (const glm::vec3&);
    bool             updateDelta         (const glm::vec3&);
    bool             updatePosition      (const glm::vec3&);
    void             resetPosition       ();

    float            intensityFactor     () const;
    float            innerRadiusFactor   () const;
    bool             invert              () const;

    void             intensityFactor     (float);
    void             innerRadiusFactor   (float);
    void             invert              (bool);
    void             toggleInvert        ();
    void             useLastPosition     (bool);
    void             useIntersection     (bool);
    void             direction           (const glm::vec3&);
    void             useAverageDirection ();
    void             linearStep          (bool);

    void             sculpt              (AffectedFaces&, ActionUnitOnWMesh&) const;

    SAFE_REF_CONST (WingedMesh, mesh)
    SAFE_REF_CONST (WingedFace, face)
  private:
    IMPLEMENTATION
};

#endif
