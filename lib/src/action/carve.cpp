#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <unordered_set>
#include "action/carve.hpp"
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "carve-brush.hpp"
#include "id.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
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
    std::vector <WingedFace*> faces;
    PrimSphere                sphere (brush.position (), brush.width ());
    WingedMesh&               mesh = brush.mesh ();

    mesh.intersects        (sphere, faces);
    //this->carveFaces       (brush, faces);
    this->subdivideFaces   (brush, sphere, faces);
    this->finalize         (mesh, faces);
    this->self->bufferData (mesh);
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

  bool isSubdividable (const CarveBrush& brush, const WingedFace& face) const {
    return face.incircleRadiusSqr (brush.mesh ()) > brush.detail () * brush.detail ();
  }

  void subdivideFaces ( const CarveBrush& brush, const PrimSphere& sphere
                      , std::vector <WingedFace*>& faces ) 
  {
    WingedMesh&             mesh = brush.mesh ();
    std::unordered_set <Id> thisIteration;
    std::unordered_set <Id> nextIteration;
    std::unordered_set <Id> affectedFaces;

    auto checkNextIteration = [&] (const WingedFace& face) -> void {
      if ( nextIteration.count (face.id ()) == 0 
        && this->isSubdividable (brush, face)
        && IntersectionUtil::intersects (sphere,mesh,face)) 
      {
        nextIteration.insert (face.id ());
      }
    };

    // initialize first iteration
    for (WingedFace* face : faces) {
      assert (face);
      affectedFaces.insert (face->id ());
      checkNextIteration   (*face);
    }
    thisIteration = nextIteration;
    nextIteration.clear ();

    // iterate
    while (thisIteration.size () > 0) {
      std::vector <Id> tmpAffected;
      this->actions.add <ActionSubdivide> ().run (mesh, thisIteration, &tmpAffected);

      for (const Id& id : tmpAffected) {
        checkNextIteration   (mesh.faceRef (id));
        affectedFaces.insert (id);
      }
      thisIteration = nextIteration;
      nextIteration.clear ();
    }

    // collect affected faces
    faces.clear ();
    for (const Id& id : affectedFaces) {
      faces.push_back (&mesh.faceRef (id));
    }
  }

  void finalize (WingedMesh& mesh, std::vector <WingedFace*>& faces) {
    // compute set of vertices
    std::unordered_set <WingedVertex*> vertices;
    for (const WingedFace* face : faces) {
      assert (face);
      vertices.insert (&face->firstVertex  ());
      vertices.insert (&face->secondVertex ());
      vertices.insert (&face->thirdVertex  ());
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
