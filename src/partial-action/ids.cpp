#include <memory>
#include <vector>
#include "partial-action/ids.hpp"
#include "macro.hpp"
#include "id.hpp"
#include "state.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"

struct PAIds :: Impl {
  std::vector <std::unique_ptr <Id>>           ids;
  std::vector <std::unique_ptr <unsigned int>> indices;

  void reserveIds     (unsigned int s) { 
    if (s > this->ids.size ())
      this->ids.resize (s); 
  }
  void reserveIndices (unsigned int s) { 
    if (s > this->indices.size ())
      this->indices.resize (s); 
  }

  void reserve (unsigned int sIds, unsigned int sIndices) {
    this->reserveIds     (sIds);
    this->reserveIndices (sIndices);
  }

  void setId (unsigned int i, const Id& id) {
    this->reserveIds (i + 1);
    this->ids [i].reset (new Id (id));
  }

  void setIds (std::initializer_list <Id> ids, unsigned int start) {
    this->reserveIds (start + ids.size ());
    unsigned int i = start;
    for (const Id& id : ids) {
      this->setId (i, id);
      i++;
    }
  }

  void setIndex (unsigned int i, unsigned int index) {
    this->reserveIndices (i + 1);
    this->indices [i].reset (new unsigned int (index));
  }

  void setIndices (std::initializer_list <unsigned int> indices, unsigned int start) {
    this->reserveIndices (start + indices.size ());
    unsigned int i = start;
    for (unsigned int index : indices) {
      this->setIndex (i, index);
      i++;
    }
  }

  Id*           getId    (unsigned int i) { return this->ids     [i].get (); }
  unsigned int* getIndex (unsigned int i) { return this->indices [i].get (); }

  void resetId (unsigned int i) {
    this->reserveIds (i + 1);
    this->ids [i].reset (nullptr);
  }

  void resetIndex (unsigned int i) {
    this->reserveIndices (i + 1);
    this->indices [i].reset (nullptr);
  }

  WingedMesh& getMesh (unsigned int i) {
    assert (this->ids [i]);
    return State::mesh (*this->ids [i]);
  }

  WingedFace* getFace (WingedMesh& mesh, unsigned int i) {
    return this->ids [i] ? &mesh.face (*this->ids [i]) : nullptr;
  }

  WingedEdge* getEdge (WingedMesh& mesh, unsigned int i) {
    return this->ids [i] ? &mesh.edgeSLOW (*this->ids [i]) : nullptr;
  }

  WingedVertex* getVertex (WingedMesh& mesh, unsigned int i) {
    return this->indices [i] ? &mesh.vertexSLOW (*this->indices [i]) : nullptr;
  }

  void setMesh (unsigned int i, const WingedMesh* mesh) {
    if (mesh) this->setId   (i, mesh->id ());
    else      this->resetId (i);
  }

  void setFace (unsigned int i, const WingedFace* face) {
    if (face) this->setId   (i, face->id ());
    else      this->resetId (i);
  }

  void setEdge (unsigned int i, const WingedEdge* edge) {
    if (edge) this->setId (i, edge->id ());
    else      this->resetId (i);
  }

  void setVertex (unsigned int i, const WingedVertex* vertex) {
    if (vertex) this->setIndex   (i, vertex->index ());
    else        this->resetIndex (i);
  }
};

DELEGATE_CONSTRUCTOR (PAIds)
DELEGATE_DESTRUCTOR  (PAIds)

DELEGATE1 (void         , PAIds, reserveIds,     unsigned int)
DELEGATE1 (void         , PAIds, reserveIndices, unsigned int)
DELEGATE2 (void         , PAIds, reserve,        unsigned int, unsigned int)
DELEGATE2 (void         , PAIds, setId,          unsigned int, const Id&)
DELEGATE2 (void         , PAIds, setIds,         std::initializer_list <Id>, unsigned int)
DELEGATE2 (void         , PAIds, setIndex,       unsigned int, unsigned int)
DELEGATE2 (void         , PAIds, setIndices,     std::initializer_list <unsigned int>, unsigned int)
DELEGATE1 (Id*          , PAIds, getId,          unsigned int)
DELEGATE1 (unsigned int*, PAIds, getIndex,       unsigned int)
DELEGATE1 (void         , PAIds, resetId,        unsigned int)
DELEGATE1 (void         , PAIds, resetIndex,     unsigned int)
DELEGATE1 (WingedMesh&  , PAIds, getMesh,        unsigned int)
DELEGATE2 (WingedFace*  , PAIds, getFace,        WingedMesh&, unsigned int)
DELEGATE2 (WingedEdge*  , PAIds, getEdge,        WingedMesh&, unsigned int)
DELEGATE2 (WingedVertex*, PAIds, getVertex,      WingedMesh&, unsigned int)
DELEGATE2 (void         , PAIds, setMesh,        unsigned int, const WingedMesh*)
DELEGATE2 (void         , PAIds, setFace,        unsigned int, const WingedFace*)
DELEGATE2 (void         , PAIds, setEdge,        unsigned int, const WingedEdge*)
DELEGATE2 (void         , PAIds, setVertex,      unsigned int, const WingedVertex*)
