#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include <glm/fwd.hpp>
#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class Id;
class WingedMesh;
class PrimRay;
class Intersection;

class CarveCache {
  public: 
    DECLARE_BIG3 (CarveCache)

    bool        intersects (const PrimRay&, Intersection&);
    WingedMesh* meshCache  ();
    void        meshCache  (WingedMesh*);
    void        reset      ();

  private:
    friend class ActionCarve;

    class Impl;
    Impl* impl;
};

class ActionCarve : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionCarve)

    void run (WingedMesh&, const glm::vec3&, float, CarveCache&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif
