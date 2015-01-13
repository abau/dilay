#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/fwd.hpp>
#include "fwd-winged.hpp"
#include "macro.hpp"

template <typename T> class ActionUnitOn;
class WingedFace;
class WingedMesh;

class SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrush)

    float            radius          () const;
    float            detailFactor    () const;
    bool             subdivide       () const;

    void             radius          (float);
    void             detailFactor    (float);
    void             subdivide       (bool);

    void             sculpt          (const VertexPtrSet&, ActionUnitOn <WingedMesh>&) const;
    float            subdivThreshold () const;
    const glm::vec3& position        () const;
    WingedMesh&      mesh            () const;
    WingedFace&      face            () const;
    bool             update          (WingedMesh&, WingedFace&, const glm::vec3&);

  protected:
    bool             wasUpdated      () const;
    void             updateBase      (WingedMesh&, WingedFace&, const glm::vec3&);

  private:
    IMPLEMENTATION

    virtual void     runSculpt       (const VertexPtrSet&, ActionUnitOn <WingedMesh>&) const = 0;
    virtual bool     runUpdate       (WingedMesh&, WingedFace&, const glm::vec3&);
};

#endif
