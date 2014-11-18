#include "action/data.hpp"
#include "action/util.hpp"
#include "partial-action/modify-winged-edge.hpp"
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

  struct SetGeometryData {
    Maybe <unsigned int> vertex1;
    Maybe <unsigned int> vertex2;
    Maybe <unsigned int> leftFace;
    Maybe <unsigned int> rightFace;
    Maybe <unsigned int> leftPredecessor;
    Maybe <unsigned int> leftSuccessor;
    Maybe <unsigned int> rightPredecessor;
    Maybe <unsigned int> rightSuccessor;
  };

  typedef ActionData <Maybe <unsigned int>, SetGeometryData> Data;
};

struct PAModifyWEdge :: Impl {
  Operation operation;
  Data      data;

  void vertex1 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex1;

    this->data.index  (edge);
    this->data.values ( ActionUtil::maybeIndex (edge.vertex1 ())
                      , ActionUtil::maybeIndex (v) );
    edge.vertex1      (v);
  }

  void vertex2 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex2;

    this->data.index  (edge);
    this->data.values ( ActionUtil::maybeIndex (edge.vertex2 ())
                      , ActionUtil::maybeIndex (v) );
    edge.vertex2      (v);
  }

  void leftFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::LeftFace;
    this->data.index  (edge);
    this->data.values ( ActionUtil::maybeIndex (edge.leftFace ())
                      , ActionUtil::maybeIndex (f) );
    edge.leftFace     (f);
  }

  void rightFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::RightFace;
    this->data.index  (edge);
    this->data.values ( ActionUtil::maybeIndex (edge.rightFace ())
                      , ActionUtil::maybeIndex (f) );
    edge.rightFace    (f);
  }

  void leftPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftPredecessor;

    this->data.index     (edge);
    this->data.values    ( ActionUtil::maybeIndex (edge.leftPredecessor ())
                         , ActionUtil::maybeIndex (e) );
    edge.leftPredecessor (e);
  }

  void leftSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftSuccessor;

    this->data.index   (edge);
    this->data.values  ( ActionUtil::maybeIndex (edge.leftSuccessor ())
                       , ActionUtil::maybeIndex (e) );
    edge.leftSuccessor (e);
  }

  void rightPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightPredecessor;

    this->data.index      (edge);
    this->data.values     ( ActionUtil::maybeIndex (edge.rightPredecessor ())
                          , ActionUtil::maybeIndex (e) );
    edge.rightPredecessor (e);
  }

  void rightSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightSuccessor;

    this->data.index    (edge);
    this->data.values   ( ActionUtil::maybeIndex (edge.rightSuccessor ())
                        , ActionUtil::maybeIndex (e) );
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
  
  void reset (WingedEdge& edge) {
    this->setGeometry (edge, nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr);
  }

  void setGeometry ( WingedEdge& edge
                   , WingedVertex* v1, WingedVertex* v2
                   , WingedFace* lF, WingedFace* rF
                   , WingedEdge* lP, WingedEdge* lS
                   , WingedEdge* rP, WingedEdge* rS) 
  {
    this->operation = Operation::SetGeometry;
    this->data.index  (edge);
    this->data.values (
        SetGeometryData { 
          ActionUtil::maybeIndex (edge.vertex1          ())
        , ActionUtil::maybeIndex (edge.vertex2          ())
        , ActionUtil::maybeIndex (edge.leftFace         ())
        , ActionUtil::maybeIndex (edge.rightFace        ())
        , ActionUtil::maybeIndex (edge.leftPredecessor  ())
        , ActionUtil::maybeIndex (edge.leftSuccessor    ())
        , ActionUtil::maybeIndex (edge.rightPredecessor ())
        , ActionUtil::maybeIndex (edge.rightSuccessor   ())
        }
      , SetGeometryData { 
          ActionUtil::maybeIndex (v1)
        , ActionUtil::maybeIndex (v2)
        , ActionUtil::maybeIndex (lF)
        , ActionUtil::maybeIndex (rF)
        , ActionUtil::maybeIndex (lP)
        , ActionUtil::maybeIndex (lS)
        , ActionUtil::maybeIndex (rP)
        , ActionUtil::maybeIndex (rS)
        } );
    edge.setGeometry (v1,v2,lF,rF,lP,lS,rP,rS);
  }

  void toggle (WingedMesh& mesh, ActionDataType type) const { 
    WingedEdge& edge = mesh.edgeRef (this->data.index ());

    if (this->operation == Operation::SetGeometry) {
      const SetGeometryData& d = this->data.value <SetGeometryData> (type);

      edge.setGeometry 
        ( ActionUtil::wingedVertex (mesh, d.vertex1)
        , ActionUtil::wingedVertex (mesh, d.vertex2)
        , ActionUtil::wingedFace   (mesh, d.leftFace)
        , ActionUtil::wingedFace   (mesh, d.rightFace)
        , ActionUtil::wingedEdge   (mesh, d.leftPredecessor)
        , ActionUtil::wingedEdge   (mesh, d.leftSuccessor)
        , ActionUtil::wingedEdge   (mesh, d.rightPredecessor)
        , ActionUtil::wingedEdge   (mesh, d.rightSuccessor)
        );
    }
    else {
      const Maybe <unsigned int>& m = this->data.value <Maybe <unsigned int>> (type);

      switch (this->operation) {
        case Operation::Vertex1: {
          edge.vertex1 (ActionUtil::wingedVertex (mesh, m));
          break;
        }
        case Operation::Vertex2: {
          edge.vertex2 (ActionUtil::wingedVertex (mesh, m));
          break;
        }
        case Operation::LeftFace: {
          edge.leftFace (ActionUtil::wingedFace (mesh, m));
          break;
        }
        case Operation::RightFace: {
          edge.rightFace (ActionUtil::wingedFace (mesh, m));
          break;
        }
        case Operation::LeftPredecessor: {
          edge.leftPredecessor (ActionUtil::wingedEdge (mesh, m));
          break;
        }
        case Operation::LeftSuccessor: {
          edge.leftSuccessor (ActionUtil::wingedEdge (mesh, m));
          break;
        }
        case Operation::RightPredecessor: {
          edge.rightPredecessor (ActionUtil::wingedEdge (mesh, m));
          break;
        }
        case Operation::RightSuccessor: {
          edge.rightSuccessor (ActionUtil::wingedEdge (mesh, m));
          break;
        }
        case Operation::SetGeometry: {
          break;
        }
      }
    }
  }

  void runUndo (WingedMesh& mesh) const { this->toggle (mesh, ActionDataType::Old); }
  void runRedo (WingedMesh& mesh) const { this->toggle (mesh, ActionDataType::New); }
};

