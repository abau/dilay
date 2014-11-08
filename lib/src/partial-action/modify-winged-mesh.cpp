#include <glm/glm.hpp>
#include "action/ids.hpp"
#include "octree.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "primitive/triangle.hpp"
#include "state.hpp"
#include "variant.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  enum class Operation { 
      ResetEdge, ResetFace, ResetVertex
    , DeleteEdge, DeleteFace, DeleteVertex
    , AddEdge, AddFace, AddVertex
    , InitOctreeRoot, SetIndex
  };

  struct FaceData {
    PrimTriangle triangle;
  };

  struct VertexData {
    glm::vec3 position;
  };

  struct OctreeRootData {
    glm::vec3 position;
    float     width;
  };
};

struct PAModifyWMesh :: Impl {
  Operation operation;
  ActionIds operandIds;

  Variant <FaceData, VertexData, OctreeRootData> operandData;

  void resetEdge (WingedEdge& edge) {
    this->operation = Operation::ResetEdge;
    this->operandIds.setEdge   (0, &edge);
    this->operandIds.setFace   (1, edge.leftFace ());
    this->operandIds.setFace   (2, edge.rightFace ());
    this->operandIds.setEdge   (3, edge.leftPredecessor ());
    this->operandIds.setEdge   (4, edge.leftSuccessor ());
    this->operandIds.setEdge   (5, edge.rightPredecessor ());
    this->operandIds.setEdge   (6, edge.rightSuccessor ());
    this->operandIds.setVertex (0, edge.vertex1 ());
    this->operandIds.setVertex (1, edge.vertex2 ());

    edge.setGeometry (nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr);
  }

  void resetFace (WingedFace& face) {
    this->operation = Operation::ResetFace;
    this->operandIds.setFace (0, &face);
    this->operandIds.setEdge (1, face.edge  ());

    face.edge (nullptr);
  }

