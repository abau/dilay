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
    DeleteEdge, DeleteFace, DeleteVertex, AddEdge, AddFace, AddVertex, InitOctreeRoot
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

  void saveEdgeOperand (const WingedEdge& edge) {
    this->operandIds.setEdge (0, &edge);
    this->operandIds.setFace (1, edge.leftFace ());
    this->operandIds.setFace (2, edge.rightFace ());
    this->operandIds.setEdge (3, edge.leftPredecessor ());
    this->operandIds.setEdge (4, edge.leftSuccessor ());
    this->operandIds.setEdge (5, edge.rightPredecessor ());
    this->operandIds.setEdge (6, edge.rightSuccessor ());

    this->operandIds.setVertex (0, edge.vertex1 ());
    this->operandIds.setVertex (1, edge.vertex2 ());
  }

  WingedEdge& addSavedEdge (WingedMesh& mesh) {
    return mesh.addEdge (
      WingedEdge ( this->operandIds.getIdRef  (0)
                 , this->operandIds.getVertex (mesh,0), this->operandIds.getVertex (mesh,1)
                 , this->operandIds.getFace   (mesh,1), this->operandIds.getFace   (mesh,2)
                 , this->operandIds.getEdge   (mesh,3), this->operandIds.getEdge   (mesh,4)
                 , this->operandIds.getEdge   (mesh,5), this->operandIds.getEdge   (mesh,6) ));
  }

  void saveFaceOperand (const WingedFace& face, const PrimTriangle& triangle, bool saveIndex) {
    this->operandIds.setFace     (0, &face);
    this->operandIds.setEdge     (1, face.edge  ());
    if (saveIndex) {
      this->operandIds.setIndex  (0, face.index ());
    }
    this->operandData.set <FaceData> (FaceData {triangle});
  }
  
  WingedFace& addSavedFace (WingedMesh& mesh) {
    WingedFace  face ( this->operandIds.getIdRef (0)
                     , this->operandIds.getEdge (mesh,1) );

    if (this->operandIds.numIndices () > 0) {
      return mesh.addFace ( std::move (face)
                          , this->operandData.get <FaceData> ().triangle
                          , this->operandIds.getIndexRef (0) );
    }
    else {
      return mesh.addFace ( std::move (face)
                          , this->operandData.get <FaceData> ().triangle );
    }
  }

  void saveVertexOperand (const WingedVertex& vertex, const glm::vec3& position) {
    this->operandIds.setIndex (0, vertex.index ());
    this->operandData.set <VertexData> (VertexData { position });
  }

  WingedVertex& addSavedVertex (WingedMesh& mesh) {
    return mesh.addVertex ( this->operandData.get <VertexData> ().position
                          , this->operandIds.getIndexRef (0) );
  }

  void deleteEdge (WingedMesh& mesh, WingedEdge& edge) {
    this->operation = Operation::DeleteEdge;
    this->saveEdgeOperand (edge);
    mesh.deleteEdge (edge);
  }

  void deleteFace (WingedMesh& mesh, WingedFace& face) {
    this->deleteFace (mesh, face, face.triangle (mesh));
  }

  void deleteFace (WingedMesh& mesh, WingedFace& face, const PrimTriangle& t) {
    this->operation = Operation::DeleteFace;
    this->saveFaceOperand (face, t, true);
    mesh.deleteFace (face);
  }

  void deleteVertex (WingedMesh& mesh, WingedVertex& vertex) {
    this->operation = Operation::DeleteVertex;
    this->saveVertexOperand (vertex, vertex.vector (mesh));
    mesh.deleteVertex (vertex);
  }

  WingedEdge& addEdge (WingedMesh& mesh, WingedEdge&& edge) {
    this->operation = Operation::AddEdge;
    this->saveEdgeOperand (edge);
    return mesh.addEdge (std::move (edge));
  }

  WingedFace& addFace (WingedMesh& mesh, WingedFace&& face) {
    return this->addFace (mesh, std::move (face), face.triangle (mesh));
  }

  WingedFace& addFace (WingedMesh& mesh, const PrimTriangle& t) {
    return this->addFace (mesh, WingedFace (), t);
  }

  WingedFace& addFace (WingedMesh& mesh, WingedFace&& face, const PrimTriangle& t) {
    this->operation = Operation::AddFace;
    this->saveFaceOperand (           face , t, false);
    return mesh.addFace   (std::move (face), t);
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& vector) {
    this->operation = Operation::AddVertex;
    WingedVertex& vertex = mesh.addVertex (vector);
    this->saveVertexOperand (vertex, vector);
    return vertex;
  }

  void setupOctreeRoot (WingedMesh& mesh, const glm::vec3& pos, float width) {
    this->operation = Operation::InitOctreeRoot;
    this->operandData.set <OctreeRootData> (OctreeRootData {pos, width});
    mesh.setupOctreeRoot (pos, width);
  }

  void runUndo (WingedMesh& mesh) { 

    switch (this->operation) {
      case Operation::DeleteEdge: {
        this->addSavedEdge (mesh);
        break;
      }
      case Operation::DeleteFace: {
        this->addSavedFace (mesh);
        break;
      }
      case Operation::DeleteVertex: {
        this->addSavedVertex (mesh);
        break;
      }
      case Operation::AddEdge: {
        mesh.deleteEdge (*this->operandIds.getEdge (mesh,0));
        break;
      }
      case Operation::AddFace: {
        mesh.deleteFace (*this->operandIds.getFace (mesh,0));
        break;
      }
      case Operation::AddVertex: {
        mesh.deleteVertex (*this->operandIds.getVertex (mesh,0));
        break;
      }
      case Operation::InitOctreeRoot: {
        break;
      }
      default: assert (false);
    }
  }

  void runRedo (WingedMesh& mesh) { 

    switch (this->operation) {
      case Operation::DeleteEdge: {
        mesh.deleteEdge (*this->operandIds.getEdge (mesh,0));
        break;
      }
      case Operation::DeleteFace: {
        mesh.deleteFace (*this->operandIds.getFace (mesh,0));
        break;
      }
      case Operation::DeleteVertex: {
        mesh.deleteVertex (*this->operandIds.getVertex (mesh,0));
        break;
      }
      case Operation::AddEdge: {
        this->addSavedEdge (mesh);
        break;
      }
      case Operation::AddFace: {
        this->addSavedFace (mesh);
        break;
      }
      case Operation::AddVertex: {
        this->addSavedVertex (mesh);
        break;
      }
      case Operation::InitOctreeRoot: {
        mesh.setupOctreeRoot (
            this->operandData.get <OctreeRootData> ().position
          , this->operandData.get <OctreeRootData> ().width
          );
        break;
      }
      default: assert (false);
    }
  }
};

