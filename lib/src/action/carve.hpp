#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class Id;
class WingedMesh;
class PrimRay;
class Intersection;
class CarveBrush;

class CarveCache {
  public: 
    DECLARE_BIG3 (CarveCache)

    bool        intersects (const PrimRay&, Intersection&);
    void        reset      ();

  private:
    friend class ActionCarve;

    class Impl;
    Impl* impl;
};

class ActionCarve : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionCarve)

    void run (const CarveBrush&, CarveCache&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif
