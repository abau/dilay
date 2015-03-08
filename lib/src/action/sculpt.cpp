#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/sculpt.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "affected-faces.hpp"
#include "sculpt-brush.hpp"
#include "partial-action/relax-edge.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "winged/edge.hpp"

struct ActionSculpt::Impl {
  ActionSculpt*     self;
  ActionUnitOnWMesh actions;

  Impl (ActionSculpt* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) const { this->actions.redo (mesh); }

  void run (const SculptBrush& brush) { 
    AffectedFaces domain;

    brush.sculpt (domain, this->actions);

    this->postprocessEdges (brush, domain);
    this->self->finalize   (brush.meshRef (), domain, this->actions);
  }

  void postprocessEdges (const SculptBrush& brush, AffectedFaces& domain) {
    const float maxLength    ((4.0f/3.0f) * brush.subdivThreshold ());
    const float maxLengthSqr (maxLength * maxLength);
    WingedMesh& mesh         (brush.meshRef ());

    auto isSubdividable = [&] (WingedEdge& edge) -> bool {
      return edge.lengthSqr (mesh) > maxLengthSqr;
    };

    auto subdivideEdges = [&] () {
      for (WingedEdge* e : domain.toEdgeVec ()) {
        if (isSubdividable (*e)) {
          this->actions.add <PASubdivideEdge> ().run (mesh, *e, domain);
        }
      }
      domain.commit ();
    };
    auto relaxEdges = [&] () {
      for (WingedEdge* e : domain.toEdgeVec ()) {
        this->actions.add <PARelaxEdge> ().run (mesh, *e, domain);
      }
      domain.commit ();
    };
    auto smoothVertices = [&] () {
      this->actions.add <PASmooth> ().run (mesh, domain.toVertexSet (), 5, domain);
      domain.commit ();
    };

    PASubdivideEdge::extendDomain (domain);
    if (brush.subdivide ()) {
      subdivideEdges ();
    }
    relaxEdges     ();
    smoothVertices ();
  }
};

DELEGATE_BIG3_SELF (ActionSculpt)
DELEGATE1          (void, ActionSculpt, run, const SculptBrush&)
DELEGATE1_CONST    (void, ActionSculpt, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1_CONST    (void, ActionSculpt, runRedoBeforePostProcessing, WingedMesh&)