DELEGATE_BIG3 (PAModifyWEdge)

DELEGATE2       (void, PAModifyWEdge, vertex1         , WingedEdge&, WingedVertex*)
DELEGATE2       (void, PAModifyWEdge, vertex2         , WingedEdge&, WingedVertex*)
DELEGATE2       (void, PAModifyWEdge, leftFace        , WingedEdge&, WingedFace*)
DELEGATE2       (void, PAModifyWEdge, rightFace       , WingedEdge&, WingedFace*)
DELEGATE2       (void, PAModifyWEdge, leftPredecessor , WingedEdge&, WingedEdge*)
DELEGATE2       (void, PAModifyWEdge, leftSuccessor   , WingedEdge&, WingedEdge*)
DELEGATE2       (void, PAModifyWEdge, rightPredecessor, WingedEdge&, WingedEdge*)
DELEGATE2       (void, PAModifyWEdge, rightSuccessor  , WingedEdge&, WingedEdge*)
DELEGATE3       (void, PAModifyWEdge, firstVertex     , WingedEdge&, const WingedFace&, WingedVertex*)
DELEGATE3       (void, PAModifyWEdge, secondVertex    , WingedEdge&, const WingedFace&, WingedVertex*)
DELEGATE3       (void, PAModifyWEdge, vertex          , WingedEdge&, const WingedVertex&, WingedVertex*)
DELEGATE3       (void, PAModifyWEdge, face            , WingedEdge&, const WingedFace&, WingedFace*)
DELEGATE3       (void, PAModifyWEdge, predecessor     , WingedEdge&, const WingedFace&, WingedEdge*)
DELEGATE3       (void, PAModifyWEdge, successor       , WingedEdge&, const WingedFace&, WingedEdge*)
DELEGATE1       (void, PAModifyWEdge, reset           , WingedEdge&)
DELEGATE1_CONST (void, PAModifyWEdge, runUndo         , WingedMesh&)
DELEGATE1_CONST (void, PAModifyWEdge, runRedo         , WingedMesh&)

void PAModifyWEdge::setGeometry 
  ( WingedEdge& edge, WingedVertex* v1, WingedVertex* v2
  , WingedFace* lF, WingedFace* rF, WingedEdge* lP, WingedEdge* lS
  , WingedEdge* rP, WingedEdge* rS) {
    this->impl->setGeometry (edge,v1,v2,lF,rF,lP,lS,rP,rS);
}
