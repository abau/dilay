#include <glm/glm.hpp>
#include "partial-action/modify-winged-mesh.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/edge.hpp"
#include "winged/vertex.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "primitive/triangle.hpp"
#include "octree.hpp"
#include "variant.hpp"

namespace {
  enum class Operation { 
    DeleteEdge, DeleteFace, PopVertex, AddEdge, AddFace, AddVertex, InitOctreeRoot
  };

  struct EdgeData {
    bool         isTEdge;
    FaceGradient faceGradient;
    int          vertexGradient;
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

  Variant <EdgeData, FaceData, VertexData, OctreeRootData> operandData;

  void saveEdgeOperand (const WingedEdge& edge) {
    this->operandIds.setEdge (0, &edge);
    this->operandIds.setFace (1, edge.leftFace ());
    this->operandIds.setFace (2, edge.rightFace ());
    this->operandIds.setEdge (3, edge.leftPredecessor ());
    this->operandIds.setEdge (4, edge.leftSuccessor ());
    this->operandIds.setEdge (5, edge.rightPredecessor ());
    this->operandIds.setEdge (6, edge.rightSuccessor ());
    this->operandIds.setEdge (7, edge.previousSibling ());
    this->operandIds.setEdge (8, edge.nextSibling ());

    this->operandIds.setVertex (0, edge.vertex1 ());
    this->operandIds.setVertex (1, edge.vertex2 ());

    this->operandData.set <EdgeData> (
        EdgeData { edge.isTEdge ()
                 , edge.faceGradient ()
                 , edge.vertexGradient () });
  }

  WingedEdge& addSavedEdge (WingedMesh& mesh) {
    EdgeData& data = this->operandData.get <EdgeData> ();
    return mesh.addEdge (
      WingedEdge ( this->operandIds.getVertex (mesh,0), this->operandIds.getVertex (mesh,1)
                 , this->operandIds.getFace   (mesh,1), this->operandIds.getFace   (mesh,2)
                 , this->operandIds.getEdge   (mesh,3), this->operandIds.getEdge   (mesh,4)
                 , this->operandIds.getEdge   (mesh,5), this->operandIds.getEdge   (mesh,6)
                 , this->operandIds.getEdge   (mesh,7), this->operandIds.getEdge   (mesh,8)
                 , this->operandIds.getIdRef  (0)     , data. isTEdge
                 , data. faceGradient                 , data. vertexGradient));
  }

  void saveFaceOperand (const WingedFace& face, const PrimTriangle& triangle) {
    this->operandIds.setFace     (0, &face);
    this->operandIds.setEdge     (1, face.edge ());
    this->operandData.set <FaceData> (FaceData {triangle});
  }
  
  WingedFace& addSavedFace (WingedMesh& mesh) {
    return mesh.addFace ( 
        WingedFace (this->operandIds.getEdge (mesh,1), this->operandIds.getIdRef (0))
      , this->operandData.get <FaceData> ().triangle);
  }

  void saveVertexOperand (const WingedMesh& mesh, const WingedVertex& vertex) {
    this->operandData.set <VertexData> (VertexData {vertex.vertex (mesh)});
  }

  WingedVertex& addSavedVertex (WingedMesh& mesh) {
    return mesh.addVertex (this->operandData.get <VertexData> ().position);
  }

  void deleteEdge (WingedMesh& mesh, const WingedEdge& edge) {
    this->operation = Operation::DeleteEdge;
    this->saveEdgeOperand (edge);
    mesh.deleteEdge (edge);
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face) {
    this->deleteFace (mesh, face, face.triangle (mesh));
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face, const PrimTriangle& t) {
    this->operation = Operation::DeleteFace;
    this->saveFaceOperand (face, t);
    mesh.deleteFace (face);
  }

  void popVertex (WingedMesh& mesh) {
    this->operation = Operation::PopVertex;
    this->saveVertexOperand (mesh,mesh.lastVertex ());
    mesh.popVertex ();
  }

  WingedEdge& addEdge (WingedMesh& mesh, const WingedEdge& edge) {
    this->operation = Operation::AddEdge;
    this->saveEdgeOperand (edge);
    return mesh.addEdge (edge);
  }

  WingedFace& addFace (WingedMesh& mesh, const WingedFace& face) {
    return this->addFace (mesh,face,face.triangle (mesh));
  }

  WingedFace& addFace (WingedMesh& mesh, const PrimTriangle& t) {
    return this->addFace (mesh,WingedFace (),t);
  }

  WingedFace& addFace (WingedMesh& mesh, const WingedFace& face, const PrimTriangle& t) {
    this->operation = Operation::AddFace;
    this->saveFaceOperand (face, t);
    return mesh.addFace (face, t);
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& vector) {
    this->operation = Operation::AddVertex;
    this->operandData.set <VertexData> (VertexData {vector});
    return mesh.addVertex (vector);
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
      case Operation::PopVertex: {
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
        mesh.popVertex ();
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
      case Operation::PopVertex: {
        mesh.popVertex ();
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

DELEGATE2 (void         ,PAModifyWMesh,deleteEdge     ,WingedMesh&,const WingedEdge&)
DELEGATE3 (void         ,PAModifyWMesh,deleteFace     ,WingedMesh&,const WingedFace&,const PrimTriangle&)
DELEGATE1 (void         ,PAModifyWMesh,popVertex      ,WingedMesh&)
DELEGATE2 (WingedEdge&  ,PAModifyWMesh,addEdge        ,WingedMesh&,const WingedEdge&)
DELEGATE2 (WingedFace&  ,PAModifyWMesh,addFace        ,WingedMesh&,const WingedFace&)
DELEGATE2 (WingedFace&  ,PAModifyWMesh,addFace        ,WingedMesh&,const PrimTriangle&)
DELEGATE3 (WingedFace&  ,PAModifyWMesh,addFace        ,WingedMesh&,const WingedFace&,const PrimTriangle&)
DELEGATE2 (WingedVertex&,PAModifyWMesh,addVertex      ,WingedMesh&,const glm::vec3&)
DELEGATE3 (void         ,PAModifyWMesh,setupOctreeRoot,WingedMesh&,const glm::vec3&,float)
DELEGATE1 (void         ,PAModifyWMesh,runUndo        ,WingedMesh&)
DELEGATE1 (void         ,PAModifyWMesh,runRedo        ,WingedMesh&)
