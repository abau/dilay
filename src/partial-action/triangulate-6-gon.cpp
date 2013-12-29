#include "partial-action/triangulate-6-gon.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "triangle.hpp"
#include "action/realign-face.hpp"

struct PATriangulate6Gon :: Impl {
  ActionUnitOnWMesh actions;

  WingedFace& run (WingedMesh& mesh, WingedFace& f, std::list <Id>* affectedFaces) {
    assert (f.numEdges () == 6);

    this->actions.reset ();

    /*     4
     *    /c\
     *   5---3
     *  /a\f/b\
     * 0---1---2
     */
    WingedEdge& e01  = f.edgeRef ();
    WingedEdge& e12  = f.edgeRef ().successorRef (f,0);
    WingedEdge& e23  = f.edgeRef ().successorRef (f,1);
    WingedEdge& e34  = f.edgeRef ().successorRef (f,2);
    WingedEdge& e45  = f.edgeRef ().successorRef (f,3);
    WingedEdge& e50  = f.edgeRef ().successorRef (f,4);

    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e01, f);
    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e12, f);
    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e23, f);
    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e34, f);
    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e45, f);
    this->actions.add <PAModifyWEdge> ()->increaseFaceGradient (mesh, e50, f);

    WingedVertex& v0 = e01.firstVertexRef (f);
    WingedVertex& v1 = e12.firstVertexRef (f);
    WingedVertex& v2 = e23.firstVertexRef (f);
    WingedVertex& v3 = e34.firstVertexRef (f);
    WingedVertex& v4 = e45.firstVertexRef (f);
    WingedVertex& v5 = e50.firstVertexRef (f);

    WingedFace& a = this->actions.add <PAModifyWMesh> ()->addFace
      (mesh, WingedFace (&e01, Id ()), Triangle (mesh,v0,v1,v5));
    WingedFace& b = this->actions.add <PAModifyWMesh> ()->addFace 
      (mesh, WingedFace (&e23, Id ()), Triangle (mesh,v1,v2,v3));
    WingedFace& c = this->actions.add <PAModifyWMesh> ()->addFace
      (mesh, WingedFace (&e45, Id ()), Triangle (mesh,v3,v4,v5));

    WingedEdge& e13 = this->actions.add <PAModifyWMesh> ()->addEdge (mesh, WingedEdge ());
    WingedEdge& e35 = this->actions.add <PAModifyWMesh> ()->addEdge (mesh, WingedEdge ());
    WingedEdge& e51 = this->actions.add <PAModifyWMesh> ()->addEdge (mesh, WingedEdge ());

    this->actions.add <PAModifyWEdge> ()->setGeometry (mesh,e13,&v1,&v3,&f,&b,&e51,&e35,&e23,&e12);
    this->actions.add <PAModifyWEdge> ()->setGeometry (mesh,e35,&v3,&v5,&f,&c,&e13,&e51,&e45,&e34);
    this->actions.add <PAModifyWEdge> ()->setGeometry (mesh,e51,&v5,&v1,&f,&a,&e35,&e13,&e01,&e50);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e01,f,&a);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e01,a,&e50);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e01,a,&e51);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e12,f,&b);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e12,b,&e13);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e12,b,&e23);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e23,f,&b);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e23,b,&e12);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e23,b,&e13);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e34,f,&c);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e34,c,&e35);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e34,c,&e45);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e45,f,&c);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e45,c,&e34);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e45,c,&e35);

    this->actions.add <PAModifyWEdge> ()->face        (mesh,e50,f,&a);
    this->actions.add <PAModifyWEdge> ()->predecessor (mesh,e50,a,&e51);
    this->actions.add <PAModifyWEdge> ()->successor   (mesh,e50,a,&e01);

    this->actions.add <PAModifyWFace> ()->edge (mesh,f,&e13);

    this->actions.add <PAModifyWFace> ()->write (mesh,f);
    this->actions.add <PAModifyWFace> ()->write (mesh,a);
    this->actions.add <PAModifyWFace> ()->write (mesh,b);
    this->actions.add <PAModifyWFace> ()->write (mesh,c);

    WingedFace& realigned = this->actions.add <ActionRealignFace> ()->run (mesh,f);
    if (affectedFaces) {
      affectedFaces->push_back (realigned.id ());
      affectedFaces->push_back (a.id ());
      affectedFaces->push_back (b.id ());
      affectedFaces->push_back (c.id ());
    }
    return realigned;
  }

  void undo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void redo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_ACTION_BIG5 (PATriangulate6Gon)

DELEGATE3 (WingedFace&,PATriangulate6Gon,run,WingedMesh&,WingedFace&,std::list<Id>*)
DELEGATE1 (void,PATriangulate6Gon,undo,WingedMesh&)
DELEGATE1 (void,PATriangulate6Gon,redo,WingedMesh&)

