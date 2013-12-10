#include "partial-action/triangulate-6-gon.hpp"
#include "macro.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit.hpp"
#include "partial-action/modify-mesh.hpp"
#include "partial-action/modify-edge.hpp"
#include "partial-action/modify-face.hpp"
#include "triangle.hpp"
#include "action/realign-face.hpp"

struct PATriangulate6Gon :: Impl {
  ActionUnit actions;

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

    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e01, f);
    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e12, f);
    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e23, f);
    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e34, f);
    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e45, f);
    this->actions.add <PAModifyEdge> ()->faceGradient (mesh, e50, f);

    WingedVertex& v0 = e01.firstVertexRef (f);
    WingedVertex& v1 = e12.firstVertexRef (f);
    WingedVertex& v2 = e23.firstVertexRef (f);
    WingedVertex& v3 = e34.firstVertexRef (f);
    WingedVertex& v4 = e45.firstVertexRef (f);
    WingedVertex& v5 = e50.firstVertexRef (f);

    WingedFace& a = this->actions.add <PAModifyMesh> ()->addFace
      (mesh, WingedFace (&e01), Triangle (mesh,v0,v1,v5));
    WingedFace& b = this->actions.add <PAModifyMesh> ()->addFace 
      (mesh, WingedFace (&e23), Triangle (mesh,v1,v2,v3));
    WingedFace& c = this->actions.add <PAModifyMesh> ()->addFace
      (mesh, WingedFace (&e45), Triangle (mesh,v3,v4,v5));

    WingedEdge& e13 = this->actions.add <PAModifyMesh> ()->addEdge (mesh, WingedEdge ());
    WingedEdge& e35 = this->actions.add <PAModifyMesh> ()->addEdge (mesh, WingedEdge ());
    WingedEdge& e51 = this->actions.add <PAModifyMesh> ()->addEdge (mesh, WingedEdge ());

    this->actions.add <PAModifyEdge> ()->setGeometry (mesh,e13,&v1,&v3,&f,&b,&e51,&e35,&e23,&e12);
    this->actions.add <PAModifyEdge> ()->setGeometry (mesh,e35,&v3,&v5,&f,&c,&e13,&e51,&e45,&e34);
    this->actions.add <PAModifyEdge> ()->setGeometry (mesh,e51,&v5,&v1,&f,&a,&e35,&e13,&e01,&e50);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e01,f,&a);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e01,a,&e50);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e01,a,&e51);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e12,f,&b);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e12,b,&e13);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e12,b,&e23);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e23,f,&b);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e23,b,&e12);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e23,b,&e13);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e34,f,&c);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e34,c,&e35);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e34,c,&e45);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e45,f,&c);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e45,c,&e34);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e45,c,&e35);

    this->actions.add <PAModifyEdge> ()->face        (mesh,e50,f,&a);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh,e50,a,&e51);
    this->actions.add <PAModifyEdge> ()->successor   (mesh,e50,a,&e01);

    this->actions.add <PAModifyFace> ()->edge (mesh,f,&e13);

    this->actions.add <PAModifyFace> ()->write (mesh,f);
    this->actions.add <PAModifyFace> ()->write (mesh,a);
    this->actions.add <PAModifyFace> ()->write (mesh,b);
    this->actions.add <PAModifyFace> ()->write (mesh,c);

    WingedFace& realigned = this->actions.add <ActionRealignFace> ()->run (mesh,f);
    if (affectedFaces) {
      affectedFaces->push_back (realigned.id ());
      affectedFaces->push_back (a.id ());
      affectedFaces->push_back (b.id ());
      affectedFaces->push_back (c.id ());
    }
    return realigned;
  }

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_CONSTRUCTOR (PATriangulate6Gon)
DELEGATE_DESTRUCTOR  (PATriangulate6Gon)

DELEGATE3 (WingedFace&,PATriangulate6Gon,run,WingedMesh&,WingedFace&,std::list<Id>*)
DELEGATE  (void,PATriangulate6Gon,undo)
DELEGATE  (void,PATriangulate6Gon,redo)

