#include <memory>
#include <glm/glm.hpp>
#include "partial-action/modify-mesh.hpp"
#include "macro.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "state.hpp"
#include "partial-action/ids.hpp"
#include "triangle.hpp"
#include "octree.hpp"

enum class Operation { 
  DeleteEdge, DeleteFace, PopVertex, AddEdge, AddFace, AddVertex, RealignFace
};

struct PAModifyMesh :: Impl {
  Operation                        operation;
  PAIds                            operands;
  std::unique_ptr <bool>           isTEdge;
  std::unique_ptr <Triangle>       triangle;
  std::unique_ptr <glm::vec3>      vector;
  std::unique_ptr <unsigned int>   level;
  std::unique_ptr <unsigned int>   firstIndexNumber;
  std::unique_ptr <FaceGradient>   faceGradient;
  std::unique_ptr <int>            vertexGradient;

  void saveEdgeOperand (const WingedMesh& mesh, const WingedEdge& edge) {
    this->operands.setMesh (0, &mesh);
    this->operands.setEdge (1, &edge);
    this->operands.setFace (2, edge.leftFace ());
    this->operands.setFace (3, edge.rightFace ());
    this->operands.setEdge (4, edge.leftPredecessor ());
    this->operands.setEdge (5, edge.leftSuccessor ());
    this->operands.setEdge (6, edge.rightPredecessor ());
    this->operands.setEdge (7, edge.rightSuccessor ());
    this->operands.setEdge (8, edge.previousSibling ());
    this->operands.setEdge (9, edge.nextSibling ());

    this->operands.setVertex   (0, edge.vertex1 ());
    this->operands.setVertex   (1, edge.vertex2 ());

    this->isTEdge.reset        (new bool (edge.isTEdge ()));
    this->faceGradient.reset   (new FaceGradient (edge.faceGradient ()));
    this->vertexGradient.reset (new int (edge.vertexGradient ()));
  }

  WingedEdge& addSavedEdge (WingedMesh& mesh) {
    return mesh.addEdge (
      WingedEdge ( this->operands.getVertex (mesh,0), this->operands.getVertex (mesh,1)
                 , this->operands.getFace   (mesh,2), this->operands.getFace   (mesh,3)
                 , this->operands.getEdge   (mesh,4), this->operands.getEdge   (mesh,5)
                 , this->operands.getEdge   (mesh,6), this->operands.getEdge   (mesh,7)
                 , this->operands.getEdge   (mesh,8), this->operands.getEdge   (mesh,9)
                 ,*this->operands.getId     (1)     ,*this->isTEdge
                 ,*this->faceGradient, *this->vertexGradient));
  }

  void saveFaceOperand (const WingedMesh& mesh, const WingedFace& face, const Triangle& triangle) {
    this->operands.setMesh       (0, &mesh);
    this->operands.setFace       (1, &face);
    this->operands.setEdge       (2, face.edge ());
    this->triangle.reset         (new Triangle (triangle));
    this->firstIndexNumber.reset (new unsigned int (face.firstIndexNumber ()));
  }
  
  WingedFace& addSavedFace (WingedMesh& mesh) {
    return mesh.addFace ( 
        WingedFace (this->operands.getEdge (mesh,2), *this->operands.getId (1))
      ,*this->triangle);
  }

  void saveVertexOperand (const WingedMesh& mesh, const WingedVertex& vertex) {
    this->operands.setMesh (0, &mesh);
    this->vector.reset     (new glm::vec3 (vertex.vertex (mesh)));
    this->level.reset      (new unsigned int (vertex.level ()));
  }

  WingedVertex& addSavedVertex (WingedMesh& mesh) {
    return mesh.addVertex (*this->vector, *this->level);
  }

  void deleteEdge (WingedMesh& mesh, const WingedEdge& edge) {
    this->operation = Operation::DeleteEdge;
    this->saveEdgeOperand (mesh,edge);
    mesh.deleteEdge (edge);
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face) {
    this->deleteFace (mesh, face, face.triangle (mesh));
  }

