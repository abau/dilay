#include <memory>
#include <vector>
#include "action/ids.hpp"
#include "id.hpp"
#include "scene.hpp"
#include "sphere/mesh.hpp"
#include "state.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionIds :: Impl {
  std::vector <std::unique_ptr <Id>>           ids;
  std::vector <std::unique_ptr <unsigned int>> indices;

  unsigned int numIds () const {
    return this->ids.size ();
  }

  unsigned int numIndices () const {
    return this->indices.size ();
  }

  void reserveIds     (unsigned int s) { 
    if (s > this->numIds ())
      this->ids.resize (s); 
  }
  void reserveIndices (unsigned int s) { 
    if (s > this->numIndices ())
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

  WingedMesh& getWingedMesh (unsigned int i) {
    assert (this->ids [i]);
    return State::scene ().wingedMesh (*this->ids [i]);
  }

  SphereMesh& getSphereMesh (unsigned int i) {
    assert (this->ids [i]);
    return State::scene ().sphereMesh (*this->ids [i]);
  }

  SphereMeshNode* getSphereMeshNode (SphereMesh& mesh, unsigned int i) {
    return this->ids [i] ? &mesh.node (*this->ids [i]) : nullptr;
  }

  WingedFace* getFace (WingedMesh& mesh, unsigned int i) {
    return this->ids [i] ? mesh.face (*this->ids [i]) : nullptr;
  }

  WingedEdge* getEdge (WingedMesh& mesh, unsigned int i) {
    return this->ids [i] ? mesh.edge (*this->ids [i]) : nullptr;
  }

  WingedVertex* getVertex (WingedMesh& mesh, unsigned int i) {
    return this->indices [i] ? mesh.vertex (*this->indices [i]) : nullptr;
  }

  void setMesh (unsigned int i, const WingedMesh& mesh) {
    this->setId (i, mesh.id ());
  }

  void setMesh (unsigned int i, const SphereMesh& mesh) {
    this->setId (i, mesh.id ());
  }

  void setNode (unsigned int i, const SphereMeshNode* node) {
    if (node) this->setId   (i, node->id ());
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

template <> 
WingedMesh& ActionIds::getMesh <WingedMesh> (unsigned int i) { return this->getWingedMesh (i); }

template <> 
SphereMesh& ActionIds::getMesh <SphereMesh> (unsigned int i) { return this->getSphereMesh (i); }

DELEGATE_BIG3 (ActionIds)

DELEGATE_CONST (unsigned int   , ActionIds, numIds)
DELEGATE_CONST (unsigned int   , ActionIds, numIndices)
DELEGATE2      (void           , ActionIds, setId,            unsigned int, const Id&)
DELEGATE2      (void           , ActionIds, setIds,           std::initializer_list <Id>, unsigned int)
DELEGATE2      (void           , ActionIds, setIndex,         unsigned int, unsigned int)
DELEGATE2      (void           , ActionIds, setIndices,       std::initializer_list <unsigned int>, unsigned int)
DELEGATE1      (Id*            , ActionIds, getId,            unsigned int)
DELEGATE1      (unsigned int*  , ActionIds, getIndex,         unsigned int)
DELEGATE1      (void           , ActionIds, resetId,          unsigned int)
DELEGATE1      (void           , ActionIds, resetIndex,       unsigned int)
DELEGATE1      (WingedMesh&    , ActionIds, getWingedMesh,    unsigned int)
DELEGATE1      (SphereMesh&    , ActionIds, getSphereMesh,    unsigned int)
DELEGATE2      (SphereMeshNode*, ActionIds, getSphereMeshNode,SphereMesh&, unsigned int)
DELEGATE2      (WingedFace*    , ActionIds, getFace,          WingedMesh&, unsigned int)
DELEGATE2      (WingedEdge*    , ActionIds, getEdge,          WingedMesh&, unsigned int)
DELEGATE2      (WingedVertex*  , ActionIds, getVertex,        WingedMesh&, unsigned int)
DELEGATE2      (void           , ActionIds, setMesh,          unsigned int, const WingedMesh&)
DELEGATE2      (void           , ActionIds, setMesh,          unsigned int, const SphereMesh&)
DELEGATE2      (void           , ActionIds, setNode,          unsigned int, const SphereMeshNode*)
DELEGATE2      (void           , ActionIds, setFace,          unsigned int, const WingedFace*)
DELEGATE2      (void           , ActionIds, setEdge,          unsigned int, const WingedEdge*)
DELEGATE2      (void           , ActionIds, setVertex,        unsigned int, const WingedVertex*)
