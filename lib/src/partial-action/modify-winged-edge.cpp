#include "action/ids.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "state.hpp"
#include "variant.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  enum class Operation 
    { Vertex1, Vertex2, LeftFace, RightFace
    , LeftPredecessor, LeftSuccessor, RightPredecessor, RightSuccessor
    , SetGeometry
    };
};

struct PAModifyWEdge :: Impl {
  Operation          operation;
  ActionIds          operands; 
  Variant <bool,int> operandData;

  void vertex1 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex1;
    this->operands.setEdge   (0, &edge);
    this->operands.setVertex (0, edge.vertex1 ());
    edge.vertex1 (v);
  }

  void vertex2 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex2;
    this->operands.setEdge   (0, &edge);
    this->operands.setVertex (0, edge.vertex2 ());
    edge.vertex2 (v);
  }

  void leftFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::LeftFace;
    this->operands.setEdge (0, &edge);
    this->operands.setFace (1, edge.leftFace ());
    edge.leftFace (f);
  }

  void rightFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::RightFace;
    this->operands.setEdge (0, &edge);
    this->operands.setFace (1, edge.rightFace ());
    edge.rightFace (f);
  }

  void leftPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftPredecessor;
    this->operands.setEdge (0, &edge);
    this->operands.setEdge (1, edge.leftPredecessor ());
    edge.leftPredecessor (e);
  }

  void leftSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftSuccessor;
    this->operands.setEdge (0, &edge);
    this->operands.setEdge (1, edge.leftSuccessor ());
    edge.leftSuccessor (e);
  }

  void rightPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightPredecessor;
    this->operands.setEdge (0, &edge);
    this->operands.setEdge (1, edge.rightPredecessor ());
    edge.rightPredecessor (e);
  }

  void rightSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightSuccessor;
    this->operands.setEdge (0, &edge);
    this->operands.setEdge (1, edge.rightSuccessor ());
    edge.rightSuccessor (e);
  }

  void firstVertex (WingedEdge& edge, const WingedFace& f, WingedVertex* v) {
    if (edge.isLeftFace (f))
      this->vertex1 (edge, v);
    else
      this->vertex2 (edge, v);
  }

  void secondVertex (WingedEdge& edge, const WingedFace& f, WingedVertex* v) {
    if (edge.isLeftFace (f))
      this->vertex2 (edge, v);
    else
      this->vertex1 (edge, v);
  }

  void vertex (WingedEdge& edge, const WingedVertex& vertex, WingedVertex* v) {
    if (edge.isVertex1 (vertex))
      this->vertex1 (edge, v);
    else 
      this->vertex2 (edge, v);
  }

  void face (WingedEdge& edge, const WingedFace& fOld, WingedFace* fNew) {
    if (edge.isLeftFace (fOld))
      this->leftFace (edge,fNew);
    else
      this->rightFace (edge,fNew);
  }

  void predecessor (WingedEdge& edge, const WingedFace& f, WingedEdge* e) {
    if (edge.isLeftFace (f)) 
      this->leftPredecessor  (edge, e);
    else 
      this->rightPredecessor (edge, e);
  }

  void successor (WingedEdge& edge, const WingedFace& f, WingedEdge* e) {
    if (edge.isLeftFace (f)) 
      this->leftSuccessor  (edge, e);
    else 
      this->rightSuccessor (edge, e);
  }
  
  void setGeometry ( WingedEdge& edge
                   , WingedVertex* v1, WingedVertex* v2
                   , WingedFace* lF, WingedFace* rF
                   , WingedEdge* lP, WingedEdge* lS
                   , WingedEdge* rP, WingedEdge* rS) {
    this->operation = Operation::SetGeometry;
    this->operands.setVertex (0,  edge.vertex1          ());
    this->operands.setVertex (1,  edge.vertex2          ());

    this->operands.setEdge   (0, &edge);
    this->operands.setFace   (1,  edge.leftFace         ());
    this->operands.setFace   (2,  edge.rightFace        ());
    this->operands.setEdge   (3,  edge.leftPredecessor  ());
    this->operands.setEdge   (4,  edge.leftSuccessor    ());
    this->operands.setEdge   (5,  edge.rightPredecessor ());
    this->operands.setEdge   (6,  edge.rightSuccessor   ());

    edge.setGeometry (v1,v2,lF,rF,lP,lS,rP,rS);
  }

  void toggle (WingedMesh& mesh) { 
    WingedEdge& edge = this->operands.getEdgeRef (mesh,0);

    switch (this->operation) {
      case Operation::Vertex1: {
        WingedVertex* v = edge.vertex1 ();
        edge.vertex1 (this->operands.getVertex (mesh,0)); 
        this->operands.setVertex (0,v);
        break;
      }
      case Operation::Vertex2: {
        WingedVertex* v = edge.vertex2 ();
        edge.vertex2 (this->operands.getVertex (mesh,0)); 
        this->operands.setVertex (0,v);
        break;
      }
      case Operation::LeftFace: {
        WingedFace* f = edge.leftFace ();
        edge.leftFace (this->operands.getFace (mesh,1));
        this->operands.setFace (1,f);
        break;
      }
      case Operation::RightFace: {
        WingedFace* f = edge.rightFace ();
        edge.rightFace (this->operands.getFace (mesh,1));
        this->operands.setFace (1,f);
        break;
      }
      case Operation::LeftPredecessor: {
        WingedEdge* e = edge.leftPredecessor ();
        edge.leftPredecessor (this->operands.getEdge (mesh,1));
        this->operands.setEdge (1,e);
        break;
      }
      case Operation::LeftSuccessor: {
        WingedEdge* e = edge.leftSuccessor ();
        edge.leftSuccessor (this->operands.getEdge (mesh,1));
        this->operands.setEdge (1,e);
        break;
      }
      case Operation::RightPredecessor: {
        WingedEdge* e = edge.rightPredecessor ();
        edge.rightPredecessor (this->operands.getEdge (mesh,1));
        this->operands.setEdge (1,e);
        break;
      }
      case Operation::RightSuccessor: {
        WingedEdge* e = edge.rightSuccessor ();
        edge.rightSuccessor (this->operands.getEdge (mesh,1));
        this->operands.setEdge (1,e);
        break;
      }
      case Operation::SetGeometry: {
        WingedVertex* v1 = edge.vertex1          ();
        WingedVertex* v2 = edge.vertex2          ();
        WingedFace*   lF = edge.leftFace         ();
        WingedFace*   rF = edge.rightFace        ();
        WingedEdge*   lP = edge.leftPredecessor  ();
        WingedEdge*   lS = edge.leftSuccessor    ();
        WingedEdge*   rP = edge.rightPredecessor ();
        WingedEdge*   rS = edge.rightSuccessor   ();

        edge.setGeometry 
          ( this->operands.getVertex (mesh,0),  this->operands.getVertex (mesh,1)
          , this->operands.getFace   (mesh,1),  this->operands.getFace   (mesh,2)
          , this->operands.getEdge   (mesh,3),  this->operands.getEdge   (mesh,4)
          , this->operands.getEdge   (mesh,5),  this->operands.getEdge   (mesh,6));

        this->operands.setVertex (0, v1);
        this->operands.setVertex (1, v2);

        this->operands.setFace   (1, lF);
        this->operands.setFace   (2, rF);
        this->operands.setEdge   (3, lP);
        this->operands.setEdge   (4, lS);
        this->operands.setEdge   (5, rP);
        this->operands.setEdge   (6, rS);
        break;
      }
      default: assert (false);
    }
  }

  void runUndo (WingedMesh& mesh) { this->toggle (mesh); }
  void runRedo (WingedMesh& mesh) { this->toggle (mesh); }
};

