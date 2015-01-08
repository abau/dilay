#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/carve.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "carve-brush.hpp"
#include "intersection.hpp"
#include "octree.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "partial-action/relax-edge.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "primitive/sphere.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionCarve::Impl {
  ActionCarve*              self;
  ActionUnitOn <WingedMesh> actions;

  Impl (ActionCarve* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) const { this->actions.redo (mesh); }

  /*
  void run (const CarveBrush& brush) { 
    AffectedFaces domain;
    PrimSphere    sphere (brush.position (), brush.radius ());
    WingedMesh&   mesh = brush.mesh ();

    mesh.intersects      (sphere, domain);

    for (WingedFace* f : domain.faces ()) {
      this->actions.add <PACollapseFace> ().run (mesh, *f, domain);
      break;
    }
    this->finalize       (mesh, domain);
  }
  */

  void run (const CarveBrush& brush) { 
    AffectedFaces domain;
    PrimSphere    sphere (brush.position (), brush.radius ());

    IntersectionUtil::extend (sphere, brush.mesh (), brush.face (), domain);

    this->carveFaces (brush, domain);

    if (brush.subdivide ()) {
      this->subdivideEdges (brush, domain);
    }
    this->self->finalize (brush.mesh (), domain, this->actions);
  }

  glm::vec3 carveVertex ( const CarveBrush& brush, const glm::vec3& normal
                        , const glm::vec3& pos ) const 
  {
    const float delta = brush.y (glm::distance <float> (pos, brush.position ()));
    return pos + (normal * delta);
  }

  void carveFaces (CarveBrush brush, AffectedFaces& domain) {
    WingedMesh&  mesh     = brush.mesh         ();
    VertexPtrSet vertices = domain.toVertexSet ();

    // get average normal
    glm::vec3 avgNormal (0.0f);
    for (WingedVertex* v : vertices) {
      avgNormal = avgNormal + v->savedNormal (mesh);
    }
    avgNormal = avgNormal / float (vertices.size ());

    // write new positions
    for (WingedVertex* v : vertices) {
      const glm::vec3 newPos = this->carveVertex (brush, avgNormal, v->position (mesh));

      this->actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }

  void subdivideEdges (const CarveBrush& brush, AffectedFaces& domain) {
    const float maxLength    ((4.0f/3.0f) * brush.subdivThreshold ());
    const float maxLengthSqr (maxLength * maxLength);
    WingedMesh& mesh         (brush.mesh ());

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
    subdivideEdges ();
    relaxEdges     ();
    smoothVertices ();
  }
};

DELEGATE_BIG3_SELF (ActionCarve)
DELEGATE1          (void, ActionCarve, run, const CarveBrush&)
DELEGATE1_CONST    (void, ActionCarve, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1_CONST    (void, ActionCarve, runRedoBeforePostProcessing, WingedMesh&)
