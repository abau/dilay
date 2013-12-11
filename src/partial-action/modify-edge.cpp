#include <memory>
#include "partial-action/modify-edge.hpp"
#include "macro.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "state.hpp"
#include "partial-action/ids.hpp"

enum class Operation 
  { Vertex1, Vertex2, LeftFace, RightFace
  , LeftPredecessor, LeftSuccessor, RightPredecessor, RightSuccessor
  , PreviousSibling, NextSibling
  , FirstVertex, SecondVertex
  , Predecessor, Successor
  , SetGeometry, IsTEdge, FaceGradient
  };

struct PAModifyEdge :: Impl {
  Operation                      operation;
  PAIds                          operands; 
  std::unique_ptr <bool>         flag;
  std::unique_ptr <FaceGradient> fGradient;

  void vertex1 (WingedMesh& mesh, WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex1;
    this->operands.setIds    ({mesh.id (), edge.id ()});
    this->operands.setVertex (0, edge.vertex1 ());
    edge.vertex1 (v);
  }

  void vertex2 (WingedMesh& mesh, WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex2;
    this->operands.setIds    ({mesh.id (), edge.id ()});
    this->operands.setVertex (0, edge.vertex2 ());
    edge.vertex2 (v);
  }

  void leftFace (WingedMesh& mesh, WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::LeftFace;
    this->operands.setIds  ({mesh.id (), edge.id ()});
    this->operands.setFace (2, edge.leftFace ());
    edge.leftFace (f);
  }

  void rightFace (WingedMesh& mesh, WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::RightFace;
    this->operands.setIds  ({mesh.id (), edge.id ()});
    this->operands.setFace (2, edge.rightFace ());
    edge.rightFace (f);
  }