DELEGATE_BIG3 (PAModifyWEdge)

DELEGATE2 (void,PAModifyWEdge,vertex1         ,WingedEdge&,WingedVertex*)
DELEGATE2 (void,PAModifyWEdge,vertex2         ,WingedEdge&,WingedVertex*)
DELEGATE2 (void,PAModifyWEdge,leftFace        ,WingedEdge&,WingedFace*)
DELEGATE2 (void,PAModifyWEdge,rightFace       ,WingedEdge&,WingedFace*)
DELEGATE2 (void,PAModifyWEdge,leftPredecessor ,WingedEdge&,WingedEdge*)
DELEGATE2 (void,PAModifyWEdge,leftSuccessor   ,WingedEdge&,WingedEdge*)
DELEGATE2 (void,PAModifyWEdge,rightPredecessor,WingedEdge&,WingedEdge*)
DELEGATE2 (void,PAModifyWEdge,rightSuccessor  ,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyWEdge,firstVertex     ,WingedEdge&,const WingedFace&,WingedVertex*)
DELEGATE3 (void,PAModifyWEdge,secondVertex    ,WingedEdge&,const WingedFace&,WingedVertex*)
DELEGATE3 (void,PAModifyWEdge,vertex          ,WingedEdge&,const WingedVertex&,WingedVertex*)
DELEGATE3 (void,PAModifyWEdge,face            ,WingedEdge&,const WingedFace&,WingedFace*)
DELEGATE3 (void,PAModifyWEdge,predecessor     ,WingedEdge&,const WingedFace&,WingedEdge*)
DELEGATE3 (void,PAModifyWEdge,successor       ,WingedEdge&,const WingedFace&,WingedEdge*)
DELEGATE1 (void,PAModifyWEdge,runUndo         ,WingedMesh&)
DELEGATE1 (void,PAModifyWEdge,runRedo         ,WingedMesh&)

void PAModifyWEdge::setGeometry 
  ( WingedEdge& edge, WingedVertex* v1, WingedVertex* v2
  , WingedFace* lF, WingedFace* rF, WingedEdge* lP, WingedEdge* lS
  , WingedEdge* rP, WingedEdge* rS) {
    this->impl->setGeometry (edge,v1,v2,lF,rF,lP,lS,rP,rS);
}
