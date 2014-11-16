#include <glm/glm.hpp>
#include <set>
#include "../mesh.hpp"
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "config.hpp"
#include "id-map.hpp"
#include "id.hpp"
#include "indexable.hpp"
#include "intersection.hpp"
#include "octree.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "primitive/triangle.hpp"
#include "selection.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct WingedMesh::Impl {
  WingedMesh*                  self;
  const IdObject               id;
  Mesh                         mesh;
  IndexableList <WingedVertex> vertices;
  Edges                        edges;
  Octree                       octree;
  std::set  <unsigned int>     freeVertexIndices;
  IdMap <Edges::iterator>      edgeMap;

  Impl (WingedMesh* s)
    : self   (s)
    {}

  Impl (WingedMesh* s, const Id& i) 
    : self   (s)
    , id     (i)
    {}

  glm::vec3    vector (unsigned int i) const { return this->mesh.vertex (i); }
  unsigned int index  (unsigned int i) const { return this->mesh.index  (i); }
  glm::vec3    normal (unsigned int i) const { return this->mesh.normal (i); }

  WingedVertex* vertex (unsigned int i) const {
    return this->vertices.get (i);
  }

  WingedEdge* edge (const Id& id) const {
    auto it = this->edgeMap.iterator (id);
    return it == this->edgeMap.end () ? nullptr
                                      : &*it->second;
  }

  WingedFace* face (unsigned int index) const { return this->octree.face (index); }

  bool hasFreeVertexIndex () const {
    return ! this->freeVertexIndices.empty ();
  }

  WingedVertex& addVertex (const glm::vec3& pos) {
    WingedVertex&      vertex = this->vertices.emplace ();
    const unsigned int index  = this->mesh.addVertex (pos);
    assert (vertex.index () == index);
    return vertex;
  }

  WingedVertex& addVertex (const glm::vec3& v, unsigned int index) {
    assert (index < this->numVertices ());
    WingedVertex& vertex = this->vertices.emplaceAt (index);
    this->mesh.setVertex (index, v);
    return vertex;
  }

  WingedEdge& addEdge (const Id& id) {
    this->edges.emplace_back (id);

    WingedEdge& edge = this->edges.back ();
    this->edgeMap.insert (edge.id (), --this->edges.end ());
    return edge;
  }

  WingedFace& addFace (const PrimTriangle& geometry) {
    WingedFace& face = this->octree.addFace (geometry);

    if (3 * face.index () == this->mesh.numIndices ()) {
      this->mesh.addIndices (3);
    }
    else if (3 * face.index () > this->mesh.numIndices ()) {
      std::abort ();
    }
    return face;
  }

  void setIndex (unsigned int index, unsigned int vertexIndex) { 
    return this->mesh.setIndex (index, vertexIndex); 
  }

  void setVertex (unsigned int index, const glm::vec3& v) {
    assert (this->freeVertexIndices.count (index) == 0);
    return this->mesh.setVertex (index,v);
  }

  void setNormal (unsigned int index, const glm::vec3& n) {
    assert (this->freeVertexIndices.count (index) == 0);
    return this->mesh.setNormal (index,n);
  }

  void deleteEdge (WingedEdge& edge) { 
    Id   id = edge.id ();
    auto it = this->edgeMap.iterator (id);
    assert (it != this->edgeMap.end ());
    this->edges.erase    (it->second); 
    this->edgeMap.remove (id);
  }

  void deleteFace (WingedFace& face) { 
    this->octree.deleteFace (face); 
  }

  void deleteVertex (WingedVertex& vertex) {
    this->vertices.deleteElement (vertex);
  }

  WingedFace& realignFace ( const WingedFace& face, const PrimTriangle& triangle
                          , bool* sameNode, Octree* newOctree = nullptr ) 
  {
    std::vector <WingedEdge*> adjacents = face.adjacentEdges ().collect ();

    WingedFace& newFace = bool (newOctree) 
                        ? newOctree -> addFace  (face, triangle)
                        : this->octree.realignFace (face, triangle, sameNode);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == &face)
        e->leftFace (&newFace);
      else if (e->rightFace () == &face)
        e->rightFace (&newFace);
      else
        std::abort ();
    }
    return newFace;
  }

  void realignAllFaces () {
    std::vector <WingedFace*> faces;
    this->octree.forEachFace ([&faces] (WingedFace& f) { 
      faces.push_back (&f);
    });

    for (WingedFace* f : faces) {
      this->realignFace (std::move (*f), f->triangle (*this->self), nullptr);
    }
  }

  unsigned int numVertices () const { 
    return this->vertices.numElements (); 
  }

  unsigned int numEdges () const { 
    return this->edges.size (); 
  }

  unsigned int numFaces () const { 
    return this->octree.numFaces ();
  }

  unsigned int numIndices () const { 
    return this->mesh.numIndices (); 
  }

  bool isEmpty () const {
    return this->numVertices () == 0
        && this->numFaces    () == 0
        && this->numIndices  () == 0;
  }

  void writeAllIndices () {
    this->octree.forEachFace ([this] (WingedFace& face) {
      face.writeIndices (*this->self);
    });
  }

  void writeAllNormals () {
    this->vertices.forEachElement ([this] (WingedVertex& v) {
      v.writeNormal (*this->self, v.interpolatedNormal (*this->self));
    });
  }

  void bufferData  () { 
#ifndef NDEBUG
    this->forEachFreeFaceIndex ([this] (unsigned int freeFaceIndex) {
      assert (this->freeVertexIndices.count (this->mesh.index (freeFaceIndex + 0)) == 0);
      assert (this->freeVertexIndices.count (this->mesh.index (freeFaceIndex + 1)) == 0);
      assert (this->freeVertexIndices.count (this->mesh.index (freeFaceIndex + 2)) == 0);
    });
#endif
    this->mesh.bufferData (); 
  }

  void render (const Selection& selection) { 
    if (selection.hasMajor (this->id.id ())) {
      this->mesh.color (Config::get <Color> ("/config/editor/selection/color"));
    }
    else {
      this->mesh.color (Config::get <Color> ("/config/editor/mesh/color/normal"));
    }
    this->mesh.render   (); 
#ifdef DILAY_RENDER_OCTREE
    this->octree.render ();
#endif
  }

  void reset () {
    this->mesh             .reset ();
    this->vertices         .reset ();
    this->edges            .clear ();
    this->octree           .reset ();
  }

  void setupOctreeRoot (const glm::vec3& center, float width) {
    assert (this->isEmpty ());
    this->octree.setupRoot (center,width);
  }

  void toggleRenderMode () { this->mesh.toggleRenderMode (); }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->octree.intersects (*this->self, ray, intersection);
  }

  bool intersects (const PrimSphere& sphere, AffectedFaces& faces) {
    return this->octree.intersects (*this->self, sphere, faces);
  }

  void               scale          (const glm::vec3& v)   { return this->mesh.scale (v); }
  void               scaling        (const glm::vec3& v)   { return this->mesh.scaling (v); }
  glm::vec3          scaling        () const               { return this->mesh.scaling (); }
  void               translate      (const glm::vec3& v)   { return this->mesh.translate (v); }
  void               position       (const glm::vec3& v)   { return this->mesh.position (v); }
  glm::vec3          position       () const               { return this->mesh.position (); }
  void               rotationMatrix (const glm::mat4x4& m) { return this->mesh.rotationMatrix (m); }
  const glm::mat4x4& rotationMatrix () const               { return this->mesh.rotationMatrix (); }
  void               rotationX      (float v)              { return this->mesh.rotationX (v); }
  void               rotationY      (float v)              { return this->mesh.rotationY (v); }
  void               rotationZ      (float v)              { return this->mesh.rotationZ (v); }

  void normalize () {
    glm::mat4x4 model = this->mesh.modelMatrix ();
    glm::vec3   maxVertex (std::numeric_limits <float>::lowest ());
    glm::vec3   minVertex (std::numeric_limits <float>::max    ());

    for (unsigned int i = 0; i < this->numVertices (); ++i) {
      const glm::vec3 v = Util::transformPosition  (model, this->vector (i));
            maxVertex   = glm::max (maxVertex, v);
            minVertex   = glm::min (minVertex, v);

      this->setVertex (i, v);
      this->setNormal (i, Util::transformDirection (model, this->normal (i)));
    }

    Octree newOctree;

    this->octree.forEachFace ([&newOctree,this] (WingedFace& face) { 
      this->realignFace (face, face.triangle (*this->self), nullptr, &newOctree);
    });

    this->octree = std::move (newOctree);

    this->mesh.position       (glm::vec3   (0.0f));
    this->mesh.scaling        (glm::vec3   (1.0f));
    this->mesh.rotationMatrix (glm::mat4x4 (1.0f));
  }

  void forEachVertex (const std::function <void (WingedVertex&)>& f) const {
    this->vertices.forEachElement (f);
  }

  void forEachFreeFaceIndex (const std::function <void (unsigned int)>& f) const {
    this->octree.forEachFreeFaceIndex (f);
  }
};

