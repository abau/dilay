#ifndef DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH
#define DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH

#include "action/on.hpp"
#include "macro.hpp"

class AffectedFaces;
class WingedMesh;
class WingedFace;
template <typename T> class ActionUnitOn;

class ActionOnPostProcessedWMesh : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3_VIRTUAL (ActionOnPostProcessedWMesh)

  protected:
    WingedFace& realignFace     (WingedMesh&, WingedFace&);
    void        realignAllFaces (WingedMesh&);
    void        writeAllNormals (WingedMesh&);
    void        writeAllIndices (WingedMesh&);
    void        bufferData      (WingedMesh&);

    /** `finalize (m,f,a)` finalizes an action on `m`:
     * 1. realign each face in `f`
     * 2. collapse each degenerated face in `m`
     * 3. reset free face indices in `m`
     * 4. write normal for each vertex that is either included in `f` 
     * or that has been modified by (2.) 
     * 5. buffer data of `m` */
    void        finalize        (WingedMesh&, AffectedFaces&, ActionUnitOn <WingedMesh>&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    virtual void runUndoBeforePostProcessing (WingedMesh&) const = 0;
    virtual void runRedoBeforePostProcessing (WingedMesh&) const = 0;

    IMPLEMENTATION
};

#endif
