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

  bool hasId (unsigned int i) const {
    return i < this->numIds () && this->ids [i] != nullptr;
  }

  bool hasIndex (unsigned int i) const {
    return i < this->numIndices () && this->indices [i] != nullptr;
  }

  void reserveIds (unsigned int s) { 
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

  Id* getId (unsigned int i) const { 
    return this->ids.size () > i ? this->ids [i].get ()
                                 : nullptr;
  }

  unsigned int* getIndex (unsigned int i) const {
    return this->indices.size () > i ? this->indices [i].get ()
                                     : nullptr;
  }

  WingedMesh& getWingedMesh (unsigned int i) const {
    return State::scene ().wingedMesh (*this->getId (i));
  }

  SphereMesh& getSphereMesh (unsigned int i) const {
    return State::scene ().sphereMesh (*this->getId (i));
  }

  SphereMeshNode* getSphereMeshNode (SphereMesh& mesh, unsigned int i) const {
    return this->hasId (i) ? &mesh.node (*this->ids [i]) : nullptr;
  }

  WingedFace* getFace (WingedMesh& mesh, unsigned int i) const {
    return this->hasId (i) ? mesh.face (*this->ids [i]) : nullptr;
  }

  WingedEdge* getEdge (WingedMesh& mesh, unsigned int i) const {
    return this->hasId (i) ? mesh.edge (*this->ids [i]) : nullptr;
  }

  WingedVertex* getVertex (WingedMesh& mesh, unsigned int i) const {
    return this->hasIndex (i) ? mesh.vertex (*this->indices [i]) : nullptr;
  }

  void setMesh (unsigned int i, const WingedMesh& mesh) {
    this->setId (i, mesh.id ());
  }

  void setMesh (unsigned int i, const SphereMesh& mesh) {
    this->setId (i, mesh.id ());
  }

  void setNode (unsigned int i, const SphereMeshNode* node) {
    if (node) {
      this->setId (i, node->id ());
    }
  }

  void setFace (unsigned int i, const WingedFace* face) {
    if (face) {
      this->setId (i, face->id ());
    }
  }

  void setEdge (unsigned int i, const WingedEdge* edge) {
    if (edge) {
      this->setId (i, edge->id ());
    }
  }

  void setVertex (unsigned int i, const WingedVertex* vertex) {
    if (vertex) {
      this->setIndex (i, vertex->index ());
    }
  }
};

template <> 
WingedMesh& ActionIds::getMesh <WingedMesh> (unsigned int i) const { 
  return this->getWingedMesh (i); 
}

template <> 
SphereMesh& ActionIds::getMesh <SphereMesh> (unsigned int i) const { 
  return this->getSphereMesh (i); 
}

DELEGATE_BIG3 (ActionIds)

DELEGATE_CONST  (unsigned int   , ActionIds, numIds)
DELEGATE_CONST  (unsigned int   , ActionIds, numIndices)
DELEGATE2       (void           , ActionIds, setId,            unsigned int, const Id&)
DELEGATE2       (void           , ActionIds, setIds,           std::initializer_list <Id>, unsigned int)
DELEGATE2       (void           , ActionIds, setIndex,         unsigned int, unsigned int)
DELEGATE2       (void           , ActionIds, setIndices,       std::initializer_list <unsigned int>, unsigned int)
DELEGATE1_CONST (Id*            , ActionIds, getId,            unsigned int)
DELEGATE1_CONST (unsigned int*  , ActionIds, getIndex,         unsigned int)
DELEGATE1_CONST (WingedMesh&    , ActionIds, getWingedMesh,    unsigned int)
DELEGATE1_CONST (SphereMesh&    , ActionIds, getSphereMesh,    unsigned int)
DELEGATE2_CONST (SphereMeshNode*, ActionIds, getSphereMeshNode,SphereMesh&, unsigned int)
DELEGATE2_CONST (WingedFace*    , ActionIds, getFace,          WingedMesh&, unsigned int)
DELEGATE2_CONST (WingedEdge*    , ActionIds, getEdge,          WingedMesh&, unsigned int)
DELEGATE2_CONST (WingedVertex*  , ActionIds, getVertex,        WingedMesh&, unsigned int)
DELEGATE2       (void           , ActionIds, setMesh,          unsigned int, const WingedMesh&)
DELEGATE2       (void           , ActionIds, setMesh,          unsigned int, const SphereMesh&)
DELEGATE2       (void           , ActionIds, setNode,          unsigned int, const SphereMeshNode*)
DELEGATE2       (void           , ActionIds, setFace,          unsigned int, const WingedFace*)
DELEGATE2       (void           , ActionIds, setEdge,          unsigned int, const WingedEdge*)
DELEGATE2       (void           , ActionIds, setVertex,        unsigned int, const WingedVertex*)