  void leftPredecessor (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftPredecessor;
    this->operands.setIds  ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.leftPredecessor ());
    edge.leftPredecessor (e);
  }

  void leftSuccessor (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftSuccessor;
    this->operands.setIds  ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.leftSuccessor ());
    edge.leftSuccessor (e);
  }

  void rightPredecessor (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightPredecessor;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.rightPredecessor ());
    edge.rightPredecessor (e);
  }

  void rightSuccessor (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightSuccessor;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.rightSuccessor ());
    edge.rightSuccessor (e);
  }

  void previousSibling (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::PreviousSibling;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.previousSibling ());
    edge.previousSibling (e);
  }

  void nextSibling (WingedMesh& mesh, WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::NextSibling;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->operands.setEdge (2, edge.nextSibling ());
    edge.nextSibling   (e);
  }

  void firstVertex (WingedMesh& mesh, WingedEdge& edge, const WingedFace& f, WingedVertex* v) {
    this->operation = Operation::FirstVertex;
    this->operands.setIds    ({mesh.id (), edge.id (), f.id ()});
    this->operands.setVertex (0, edge.firstVertex (f));
    edge.firstVertex (f,v);
  }

  void secondVertex (WingedMesh& mesh, WingedEdge& edge, const WingedFace& f, WingedVertex* v) {
    this->operation = Operation::SecondVertex;
    this->operands.setIds    ({mesh.id (), edge.id (), f.id ()});
    this->operands.setVertex (0, edge.secondVertex (f));
    edge.secondVertex (f,v);
  }

  void face (WingedMesh& mesh, WingedEdge& edge, const WingedFace& fOld, WingedFace* fNew) {
    if (edge.isLeftFace (fOld))
      this->leftFace (mesh,edge,fNew);
    else if (edge.isRightFace (fOld))
      this->rightFace (mesh,edge,fNew);
    else
      assert (false);
  }

  void predecessor (WingedMesh& mesh, WingedEdge& edge, const WingedFace& f, WingedEdge* e) {
    this->operation = Operation::Predecessor;
    this->operands.setIds ({mesh.id (), edge.id (), f.id ()});
    this->operands.setEdge (3, edge.predecessor (f));
    edge.predecessor (f,e);
  }

  void successor (WingedMesh& mesh, WingedEdge& edge, const WingedFace& f, WingedEdge* e) {
    this->operation = Operation::Successor;
    this->operands.setIds ({mesh.id (), edge.id (), f.id ()});
    this->operands.setEdge (3, edge.successor (f));
    edge.successor (f,e);
  }
  
  void setGeometry ( WingedMesh& mesh, WingedEdge& edge
                   , WingedVertex* v1, WingedVertex* v2
                   , WingedFace* lF, WingedFace* rF
                   , WingedEdge* lP, WingedEdge* lS
                   , WingedEdge* rP, WingedEdge* rS
                   , WingedEdge* p, WingedEdge* n) {
    this->operation = Operation::SetGeometry;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->operands.setVertex (2 , edge.vertex1          ());
    this->operands.setVertex (3 , edge.vertex2          ());
    this->operands.setFace   (4 , edge.leftFace         ());
    this->operands.setFace   (5 , edge.rightFace        ());
    this->operands.setEdge   (6 , edge.leftPredecessor  ());
    this->operands.setEdge   (7 , edge.leftSuccessor    ());
    this->operands.setEdge   (8 , edge.rightPredecessor ());
    this->operands.setEdge   (9 , edge.rightSuccessor   ());
    this->operands.setEdge   (10, edge.previousSibling  ());
    this->operands.setEdge   (11, edge.nextSibling      ());
    edge.setGeometry (v1,v2,lF,rF,lP,lS,rP,rS,p,n);
  }

  void isTEdge (WingedMesh& mesh, WingedEdge& edge, bool value) {
    this->operation = Operation::IsTEdge;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->flag.reset      (new bool (edge.isTEdge ()));
    edge.isTEdge (value);
  }

  void faceGradient (WingedMesh& mesh, WingedEdge& edge, FaceGradient g) {
    this->operation = Operation::FaceGradient;
    this->operands.setIds ({mesh.id (), edge.id ()});
    this->fGradient.reset (new FaceGradient (edge.faceGradient ())); 
    edge.faceGradient (g);
  }

  void faceGradient (WingedMesh& mesh, WingedEdge& edge, const WingedFace& face) {
    if (edge.isLeftFace (face)) {
      switch (edge.faceGradient ()) {
        case FaceGradient::Left:
          assert (false);
        case FaceGradient::None:
          return this->faceGradient (mesh, edge, FaceGradient::Left);
        case FaceGradient::Right:
          return this->faceGradient (mesh, edge, FaceGradient::None);
      };
    }
    else {
      switch (edge.faceGradient ()) {
        case FaceGradient::Left:
          return this->faceGradient (mesh, edge, FaceGradient::None);
        case FaceGradient::None:
          return this->faceGradient (mesh, edge, FaceGradient::Right);
        case FaceGradient::Right:
          assert (false);
      };
    }
  }

  void toggle () { 
    WingedMesh& mesh =  this->operands.getMesh (0);
    WingedEdge& edge = *this->operands.getEdge (mesh,1);

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
        edge.leftFace (this->operands.getFace (mesh,2));
        this->operands.setFace (2,f);
        break;
      }
      case Operation::RightFace: {
        WingedFace* f = edge.rightFace ();
        edge.rightFace (this->operands.getFace (mesh,2));
        this->operands.setFace (2,f);
        break;
      }
      case Operation::LeftPredecessor: {
        WingedEdge* e = edge.leftPredecessor ();
        edge.leftPredecessor (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::LeftSuccessor: {
        WingedEdge* e = edge.leftSuccessor ();
        edge.leftSuccessor (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::RightPredecessor: {
        WingedEdge* e = edge.rightPredecessor ();
        edge.rightPredecessor (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::RightSuccessor: {
        WingedEdge* e = edge.rightSuccessor ();
        edge.rightSuccessor (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::PreviousSibling: {
        WingedEdge* e = edge.previousSibling ();
        edge.previousSibling (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::NextSibling: {
        WingedEdge* e = edge.nextSibling ();
        edge.nextSibling (this->operands.getEdge (mesh,2));
        this->operands.setEdge (2,e);
        break;
      }
      case Operation::FirstVertex: {
        WingedFace*   f = this->operands.getFace (mesh,2);
        WingedVertex* v = edge.firstVertex (*f);
        edge.firstVertex (*f, this->operands.getVertex (mesh,0));
        this->operands.setVertex (0,v);
        break;
      }
      case Operation::SecondVertex: {
        WingedFace*   f = this->operands.getFace (mesh,2);
        WingedVertex* v = edge.secondVertex (*f);
        edge.secondVertex (*f, this->operands.getVertex (mesh,0));
        this->operands.setVertex (0,v);
        break;
      }
      case Operation::Predecessor: {
        WingedFace* f = this->operands.getFace (mesh,2);
        WingedEdge* e = edge.predecessor (*f);
        edge.predecessor (*f, this->operands.getEdge (mesh,3));
        this->operands.setEdge (3,e);
        break;
      }
      case Operation::Successor: {
        WingedFace* f = this->operands.getFace (mesh,2);
        WingedEdge* e = edge.successor (*f);
        edge.successor (*f, this->operands.getEdge (mesh,3));
        this->operands.setEdge (3,e);
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
        WingedEdge*   p  = edge.previousSibling  ();
        WingedEdge*   n  = edge.nextSibling      ();

        edge.setGeometry 
          ( this->operands.getVertex (mesh,2),  this->operands.getVertex (mesh,3)
          , this->operands.getFace   (mesh,4),  this->operands.getFace   (mesh,5)
          , this->operands.getEdge   (mesh,6),  this->operands.getEdge   (mesh,7)
          , this->operands.getEdge   (mesh,8),  this->operands.getEdge   (mesh,9)
          , this->operands.getEdge   (mesh,10), this->operands.getEdge   (mesh,11));

        this->operands.setVertex (2 , v1);
        this->operands.setVertex (3 , v2);
        this->operands.setFace   (4 , lF);
        this->operands.setFace   (5 , rF);
        this->operands.setEdge   (6 , lP);
        this->operands.setEdge   (7 , lS);
        this->operands.setEdge   (8 , rP);
        this->operands.setEdge   (9 , rS);
        this->operands.setEdge   (10, p );
        this->operands.setEdge   (11, n );
        break;
      }
      case Operation::IsTEdge: {
        bool isTEdge = edge.isTEdge ();
        edge.isTEdge     (*this->flag);
        this->flag.reset (new bool (isTEdge));
        break;
      }
      case Operation::FaceGradient: {
        FaceGradient gradient = edge.faceGradient ();
        edge.faceGradient     (*this->fGradient);
        this->fGradient.reset (new FaceGradient (gradient));
        break;
      }
      default: assert (false);
    }
  }

  void undo () { this->toggle (); }
  void redo () { this->toggle (); }
};

DELEGATE_CONSTRUCTOR (PAModifyEdge)
DELEGATE_DESTRUCTOR  (PAModifyEdge)

DELEGATE3 (void,PAModifyEdge,vertex1         ,WingedMesh&,WingedEdge&,WingedVertex*)
DELEGATE3 (void,PAModifyEdge,vertex2         ,WingedMesh&,WingedEdge&,WingedVertex*)
DELEGATE3 (void,PAModifyEdge,leftFace        ,WingedMesh&,WingedEdge&,WingedFace*)
DELEGATE3 (void,PAModifyEdge,rightFace       ,WingedMesh&,WingedEdge&,WingedFace*)
DELEGATE3 (void,PAModifyEdge,leftPredecessor ,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,leftSuccessor   ,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,rightPredecessor,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,rightSuccessor  ,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,previousSibling ,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,nextSibling     ,WingedMesh&,WingedEdge&,WingedEdge*)
DELEGATE4 (void,PAModifyEdge,firstVertex     ,WingedMesh&,WingedEdge&,const WingedFace&,WingedVertex*)
DELEGATE4 (void,PAModifyEdge,secondVertex    ,WingedMesh&,WingedEdge&,const WingedFace&,WingedVertex*)
DELEGATE4 (void,PAModifyEdge,face            ,WingedMesh&,WingedEdge&,const WingedFace&,WingedFace*)
DELEGATE4 (void,PAModifyEdge,predecessor     ,WingedMesh&,WingedEdge&,const WingedFace&,WingedEdge*)
DELEGATE4 (void,PAModifyEdge,successor       ,WingedMesh&,WingedEdge&,const WingedFace&,WingedEdge*)
DELEGATE3 (void,PAModifyEdge,isTEdge         ,WingedMesh&,WingedEdge&,bool)
DELEGATE3 (void,PAModifyEdge,faceGradient    ,WingedMesh&,WingedEdge&,FaceGradient)
DELEGATE3 (void,PAModifyEdge,faceGradient    ,WingedMesh&,WingedEdge&,const WingedFace&)
DELEGATE  (void,PAModifyEdge,undo)
DELEGATE  (void,PAModifyEdge,redo)

void PAModifyEdge::setGeometry 
  ( WingedMesh& mesh, WingedEdge& edge, WingedVertex* v1, WingedVertex* v2
  , WingedFace* lF, WingedFace* rF, WingedEdge* lP, WingedEdge* lS
  , WingedEdge* rP, WingedEdge* rS, WingedEdge* p, WingedEdge* n) {
    this->impl->setGeometry (mesh,edge,v1,v2,lF,rF,lP,lS,rP,rS,p,n);
}
