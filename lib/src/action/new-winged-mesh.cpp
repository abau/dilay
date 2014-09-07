#include <glm/glm.hpp>
#include <limits>
#include <map>
#include "action/ids.hpp"
#include "action/new-winged-mesh.hpp"
#include "action/unit/on.hpp"
#include "id.hpp"
#include "mesh-definition.hpp"
#include "mesh-type.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/triangle.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"

struct ActionNewWingedMesh :: Impl {
  ActionUnitOn <WingedMesh> actions;
  ActionIds                 ids;
  MeshType                  meshType;

  typedef std::pair <unsigned int,unsigned int> UIPair;
  typedef std::map <UIPair, WingedEdge*>        EdgeMap;

  WingedMesh& run (MeshType t, const MeshDefinition& def) {
    assert (this->actions.isEmpty ());

    WingedMesh& mesh = State::scene ().newWingedMesh (t);
    this->meshType   = t;
    this->ids.setMesh (0, mesh);

    // octree
    glm::vec3 maxVertex (std::numeric_limits <float>::lowest ());
    glm::vec3 minVertex (std::numeric_limits <float>::max    ());

    for (unsigned int i = 0; i < def.numVertices (); i++) {
      maxVertex = glm::max (maxVertex, def.vertex (i));
      minVertex = glm::min (minVertex, def.vertex (i));
    }
    const glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
    const glm::vec3 delta  =  maxVertex - minVertex;
    const float     width  = glm::max (glm::max (delta.x, delta.y), delta.z);

    this->actions.add <PAModifyWMesh> ().setupOctreeRoot (mesh, center, width);

    // vertices
    for (unsigned int i = 0; i < def.numVertices (); i++) {
      this->actions.add<PAModifyWMesh> ().addVertex (mesh, def.vertex (i));
    }

    // faces & edges
    EdgeMap edgeMap;

    for (unsigned int i = 0; i < def.numFace3 (); i++) {
      unsigned int index1, index2, index3;
      def.face (i, index1, index2, index3);

      WingedFace& f = this->actions.add<PAModifyWMesh> ()
                           .addFace ( mesh
                                    , PrimTriangle ( mesh, *mesh.vertex (index1)
                                                         , *mesh.vertex (index2)
                                                         , *mesh.vertex (index3) ));
      WingedEdge& e1 = this->findOrAddEdge (mesh, edgeMap, index1, index2, f);
      WingedEdge& e2 = this->findOrAddEdge (mesh, edgeMap, index2, index3, f);
      WingedEdge& e3 = this->findOrAddEdge (mesh, edgeMap, index3, index1, f);

      this->actions.add <PAModifyWEdge> ().predecessor (e1, f, &e3);
      this->actions.add <PAModifyWEdge> ().successor   (e1, f, &e2);
      this->actions.add <PAModifyWEdge> ().predecessor (e2, f, &e1);
      this->actions.add <PAModifyWEdge> ().successor   (e2, f, &e3);
      this->actions.add <PAModifyWEdge> ().predecessor (e3, f, &e2);
      this->actions.add <PAModifyWEdge> ().successor   (e3, f, &e1);
    }

    // write & buffer
    mesh.writeAllIndices             ();
    mesh.writeAllInterpolatedNormals ();
    mesh.bufferData                  ();

    return mesh;
  }

  /** `findOrAddEdge (w,m,i1,i2,f)` searches an edge with indices `(i2,i1)` 
   * (in that order) in `m`.
   * If such an edge exists, `f` becomes its new right face.
   * Otherwise a new edge is added to `w` and `m` with `f` being its left face.
   * The found (resp. created) edge is returned
   */
  WingedEdge& findOrAddEdge ( WingedMesh& mesh, EdgeMap& map
                            , unsigned int index1, unsigned int index2
                            , WingedFace& face)
  {
    auto result = map.find (UIPair (index2, index1));
    if (result == map.end ()) {
      WingedVertex* v1    = mesh.vertex (index1);
      WingedVertex* v2    = mesh.vertex (index2);
      WingedEdge& newEdge = this->actions.add <PAModifyWMesh> ()
                                 .addEdge ( mesh
                                          , WingedEdge ( Id ()
                                                       , v1, v2
                                                       , &face  , nullptr
                                                       , nullptr, nullptr
                                                       , nullptr, nullptr ));
        
      map.insert (std::pair <UIPair,WingedEdge*> ( UIPair (index1,index2)
                                                 , &newEdge ));

      this->actions.add <PAModifyWVertex> ().edge (*v1 , &newEdge);
      this->actions.add <PAModifyWVertex> ().edge (*v2 , &newEdge);
      this->actions.add <PAModifyWFace  > ().edge (face, &newEdge);

      return newEdge;
    }
    else {
      WingedEdge* existingEdge = result->second;

      this->actions.add <PAModifyWEdge> ().rightFace (*existingEdge, &face);
      this->actions.add <PAModifyWFace> ().edge      (face, existingEdge);
      return *existingEdge;
    }
  };

  void runUndo () {
    this->actions.undo (this->ids.getWingedMesh (0));
    State::scene ().deleteMesh (this->meshType, this->ids.getIdRef (0));
  }

  void runRedo () {
    WingedMesh& mesh = State::scene ().newWingedMesh (this->meshType, this->ids.getIdRef (0));
    this->actions.redo (mesh);
  }
};

DELEGATE_BIG3 (ActionNewWingedMesh)
DELEGATE2 (WingedMesh&, ActionNewWingedMesh, run, MeshType, const MeshDefinition&)
DELEGATE  (void       , ActionNewWingedMesh, runUndo)
DELEGATE  (void       , ActionNewWingedMesh, runRedo)