DELEGATE_BIG3 (PAModifyWMesh)

DELEGATE2 (void         , PAModifyWMesh, deleteEdge     , WingedMesh&, WingedEdge&)
DELEGATE2 (void         , PAModifyWMesh, deleteFace     , WingedMesh&, WingedFace&)
DELEGATE3 (void         , PAModifyWMesh, deleteFace     , WingedMesh&, WingedFace&, const PrimTriangle&)
DELEGATE2 (void         , PAModifyWMesh, deleteVertex   , WingedMesh&, WingedVertex&)
DELEGATE2 (WingedEdge&  , PAModifyWMesh, addEdge        , WingedMesh&, WingedEdge&&)
DELEGATE2 (WingedFace&  , PAModifyWMesh, addFace        , WingedMesh&, WingedFace&&)
DELEGATE2 (WingedFace&  , PAModifyWMesh, addFace        , WingedMesh&, const PrimTriangle&)
DELEGATE3 (WingedFace&  , PAModifyWMesh, addFace        , WingedMesh&, WingedFace&&, const PrimTriangle&)
DELEGATE2 (WingedVertex&, PAModifyWMesh, addVertex      , WingedMesh&, const glm::vec3&)
DELEGATE3 (void         , PAModifyWMesh, setupOctreeRoot, WingedMesh&, const glm::vec3&, float)
DELEGATE1 (void         , PAModifyWMesh, runUndo        , WingedMesh&)
DELEGATE1 (void         , PAModifyWMesh, runRedo        , WingedMesh&)
