#include <glm/glm.hpp>
#include "action/data.hpp"
#include "action/identifier.hpp"
#include "octree.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  enum class Operation { 
      DeleteEdge, DeleteFace, DeleteVertex
    , AddEdge, AddFace, AddVertex
    , InitOctreeRoot, SetIndex
  };

  struct OctreeRootData {
    glm::vec3 position;
    float     width;
  };

  typedef ActionData <PrimTriangle, glm::vec3, unsigned int, OctreeRootData> Data;
};

struct PAModifyWMesh :: Impl {
  Operation operation;
  Data      data;

  void deleteEdge (WingedMesh& mesh, WingedEdge& edge) {
    assert (edge.vertex1          () == nullptr);
    assert (edge.vertex2          () == nullptr);
    assert (edge.leftFace         () == nullptr);
    assert (edge.rightFace        () == nullptr);
    assert (edge.leftPredecessor  () == nullptr);
    assert (edge.leftSuccessor    () == nullptr);
    assert (edge.rightPredecessor () == nullptr);
    assert (edge.rightSuccessor   () == nullptr);

    this->operation = Operation::DeleteEdge;

    this->data.identifier (edge);
    mesh.deleteEdge       (edge);
  }

  void deleteFace (WingedMesh& mesh, WingedFace& face) {
    assert (face.edge () == nullptr);

    this->operation = Operation::DeleteFace;

    this->data.identifier (face);
    this->data.value      (face.triangle (mesh));
    mesh.deleteFace       (face);
  }

  void deleteVertex (WingedMesh& mesh, WingedVertex& vertex) {
    assert (vertex.edge () == nullptr);

    this->operation = Operation::DeleteVertex;

    this->data.identifier (vertex);
    this->data.value      (vertex.position (mesh));
    mesh.deleteVertex     (vertex);
  }

  WingedEdge& addEdge (WingedMesh& mesh) {
    WingedEdge& edge = mesh.addEdge (Id ());
    this->operation  = Operation::AddEdge;

    this->data.identifier (edge);
    return edge;
  }

  WingedFace& addFace (WingedMesh& mesh, const PrimTriangle& triangle) {
    WingedFace& face = mesh.addFace (triangle);
    this->operation  = Operation::AddFace;

    this->data.identifier (face);
    this->data.value      (triangle);
    return face;
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& position) {
    WingedVertex& vertex = mesh.addVertex (position);
    this->operation      = Operation::AddVertex;

    this->data.identifier (vertex);
    this->data.value      (position);
    return vertex;
  }

  void setIndex (WingedMesh& mesh, unsigned int index, unsigned int vertexIndex) {
    this->operation = Operation::SetIndex;

    this->data.identifier (index);
    this->data.values     (mesh.index (index), vertexIndex);
    mesh.setIndex         (index, vertexIndex);
  }

  void setupOctreeRoot (WingedMesh& mesh, const glm::vec3& pos, float width) {
    this->operation = Operation::InitOctreeRoot;

    this->data.value     (OctreeRootData {pos, width});
    mesh.setupOctreeRoot (pos, width);
  }

  void runUndo (WingedMesh& mesh) const { 

    switch (this->operation) {
      case Operation::DeleteEdge: {
        mesh.addEdge (this->data.identifier ().getIdRef ());
        break;
      }
      case Operation::DeleteFace: {
        WingedFace& face = mesh.addFace (this->data.value <PrimTriangle> ());

        assert (face.index () == this->data.identifier ().getIndexRef ());
        break;
      }
      case Operation::DeleteVertex: {
        WingedVertex& vertex = mesh.addVertex (this->data.value <glm::vec3> ());

        assert (vertex.index () == this->data.identifier ().getIndexRef ());
        break;
      }
      case Operation::AddEdge: {
        mesh.deleteEdge (this->data.identifier ().getEdgeRef (mesh));
        break;
      }
      case Operation::AddFace: {
        mesh.deleteFace (this->data.identifier ().getFaceRef (mesh));
        break;
      }
      case Operation::AddVertex: {
        mesh.deleteVertex (this->data.identifier ().getVertexRef (mesh));
        break;
      }
      case Operation::InitOctreeRoot: {
        break;
      }
      case Operation::SetIndex: {
        mesh.setIndex ( this->data.identifier ().getIndexRef ()
                      , this->data.value <unsigned int> (ActionDataType::Old) );
        break;
      }
    }
  }

  void runRedo (WingedMesh& mesh) const { 

    switch (this->operation) {
      case Operation::DeleteEdge: {
        mesh.deleteEdge (this->data.identifier ().getEdgeRef (mesh));
        break;
      }
      case Operation::DeleteFace: {
        mesh.deleteFace (this->data.identifier ().getFaceRef (mesh));
        break;
      }
      case Operation::DeleteVertex: {
        mesh.deleteVertex (this->data.identifier ().getVertexRef (mesh));
        break;
      }
      case Operation::AddEdge: {
        mesh.addEdge (this->data.identifier ().getIdRef ());
        break;
      }
      case Operation::AddFace: {
        WingedFace& face = mesh.addFace (this->data.value <PrimTriangle> ());

        assert (face.index () == this->data.identifier ().getIndexRef ());
        break;
      }
      case Operation::AddVertex: {
        WingedVertex& vertex = mesh.addVertex (this->data.value <glm::vec3> ());

        assert (vertex.index () == this->data.identifier ().getIndexRef ());
        break;
      }
      case Operation::InitOctreeRoot: {
        const OctreeRootData& d = this->data.value <OctreeRootData> ();
        mesh.setupOctreeRoot (d.position, d.width);
        break;
      }
      case Operation::SetIndex: {
        mesh.setIndex ( this->data.identifier ().getIndexRef ()
                      , this->data.value <unsigned int> (ActionDataType::New) );
        break;
      }
    }
  }
};

DELEGATE_BIG3   (PAModifyWMesh)
DELEGATE2       (void         , PAModifyWMesh, deleteEdge     , WingedMesh&, WingedEdge&)
DELEGATE2       (void         , PAModifyWMesh, deleteFace     , WingedMesh&, WingedFace&)
DELEGATE2       (void         , PAModifyWMesh, deleteVertex   , WingedMesh&, WingedVertex&)
DELEGATE1       (WingedEdge&  , PAModifyWMesh, addEdge        , WingedMesh&)
DELEGATE2       (WingedFace&  , PAModifyWMesh, addFace        , WingedMesh&, const PrimTriangle&)
DELEGATE2       (WingedVertex&, PAModifyWMesh, addVertex      , WingedMesh&, const glm::vec3&)
DELEGATE3       (void         , PAModifyWMesh, setIndex       , WingedMesh&, unsigned int, unsigned int)
DELEGATE3       (void         , PAModifyWMesh, setupOctreeRoot, WingedMesh&, const glm::vec3&, float)
DELEGATE1_CONST (void         , PAModifyWMesh, runUndo        , WingedMesh&)
DELEGATE1_CONST (void         , PAModifyWMesh, runRedo        , WingedMesh&)