DELEGATE_BIG3_SELF         (WingedMesh)
DELEGATE1_CONSTRUCTOR_SELF (WingedMesh,const Id&)
ID                         (WingedMesh)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vector, unsigned int)
DELEGATE1_CONST (unsigned int   , WingedMesh, index, unsigned int)
DELEGATE1_CONST (glm::vec3      , WingedMesh, normal, unsigned int)
DELEGATE1_CONST (WingedVertex*  , WingedMesh, vertex, unsigned int)
DELEGATE1_CONST (WingedEdge*    , WingedMesh, edge, const Id&)
DELEGATE1_CONST (WingedFace*    , WingedMesh, face, unsigned int)

DELEGATE1       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&)
DELEGATE2       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, const Id&)
DELEGATE1       (WingedFace&    , WingedMesh, addFace, const PrimTriangle&)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER_CONST    (const Edges&   , WingedMesh, edges)
GETTER_CONST    (const Octree&  , WingedMesh, octree)
GETTER_CONST    (const Mesh&    , WingedMesh, mesh)

DELEGATE1       (void        , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (void        , WingedMesh, deleteFace, WingedFace&)
DELEGATE1       (void        , WingedMesh, deleteVertex, WingedVertex&)
DELEGATE3       (WingedFace& , WingedMesh, realignFace, const WingedFace&, const PrimTriangle&, bool*)
DELEGATE        (void        , WingedMesh, realignAllFaces)
 
DELEGATE_CONST  (unsigned int, WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int, WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int, WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int, WingedMesh, numIndices)
DELEGATE_CONST  (bool        , WingedMesh, isEmpty)

DELEGATE        (void, WingedMesh, writeAllIndices)
DELEGATE        (void, WingedMesh, writeAllNormals)
DELEGATE        (void, WingedMesh, bufferData)
DELEGATE1       (void, WingedMesh, render, const Selection&)
DELEGATE        (void, WingedMesh, reset)
DELEGATE2       (void, WingedMesh, setupOctreeRoot, const glm::vec3&, float)
DELEGATE        (void, WingedMesh, toggleRenderMode)

DELEGATE2       (bool, WingedMesh, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimSphere&, AffectedFaces&)

DELEGATE1       (void              , WingedMesh, scale, const glm::vec3&)
DELEGATE1       (void              , WingedMesh, scaling, const glm::vec3&)
DELEGATE_CONST  (glm::vec3         , WingedMesh, scaling)
DELEGATE1       (void              , WingedMesh, translate, const glm::vec3&)
DELEGATE1       (void              , WingedMesh, position, const glm::vec3&)
DELEGATE_CONST  (glm::vec3         , WingedMesh, position)
DELEGATE1       (void              , WingedMesh, rotationMatrix, const glm::mat4x4&)
DELEGATE_CONST  (const glm::mat4x4&, WingedMesh, rotationMatrix)
DELEGATE1       (void              , WingedMesh, rotationX, float)
DELEGATE1       (void              , WingedMesh, rotationY, float)
DELEGATE1       (void              , WingedMesh, rotationZ, float)
DELEGATE        (void              , WingedMesh, normalize)
DELEGATE1_CONST (void              , WingedMesh, forEachVertex, const std::function <void (WingedVertex&)>&)
DELEGATE1_CONST (void              , WingedMesh, forEachFreeFaceIndex, const std::function <void (unsigned int)>&)
