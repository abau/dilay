#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_6_GON
#define DILAY_PARTIAL_ACTION_TRIANGULATE_6_GON

#include "action.hpp"

class WingedFace;
class WingedMesh;

class PATriangulate6Gon : public Action {
  public: PATriangulate6Gon            ();
          PATriangulate6Gon            (const PATriangulate6Gon&) = delete;
    const PATriangulate6Gon& operator= (const PATriangulate6Gon&) = delete;
         ~PATriangulate6Gon            ();

  /** `r' = run (m,f)` triangulates the 6-gon `f`.
   * Note thate `f.edge ()->firstVertex (f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   * The new faces are adjacent to `f`.
   * `f` becomes invalid: use `r` instead.
   */
    WingedFace& run (WingedMesh&, WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif
