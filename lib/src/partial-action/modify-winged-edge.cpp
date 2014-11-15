#include "action/data.hpp"
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
    ActionIdentifier vertex1;
    ActionIdentifier vertex2;
    ActionIdentifier leftFace;
    ActionIdentifier rightFace;
    ActionIdentifier leftPredecessor;
    ActionIdentifier leftSuccessor;
    ActionIdentifier rightPredecessor;
    ActionIdentifier rightSuccessor;
  };

  typedef ActionData <ActionIdentifier, SetGeometryData> Data;
};

struct PAModifyWEdge :: Impl {
  Operation operation;
  Data      data;

  void vertex1 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex1;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.vertex1 (), v);
    edge.vertex1                (v);
  }

  void vertex2 (WingedEdge& edge, WingedVertex* v) {
    this->operation = Operation::Vertex2;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.vertex2 (), v);
    edge.vertex2                (v);
  }

  void leftFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::LeftFace;
    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.leftFace (), f);
    edge.leftFace               (f);
  }

  void rightFace (WingedEdge& edge, WingedFace* f) {
    this->operation = Operation::RightFace;
    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.rightFace (), f);
    edge.rightFace              (f);
  }

  void leftPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftPredecessor;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.leftPredecessor (), e);
    edge.leftPredecessor        (e);
  }

  void leftSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::LeftSuccessor;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.leftSuccessor (), e);
    edge.leftSuccessor          (e);
  }

  void rightPredecessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightPredecessor;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.rightPredecessor (), e);
    edge.rightPredecessor       (e);
  }

  void rightSuccessor (WingedEdge& edge, WingedEdge* e) {
    this->operation = Operation::RightSuccessor;

    this->data.identifier       (edge);
    this->data.valueIdentifiers (edge.rightSuccessor (), e);
    edge.rightSuccessor         (e);
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
                   , WingedEdge* rP, WingedEdge* rS) {
    this->operation = Operation::SetGeometry;
    this->data.identifier (edge);
    this->data.values     ( SetGeometryData { edge.vertex1 (), edge.vertex2 ()
                                            , edge.leftFace (), edge.rightFace ()
                                            , edge.leftSuccessor (), edge.leftPredecessor ()
                                            , edge.rightSuccessor (), edge.rightPredecessor () }
                          , SetGeometryData { v1, v2, lF, rF, lP, lS, rP, rS } );

    edge.setGeometry (v1,v2,lF,rF,lP,lS,rP,rS);
  }

  void toggle (WingedMesh& mesh, ActionDataType type) const { 
    WingedEdge& edge = this->data.identifier ().getEdgeRef (mesh);

    switch (this->operation) {
      case Operation::Vertex1: {
        edge.vertex1 (this->data.valueIdentifier (type).getVertex (mesh));
        break;
      }
      case Operation::Vertex2: {
        edge.vertex2 (this->data.valueIdentifier (type).getVertex (mesh));
        break;
      }
      case Operation::LeftFace: {
        edge.leftFace (this->data.valueIdentifier (type).getFace (mesh));
        break;
      }
      case Operation::RightFace: {
        edge.rightFace (this->data.valueIdentifier (type).getFace (mesh));
        break;
      }
      case Operation::LeftPredecessor: {
        edge.leftPredecessor (this->data.valueIdentifier (type).getEdge (mesh));
        break;
      }
      case Operation::LeftSuccessor: {
        edge.leftSuccessor (this->data.valueIdentifier (type).getEdge (mesh));
        break;
      }
      case Operation::RightPredecessor: {
        edge.rightPredecessor (this->data.valueIdentifier (type).getEdge (mesh));
        break;
      }
      case Operation::RightSuccessor: {
        edge.rightSuccessor (this->data.valueIdentifier (type).getEdge (mesh));
        break;
      }
      case Operation::SetGeometry: {
        const SetGeometryData& d = this->data.value <SetGeometryData> (type);

        edge.setGeometry 
          ( d.vertex1.getVertex        (mesh),  d.vertex2.getVertex      (mesh)
          , d.leftFace.getFace         (mesh),  d.rightFace.getFace      (mesh)
          , d.leftPredecessor.getEdge  (mesh),  d.leftSuccessor.getEdge  (mesh)
          , d.rightPredecessor.getEdge (mesh),  d.rightSuccessor.getEdge (mesh) );
        break;
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
