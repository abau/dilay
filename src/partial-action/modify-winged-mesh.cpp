#include <glm/glm.hpp>
#include "partial-action/modify-winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "triangle.hpp"
#include "octree.hpp"
#include "variant.hpp"

enum class Operation { 
  DeleteEdge, DeleteFace, PopVertex, AddEdge, AddFace, AddVertex, RealignFace
};

struct EdgeData {
  bool         isTEdge;
  FaceGradient faceGradient;
  int          vertexGradient;
};

struct FaceData {
  Triangle     triangle;
};

struct VertexData {
  glm::vec3 position;
};

struct PAModifyWMesh :: Impl {
  Operation operation;
  ActionIds operandIds;

  Variant <EdgeData, FaceData, VertexData> operandData;

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
    EdgeData* data = this->operandData.get <EdgeData> ();
    return mesh.addEdge (
      WingedEdge ( this->operandIds.getVertex (mesh,0), this->operandIds.getVertex (mesh,1)
                 , this->operandIds.getFace   (mesh,1), this->operandIds.getFace   (mesh,2)
                 , this->operandIds.getEdge   (mesh,3), this->operandIds.getEdge   (mesh,4)
                 , this->operandIds.getEdge   (mesh,5), this->operandIds.getEdge   (mesh,6)
                 , this->operandIds.getEdge   (mesh,7), this->operandIds.getEdge   (mesh,8)
                 ,*this->operandIds.getId     (0)     , data->isTEdge
                 , data->faceGradient                 , data->vertexGradient));
  }

  void saveFaceOperand (const WingedFace& face, const Triangle& triangle) {
    this->operandIds.setFace     (0, &face);
    this->operandIds.setEdge     (1, face.edge ());
    this->operandData.set <FaceData> (FaceData {triangle});
  }
  
  WingedFace& addSavedFace (WingedMesh& mesh) {
    return mesh.addFace ( 
        WingedFace (this->operandIds.getEdge (mesh,1), *this->operandIds.getId (0))
      , this->operandData.get <FaceData> ()->triangle);
  }

  void saveVertexOperand (const WingedMesh& mesh, const WingedVertex& vertex) {
    this->operandData.set <VertexData> (VertexData {vertex.vertex (mesh)});
  }

  WingedVertex& addSavedVertex (WingedMesh& mesh) {
    return mesh.addVertex (this->operandData.get <VertexData> ()->position);
  }

  void deleteEdge (WingedMesh& mesh, const WingedEdge& edge) {
    this->operation = Operation::DeleteEdge;
    this->saveEdgeOperand (edge);
    mesh.deleteEdge (edge);
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face) {
    this->deleteFace (mesh, face, face.triangle (mesh));
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face, const Triangle& t) {
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

  WingedFace& addFace (WingedMesh& mesh, const Triangle& t) {
    return this->addFace (mesh,WingedFace (),t);
  }

  WingedFace& addFace (WingedMesh& mesh, const WingedFace& face, const Triangle& t) {
    this->operation = Operation::AddFace;
    this->saveFaceOperand (face, t);
    return mesh.addFace (face, t);
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& vector) {
    this->operation = Operation::AddVertex;
    this->operandData.set <VertexData> (VertexData {vector});
    return mesh.addVertex (vector);
  }

  WingedFace& realignFace (WingedMesh& mesh, const WingedFace& face, const Triangle& triangle) {
    assert (face.octreeNode ());
    this->operation = Operation::RealignFace;
    this->operandIds.setIds ({ face.id (), face.octreeNode ()->id ()}); 
    this->operandData.set <FaceData> (FaceData {triangle});
    return mesh.realignFace (face, triangle);
  }

  void undo (WingedMesh& mesh) { 

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
      case Operation::RealignFace: {
        this->operandIds.getFace (mesh,0)->octreeNode (
            &mesh.octreeNodeSLOW (*this->operandIds.getId (1)));
        break;
      }
      default: assert (false);
    }
  }

  void redo (WingedMesh& mesh) { 

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
      case Operation::RealignFace: {
        mesh.realignFace (*this->operandIds.getFace (mesh,0)
                         , this->operandData.get <FaceData> ()->triangle);
        break;
      }
      default: assert (false);
    }
  }
};

DELEGATE_ACTION_BIG6 (PAModifyWMesh)

DELEGATE2 (void         ,PAModifyWMesh,deleteEdge  ,WingedMesh&,const WingedEdge&)
DELEGATE2 (void         ,PAModifyWMesh,deleteFace  ,WingedMesh&,const WingedFace&)
DELEGATE3 (void         ,PAModifyWMesh,deleteFace  ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE1 (void         ,PAModifyWMesh,popVertex   ,WingedMesh&)
DELEGATE2 (WingedEdge&  ,PAModifyWMesh,addEdge     ,WingedMesh&,const WingedEdge&)
DELEGATE2 (WingedFace&  ,PAModifyWMesh,addFace     ,WingedMesh&,const WingedFace&)
DELEGATE2 (WingedFace&  ,PAModifyWMesh,addFace     ,WingedMesh&,const Triangle&)
DELEGATE3 (WingedFace&  ,PAModifyWMesh,addFace     ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE2 (WingedVertex&,PAModifyWMesh,addVertex   ,WingedMesh&,const glm::vec3&)
DELEGATE3 (WingedFace&  ,PAModifyWMesh,realignFace ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE1 (void         ,PAModifyWMesh,undo        ,WingedMesh&)
DELEGATE1 (void         ,PAModifyWMesh,redo        ,WingedMesh&)