  void resetVertex (WingedVertex& vertex) {
    this->operation = Operation::ResetVertex;
    this->operandIds.setIndex (0, vertex.index ());
    this->operandIds.setEdge  (0, vertex.edge  ());

    vertex.edge (nullptr);
  }

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
    this->operandIds.setEdge (0, &edge);
    mesh.deleteEdge (edge);
  }

  void deleteFace (WingedMesh& mesh, WingedFace& face) {
    assert (face.edge () == nullptr);

    this->operation = Operation::DeleteFace;
    this->operandIds.setFace         (0, &face);
    this->operandIds.setIndex        (0, face.index ());
    this->operandData.set <FaceData> (FaceData {face.triangle (mesh)});

    mesh.deleteFace (face);
  }

  void deleteVertex (WingedMesh& mesh, WingedVertex& vertex) {
    assert (vertex.edge () == nullptr);

    this->operation = Operation::DeleteVertex;
    this->operandIds.setIndex (0, vertex.index ());
    this->operandData.set <VertexData> (VertexData { vertex.vector (mesh) });

    mesh.deleteVertex (vertex);
  }

  WingedEdge& addEdge (WingedMesh& mesh) {
    WingedEdge& edge = mesh.addEdge (Id ());
    this->operation  = Operation::AddEdge;
    this->operandIds.setEdge (0, &edge);
    return edge;
  }

  WingedFace& addFace (WingedMesh& mesh, const PrimTriangle& triangle) {
    WingedFace& face = mesh.addFace (Id (), triangle);
    this->operation  = Operation::AddFace;
    this->operandIds.setFace         (0, &face);
    this->operandIds.setIndex        (0, face.index ());
    this->operandData.set <FaceData> (FaceData {triangle});
    return face;
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& vector) {
    WingedVertex& vertex = mesh.addVertex (vector);
    this->operation      = Operation::AddVertex;
    this->operandIds.setIndex          (0, vertex.index ());
    this->operandData.set <VertexData> (VertexData { vector });
    return vertex;
  }

  void setupOctreeRoot (WingedMesh& mesh, const glm::vec3& pos, float width) {
    this->operation = Operation::InitOctreeRoot;
    this->operandData.set <OctreeRootData> (OctreeRootData {pos, width});
    mesh.setupOctreeRoot (pos, width);
  }

  void setIndex (WingedMesh& mesh, unsigned int index, unsigned int vertexIndex) {
    this->operation = Operation::SetIndex;
    this->operandIds.setIndex (0, index);
    this->operandIds.setIndex (1, mesh.index (index));
    this->operandIds.setIndex (2, vertexIndex);

    mesh.setIndex (index, vertexIndex);
  }

  void runUndo (WingedMesh& mesh) { 

    switch (this->operation) {
      case Operation::ResetEdge: {
        this->operandIds.getEdgeRef  (mesh,0)
                        .setGeometry ( this->operandIds.getVertex (mesh,0)
                                     , this->operandIds.getVertex (mesh,1)
                                     , this->operandIds.getFace   (mesh,1)
                                     , this->operandIds.getFace   (mesh,2)
                                     , this->operandIds.getEdge   (mesh,3)
                                     , this->operandIds.getEdge   (mesh,4)
                                     , this->operandIds.getEdge   (mesh,5)
                                     , this->operandIds.getEdge   (mesh,6) );
        break;
      }
      case Operation::ResetFace: {
        this->operandIds.getFaceRef (mesh,0)
                        .edge       (this->operandIds.getEdge (mesh,1));
        break;
      }
      case Operation::ResetVertex: {
        this->operandIds.getVertexRef (mesh,0)
                        .edge         (this->operandIds.getEdge (mesh,0));
        break;
      }
      case Operation::DeleteEdge: {
        this->runRedo (mesh, Operation::AddEdge);
        break;
      }
      case Operation::DeleteFace: {
        mesh.addFace ( this->operandIds.getIdRef (0)
                     , this->operandData.get <FaceData> ().triangle
                     , this->operandIds.getIndexRef (0) );
        break;
      }
      case Operation::DeleteVertex: {
        this->runRedo (mesh, Operation::AddVertex);
        break;
      }
      case Operation::AddEdge: {
        this->runRedo (mesh, Operation::DeleteEdge);
        break;
      }
      case Operation::AddFace: {
        this->runRedo (mesh, Operation::DeleteFace);
        break;
      }
      case Operation::AddVertex: {
        this->runRedo (mesh, Operation::DeleteVertex);
        break;
      }
      case Operation::InitOctreeRoot: {
        break;
      }
      case Operation::SetIndex: {
        mesh.setIndex (this->operandIds.getIndexRef (0), this->operandIds.getIndexRef (1));
        break;
      }
    }
  }

  void runRedo (WingedMesh& mesh) { 
    this->runRedo (mesh, this->operation);
  }

  void runRedo (WingedMesh& mesh, Operation op) { 

    switch (op) {
      case Operation::ResetEdge: {
        this->operandIds.getEdgeRef  (mesh,0)
                        .setGeometry ( nullptr, nullptr, nullptr, nullptr
                                     , nullptr, nullptr, nullptr, nullptr );
        break;
      }
      case Operation::ResetFace: {
        this->operandIds.getFaceRef (mesh,0)
                        .edge       (nullptr);
        break;
      }
      case Operation::ResetVertex: {
        this->operandIds.getVertexRef (mesh,0)
                        .edge         (nullptr);
        break;
      }
      case Operation::DeleteEdge: {
        mesh.deleteEdge (this->operandIds.getEdgeRef (mesh,0));
        break;
      }
      case Operation::DeleteFace: {
        mesh.deleteFace (this->operandIds.getFaceRef (mesh,0));
        break;
      }
      case Operation::DeleteVertex: {
        mesh.deleteVertex (this->operandIds.getVertexRef (mesh,0));
        break;
      }
      case Operation::AddEdge: {
        mesh.addEdge (this->operandIds.getIdRef (0));
        break;
      }
      case Operation::AddFace: {
        WingedFace& face = mesh.addFace ( this->operandIds.getIdRef (0)
                                        , this->operandData.get <FaceData> ().triangle );

        assert (face.index () == this->operandIds.getIndexRef (0));
        break;
      }
      case Operation::AddVertex: {
        mesh.addVertex ( this->operandData.get <VertexData> ().position
                       , this->operandIds.getIndexRef (0) );
        break;
      }
      case Operation::InitOctreeRoot: {
        mesh.setupOctreeRoot (
            this->operandData.get <OctreeRootData> ().position
          , this->operandData.get <OctreeRootData> ().width
          );
        break;
      }
      case Operation::SetIndex: {
        mesh.setIndex (this->operandIds.getIndexRef (0), this->operandIds.getIndexRef (2));
        break;
      }
    }
  }
};

DELEGATE_BIG3 (PAModifyWMesh)

DELEGATE1 (void         , PAModifyWMesh, resetEdge      , WingedEdge&)
DELEGATE1 (void         , PAModifyWMesh, resetFace      , WingedFace&)
DELEGATE1 (void         , PAModifyWMesh, resetVertex    , WingedVertex&)
DELEGATE2 (void         , PAModifyWMesh, deleteEdge     , WingedMesh&, WingedEdge&)
DELEGATE2 (void         , PAModifyWMesh, deleteFace     , WingedMesh&, WingedFace&)
DELEGATE2 (void         , PAModifyWMesh, deleteVertex   , WingedMesh&, WingedVertex&)
DELEGATE1 (WingedEdge&  , PAModifyWMesh, addEdge        , WingedMesh&)
DELEGATE2 (WingedFace&  , PAModifyWMesh, addFace        , WingedMesh&, const PrimTriangle&)
DELEGATE2 (WingedVertex&, PAModifyWMesh, addVertex      , WingedMesh&, const glm::vec3&)
DELEGATE3 (void         , PAModifyWMesh, setIndex       , WingedMesh&, unsigned int, unsigned int)
DELEGATE3 (void         , PAModifyWMesh, setupOctreeRoot, WingedMesh&, const glm::vec3&, float)
DELEGATE1 (void         , PAModifyWMesh, runUndo        , WingedMesh&)
DELEGATE1 (void         , PAModifyWMesh, runRedo        , WingedMesh&)
