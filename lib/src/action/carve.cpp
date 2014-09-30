#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <unordered_set>
#include "action/carve.hpp"
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "carve-brush.hpp"
#include "id.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionCarve::Impl {
  ActionCarve*              self;
  ActionUnitOn <WingedMesh> actions;

  Impl (ActionCarve* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run (const CarveBrush& brush) { 
    std::vector <WingedFace*> domain;
    PrimSphere                sphere (brush.position (), brush.width ());
    WingedMesh&               mesh = brush.mesh ();

    mesh.intersects               (sphere, domain);
    ActionSubdivide::extendDomain (domain);
    //this->carveFaces              (brush, domain);
    this->subdivideEdges          (brush, sphere, domain);
    this->finalize                (mesh, domain);

    mesh.writeAllIndices ();

    this->self->bufferData        (mesh);
  }

  glm::vec3 carveVertex ( const CarveBrush& brush, const glm::vec3& normal
                        , const glm::vec3& pos) const 
  {
    const float delta = brush.y (glm::distance <float> (pos, brush.position ()));
    return pos + (normal * delta);
  }

  void carveFaces (CarveBrush brush, const std::vector <WingedFace*>& faces) {
    WingedMesh& mesh = brush.mesh ();

    // compute set of vertices
    std::unordered_set <WingedVertex*> vertices;
    for (const WingedFace* face : faces) {
      assert (face);
      vertices.insert (&face->firstVertex  ());
      vertices.insert (&face->secondVertex ());
      vertices.insert (&face->thirdVertex  ());
    }

    // get average normal & maximum width
    glm::vec3 avgNormal (0.0f);
    float     maxWidth = 0.0f;
    for (WingedVertex* v : vertices) {
      const glm::vec3 p         = v->vector             (mesh);
      const glm::vec3 n         = v->interpolatedNormal (mesh);
                      avgNormal = avgNormal + n;
                      maxWidth  = glm::max ( maxWidth
                                           , glm::distance2 (p, brush.position ()) );
    }
    avgNormal = avgNormal / float (vertices.size ());
    maxWidth  = glm::sqrt (maxWidth);

    brush.width (glm::min (maxWidth, brush.width ()));

    // write new positions
    for (WingedVertex* v : vertices) {
      const glm::vec3 newPos = this->carveVertex (brush, avgNormal, v->vector (mesh));

      this->actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }

  void subdivideEdges ( const CarveBrush& brush, const PrimSphere& sphere
                      , std::vector <WingedFace*>& domain ) 
  {
    typedef std::unordered_set <WingedEdge*> EdgeSet;
    typedef std::unordered_set <WingedFace*> FaceSet;

    WingedMesh& mesh = brush.mesh ();
    EdgeSet     nextIteration;
    FaceSet     affectedFaces;

    auto isSubdividable = [&] (const WingedEdge& edge) -> bool {
      return edge.lengthSqr (mesh) > brush.detail () * brush.detail ();
    };

    auto checkNextIteration = [&] (WingedEdge& edge) -> void {
      if ( nextIteration.count          (&edge) == 0 
        && isSubdividable               (edge) 
        && ( IntersectionUtil::intersects (sphere, mesh, edge.leftFaceRef  ())
          || IntersectionUtil::intersects (sphere, mesh, edge.rightFaceRef ()) ))
      {
        nextIteration.insert (&edge);
      }
    };
    // initialize first iteration
    affectedFaces.insert (domain.begin (), domain.end ());
    for (WingedFace* f : domain) {
      assert (f);
      assert (f->isTriangle ());
      for (WingedEdge& e : f->adjacentEdges ()) {
        checkNextIteration (e);
      }
    }
    // iterate
    while (nextIteration.size () > 0) {
      FaceSet tmpAffected;

      this->actions.add <ActionSubdivide> ().run (mesh, nextIteration, tmpAffected);

      nextIteration.clear ();

      for (WingedFace* face : tmpAffected) {
        assert (face);
        affectedFaces.insert (face);

        for (WingedEdge& e : face->adjacentEdges ()) {
          checkNextIteration (e);
        }
      }
    }
    // collect affected faces
    domain.clear  ();
    domain.insert (domain.end (), affectedFaces.begin (), affectedFaces.end ());
  }

  void finalize (WingedMesh& mesh, std::vector <WingedFace*>& domain) {
    // compute set of vertices & realign
    std::unordered_set <WingedVertex*> vertices;
    for (WingedFace* face : domain) {
      assert (face);
      vertices.insert (&face->firstVertex  ());
      vertices.insert (&face->secondVertex ());
      vertices.insert (&face->thirdVertex  ());

      this->self->realignFace (mesh, std::move (*face));
    }
    // write normals
    for (WingedVertex* v : vertices) {
      this->actions.add <PAModifyWVertex> ().writeInterpolatedNormal (mesh,*v);
    }
  }
};

DELEGATE_BIG3_SELF (ActionCarve)
DELEGATE1          (void, ActionCarve, run, const CarveBrush&)
DELEGATE1          (void, ActionCarve, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1          (void, ActionCarve, runRedoBeforePostProcessing, WingedMesh&)
