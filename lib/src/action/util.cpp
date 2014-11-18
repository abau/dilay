#include "action/util.hpp"
#include "maybe.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

Maybe <unsigned int> ActionUtil :: maybeIndex (const WingedEdge* e) {
  return e ? Maybe <unsigned int> (e->index ()) : Maybe <unsigned int> ();
}

Maybe <unsigned int> ActionUtil :: maybeIndex (const WingedFace* f) {
  return f ? Maybe <unsigned int> (f->index ()) : Maybe <unsigned int> ();
}

Maybe <unsigned int> ActionUtil :: maybeIndex (const WingedVertex* v) {
  return v ? Maybe <unsigned int> (v->index ()) : Maybe <unsigned int> ();
}

WingedEdge* ActionUtil :: wingedEdge (const WingedMesh& mesh, const Maybe <unsigned int>& m) {
  return m.isSet () ? mesh.edge (m.getRef ()) : nullptr;
}

WingedFace* ActionUtil :: wingedFace (const WingedMesh& mesh, const Maybe <unsigned int>& m) {
  return m.isSet () ? mesh.face (m.getRef ()) : nullptr;
}

WingedVertex* ActionUtil :: wingedVertex (const WingedMesh& mesh, const Maybe <unsigned int>& m) {
  return m.isSet () ? mesh.vertex (m.getRef ()) : nullptr;
}