  void deleteFace (WingedMesh& mesh, const WingedFace& face, const Triangle& t) {
    this->operation = Operation::DeleteFace;
    this->saveFaceOperand (mesh, face, t);
    mesh.deleteFace (face);
  }

  void popVertex (WingedMesh& mesh) {
    this->operation = Operation::PopVertex;
    this->saveVertexOperand (mesh,mesh.lastVertex ());
    mesh.popVertex ();
  }

  WingedEdge& addEdge (WingedMesh& mesh, const WingedEdge& edge) {
    this->operation = Operation::AddEdge;
    this->saveEdgeOperand (mesh,edge);
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
    this->saveFaceOperand (mesh, face, t);
    return mesh.addFace (face, t);
  }

  WingedVertex& addVertex (WingedMesh& mesh, const glm::vec3& vector, unsigned int level) {
    this->operation = Operation::AddVertex;
    this->operands.setMesh (0, &mesh);
    this->vector.reset     (new glm::vec3 (vector));
    this->level.reset      (new unsigned int (level));
    return mesh.addVertex (vector, level);
  }

  WingedFace& realignFace (WingedMesh& mesh, const WingedFace& face, const Triangle& triangle) {
    assert (face.octreeNode ());
    this->operation = Operation::RealignFace;
    this->operands.setIds ({ mesh.id (), face.id (), face.octreeNode ()->id ()}); 
    this->triangle.reset  (new Triangle (triangle));
    return mesh.realignFace (face, triangle);
  }

  void undo () { 
    WingedMesh& mesh = this->operands.getMesh (0);

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
        mesh.deleteEdge (*this->operands.getEdge (mesh,1));
        break;
      }
      case Operation::AddFace: {
        mesh.deleteFace (*this->operands.getFace (mesh,1));
        break;
      }
      case Operation::AddVertex: {
        mesh.popVertex ();
        break;
      }
      case Operation::RealignFace: {
        this->operands.getFace (mesh,1)->octreeNode (
            &mesh.octreeNodeSLOW (*this->operands.getId (2)));
        break;
      }
      default: assert (false);
    }
  }

  void redo () { 
    WingedMesh& mesh = this->operands.getMesh (0);

    switch (this->operation) {
      case Operation::DeleteEdge: {
        mesh.deleteEdge (*this->operands.getEdge (mesh,1));
        break;
      }
      case Operation::DeleteFace: {
        mesh.deleteFace (*this->operands.getFace (mesh,1));
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
        mesh.realignFace (*this->operands.getFace (mesh,1), *this->triangle);
        break;
      }
      default: assert (false);
    }
  }
};

DELEGATE_CONSTRUCTOR (PAModifyMesh)
DELEGATE_DESTRUCTOR  (PAModifyMesh)

DELEGATE2 (void         ,PAModifyMesh,deleteEdge  ,WingedMesh&,const WingedEdge&)
DELEGATE2 (void         ,PAModifyMesh,deleteFace  ,WingedMesh&,const WingedFace&)
DELEGATE3 (void         ,PAModifyMesh,deleteFace  ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE1 (void         ,PAModifyMesh,popVertex   ,WingedMesh&)
DELEGATE2 (WingedEdge&  ,PAModifyMesh,addEdge     ,WingedMesh&,const WingedEdge&)
DELEGATE2 (WingedFace&  ,PAModifyMesh,addFace     ,WingedMesh&,const WingedFace&)
DELEGATE2 (WingedFace&  ,PAModifyMesh,addFace     ,WingedMesh&,const Triangle&)
DELEGATE3 (WingedFace&  ,PAModifyMesh,addFace     ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE3 (WingedVertex&,PAModifyMesh,addVertex   ,WingedMesh&,const glm::vec3&,unsigned int)
DELEGATE3 (WingedFace&  ,PAModifyMesh,realignFace ,WingedMesh&,const WingedFace&,const Triangle&)
DELEGATE  (void         ,PAModifyMesh,undo)
DELEGATE  (void         ,PAModifyMesh,redo)
