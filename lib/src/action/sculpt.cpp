#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/finalize.hpp"
#include "action/sculpt.hpp"
#include "affected-faces.hpp"
#include "sculpt-brush.hpp"
#include "partial-action/relax-edge.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "winged/edge.hpp"

namespace {
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
          PartialAction::subdivideEdge (mesh, *e, domain);
        }
      }
      domain.commit ();
    };
    auto relaxEdges = [&] () {
      for (WingedEdge* e : domain.toEdgeVec ()) {
        PartialAction::relaxEdge (mesh, *e, domain);
      }
      domain.commit ();
    };
    auto smoothVertices = [&] () {
      PartialAction::smooth (mesh, domain.toVertexSet (), 1, domain);
      domain.commit ();
    };

    PartialAction::extendDomain (domain);
    if (brush.subdivide ()) {
      subdivideEdges ();
    }
    relaxEdges     ();
    smoothVertices ();
  }
}

void Action :: sculpt (const SculptBrush& brush) { 
  AffectedFaces domain;

  brush.sculpt (domain);

  postprocessEdges (brush, domain);
  Action::finalize (brush.meshRef (), domain);
}
