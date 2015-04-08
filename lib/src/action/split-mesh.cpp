#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "action/finalize.hpp"
#include "action/split-mesh.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/insert-edge-face.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void Action :: splitMesh (WingedMesh& mesh, const PrimPlane& plane) {

  auto correctVertices = [&mesh, &plane] (const AffectedFaces& affectedFaces) {
    for (WingedVertex* v : affectedFaces.toVertexSet ()) {
      const glm::vec3 pos = v->position (mesh);

      if (glm::epsilonEqual (plane.distance (pos), 0.0f, Util::epsilon ())) {
        v->writePosition (mesh, plane.project (pos));
      }
    }
  };

  auto discardFacesInPlane = [&mesh, &plane] (AffectedFaces& affectedFaces) {
    for (auto it = affectedFaces.faces ().begin (); it != affectedFaces.faces ().end (); ) {
      WingedFace& face = **it;

      const float d1 = plane.distance (face.vertexRef (0).position (mesh));
      const float d2 = plane.distance (face.vertexRef (1).position (mesh));
      const float d3 = plane.distance (face.vertexRef (2).position (mesh));

      const bool  e1 = glm::epsilonEqual (d1, 0.0f, Util::epsilon ());
      const bool  e2 = glm::epsilonEqual (d2, 0.0f, Util::epsilon ());
      const bool  e3 = glm::epsilonEqual (d3, 0.0f, Util::epsilon ());

      const bool  g1 = d1 > Util::epsilon ();
      const bool  g2 = d2 > Util::epsilon ();
      const bool  g3 = d3 > Util::epsilon ();

      const bool  noneOnePlane = !(e1 || e2 || e3);

      const bool  exactlyOneOnPlane = (e1 && !e2 && !e3 && (g2 != g3))
                                   || (e2 && !e1 && !e3 && (g1 != g3))
                                   || (e3 && !e1 && !e2 && (g1 != g2));

      if (noneOnePlane || exactlyOneOnPlane) {
        ++it;
      }
      else {
        it = affectedFaces.removeCommited (it);
      }
    }
  };

  auto splitEdges = [&mesh, &plane] ( const AffectedFaces& affectedFaces
                                    , VertexPtrSet& newVertices )
  {
    EdgePtrVec edges = affectedFaces.toEdgeVec ();
    float      t;

    for (WingedEdge* e : edges) {
      const glm::vec3 v1  = e->vertex1Ref ().position (mesh);
      const glm::vec3 v2  = e->vertex2Ref ().position (mesh);
      const PrimRay   ray = PrimRay (v1, v2 - v1);

      if (IntersectionUtil::intersects (ray, plane, &t)) {
        if (t > Util::epsilon () && t < (e->length (mesh) - Util::epsilon ())) {
          WingedEdge& newEdge = PartialAction::insertEdgeVertex (mesh, *e, ray.pointAt (t));

          newVertices.insert (newEdge.vertex2 ());
        }
      }
    }
  };

  auto connectNewVertices = [&mesh] ( AffectedFaces& affectedFaces
                                    , const VertexPtrSet& newVertices )
  {
    for (WingedFace* f : affectedFaces.faces ()) {
      const unsigned int numEdges = f->numEdges ();

      if (numEdges == 4) {
        WingedEdge& newEdge = PartialAction::insertEdgeFace (mesh, *f);
        affectedFaces.insert (newEdge.otherFaceRef (*f));

        assert (f->numEdges () == 3);
        assert (newEdge.otherFaceRef (*f).numEdges () == 3);
      }
      else if (numEdges == 5) {
        WingedEdge&   edge   = f->edgeRef ();
        WingedEdge&   prec   = edge.predecessorRef (*f);
        WingedEdge&   sucSuc = edge.successorRef (*f).successorRef (*f);

        WingedVertex& newV      = edge.secondVertexRef (*f);
        WingedVertex& maybeNew1 = prec.firstVertexRef (*f);
        WingedVertex& maybeNew2 = sucSuc.secondVertexRef (*f);

        assert (newVertices.count (&newV) > 0);
        
        if (newVertices.count (&maybeNew1) > 0) {
          WingedEdge& newEdge = PartialAction::insertEdgeFace (mesh, *f, edge);
          affectedFaces.insert (newEdge.otherFaceRef (*f));

          assert (f->numEdges () == 4);
          assert (newEdge.otherFaceRef (*f).numEdges () == 3);
        }
        else if (newVertices.count (&maybeNew2) > 0) {
          WingedEdge& newEdge = PartialAction::insertEdgeFace (mesh, *f, sucSuc);
          affectedFaces.insert (newEdge.otherFaceRef (*f));

          assert (f->numEdges () == 4);
          assert (newEdge.otherFaceRef (*f).numEdges () == 3);
        }
        else {
          std::abort ();
        }
      }
      else {
        std::abort ();
      }
    }
  };

  auto triangulateFaces = [&mesh] (const AffectedFaces& affectedFaces) {
    for (WingedFace* f : affectedFaces.faces ()) {
      const unsigned int numEdges = f->numEdges ();

      if (numEdges == 3) {
        f->writeIndices (mesh);
      }
      else if (numEdges == 4) {
        PartialAction::triangulateQuad (mesh, *f, nullptr);
      }
      else {
        std::abort ();
      }
    }
  };

  AffectedFaces affectedFaces;
  if (mesh.intersects (plane, affectedFaces)) {
    VertexPtrSet newVertices;

    correctVertices     (affectedFaces);
    discardFacesInPlane (affectedFaces);
    splitEdges          (affectedFaces, newVertices);
    connectNewVertices  (affectedFaces, newVertices);

    affectedFaces.commit ();

    triangulateFaces    (affectedFaces);

    Action::finalize (mesh, affectedFaces);
  }
  else {
    std::abort ();
  }
}
