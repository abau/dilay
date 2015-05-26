#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/finalize.hpp"
#include "action/sculpt.hpp"
#include "affected-faces.hpp"
#include "sculpt-brush.hpp"
#include "partial-action/collapse-edge.hpp"
#include "partial-action/relax-edge.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"

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

    auto isCollapsable = [&] (WingedEdge& edge) -> bool {
      const auto& params = brush.constParameters <SBReduceParameters> ();
      const float r2     = brush.radius () * brush.radius ();
      const float i2     = params.intensity () * params.intensity ();
      return edge.lengthSqr (mesh) < r2 * i2;
    };

    auto collapseEdges = [&] () {
      EdgePtrVec                 edges = domain.toEdgeVec ();
      std::vector <unsigned int> indices;

      for (WingedEdge* e : domain.toEdgeVec ()) {
        indices.push_back (e->index ());
      }
      domain.reset ();

      for (unsigned int i : indices) {
        WingedEdge* e = mesh.edge (i);

        if (e && isCollapsable (*e)) {
          PartialAction::collapseEdge (mesh, *e, domain);
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

    if (brush.reduce ()) {
      collapseEdges ();
    }
    else {
      PartialAction::extendDomain (domain);
      if (brush.subdivide ()) {
        subdivideEdges ();
      }
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
