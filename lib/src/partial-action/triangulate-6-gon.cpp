#include "action/unit/on.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct PATriangulate6Gon :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void run (WingedMesh& mesh, WingedFace& f, std::vector <Id>* affectedFaces) {
    assert (f.numEdges () == 6);
    assert (this->actions.isEmpty ());

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

    WingedVertex& v0 = e01.firstVertexRef (f);
    WingedVertex& v1 = e12.firstVertexRef (f);
    WingedVertex& v2 = e23.firstVertexRef (f);
    WingedVertex& v3 = e34.firstVertexRef (f);
    WingedVertex& v4 = e45.firstVertexRef (f);
    WingedVertex& v5 = e50.firstVertexRef (f);

    WingedFace& a = this->actions.add <PAModifyWMesh> ().addFace
      (mesh, WingedFace (Id (), &e01), PrimTriangle (mesh,v0,v1,v5));
    WingedFace& b = this->actions.add <PAModifyWMesh> ().addFace 
      (mesh, WingedFace (Id (), &e23), PrimTriangle (mesh,v1,v2,v3));
    WingedFace& c = this->actions.add <PAModifyWMesh> ().addFace
      (mesh, WingedFace (Id (), &e45), PrimTriangle (mesh,v3,v4,v5));

    WingedEdge& e13 = this->actions.add <PAModifyWMesh> ().addEdge (mesh, WingedEdge ());
    WingedEdge& e35 = this->actions.add <PAModifyWMesh> ().addEdge (mesh, WingedEdge ());
    WingedEdge& e51 = this->actions.add <PAModifyWMesh> ().addEdge (mesh, WingedEdge ());

    this->actions.add <PAModifyWEdge> ().setGeometry (e13,&v1,&v3,&f,&b,&e51,&e35,&e23,&e12);
    this->actions.add <PAModifyWEdge> ().setGeometry (e35,&v3,&v5,&f,&c,&e13,&e51,&e45,&e34);
    this->actions.add <PAModifyWEdge> ().setGeometry (e51,&v5,&v1,&f,&a,&e35,&e13,&e01,&e50);

    this->actions.add <PAModifyWEdge> ().face        (e01,f,&a);
    this->actions.add <PAModifyWEdge> ().predecessor (e01,a,&e50);
    this->actions.add <PAModifyWEdge> ().successor   (e01,a,&e51);

    this->actions.add <PAModifyWEdge> ().face        (e12,f,&b);
    this->actions.add <PAModifyWEdge> ().predecessor (e12,b,&e13);
    this->actions.add <PAModifyWEdge> ().successor   (e12,b,&e23);

    this->actions.add <PAModifyWEdge> ().face        (e23,f,&b);
    this->actions.add <PAModifyWEdge> ().predecessor (e23,b,&e12);
    this->actions.add <PAModifyWEdge> ().successor   (e23,b,&e13);

    this->actions.add <PAModifyWEdge> ().face        (e34,f,&c);
    this->actions.add <PAModifyWEdge> ().predecessor (e34,c,&e35);
    this->actions.add <PAModifyWEdge> ().successor   (e34,c,&e45);

    this->actions.add <PAModifyWEdge> ().face        (e45,f,&c);
    this->actions.add <PAModifyWEdge> ().predecessor (e45,c,&e34);
    this->actions.add <PAModifyWEdge> ().successor   (e45,c,&e35);

    this->actions.add <PAModifyWEdge> ().face        (e50,f,&a);
    this->actions.add <PAModifyWEdge> ().predecessor (e50,a,&e51);
    this->actions.add <PAModifyWEdge> ().successor   (e50,a,&e01);

    this->actions.add <PAModifyWFace> ().edge (f,&e13);

    this->actions.add <PAModifyWFace> ().writeIndices (mesh,f);
    this->actions.add <PAModifyWFace> ().writeIndices (mesh,a);
    this->actions.add <PAModifyWFace> ().writeIndices (mesh,b);
    this->actions.add <PAModifyWFace> ().writeIndices (mesh,c);

    if (affectedFaces) {
      affectedFaces->push_back (f.id ());
      affectedFaces->push_back (a.id ());
      affectedFaces->push_back (b.id ());
      affectedFaces->push_back (c.id ());
    }
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PATriangulate6Gon)

DELEGATE3 (void,PATriangulate6Gon,run,WingedMesh&,WingedFace&,std::vector<Id>*)
DELEGATE1 (void,PATriangulate6Gon,runUndo,WingedMesh&)
DELEGATE1 (void,PATriangulate6Gon,runRedo,WingedMesh&)
