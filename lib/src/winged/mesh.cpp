#include <glm/glm.hpp>
#include <set>
#include "../mesh.hpp"
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "config.hpp"
#include "id-map.hpp"
#include "id.hpp"
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
  typedef std::vector <Vertices::iterator> VertexMap;

  WingedMesh*              self;
  const IdObject           id;
  Mesh                     mesh;
  Vertices                 vertices;
  Edges                    edges;
  Octree                   octree;
  std::set  <unsigned int> freeFaceIndices;
  std::set  <unsigned int> freeVertexIndices;
  VertexMap                vertexMap;
  IdMap <Edges::iterator>  edgeMap;

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

  WingedVertex* vertex (unsigned int i) {
    assert (this->freeVertexIndices.count (i) == 0);
    return i > this->vertexMap.size () ? nullptr
                                       : &*this->vertexMap[i];
  }

  WingedEdge* edge (const Id& id) {
    auto it = this->edgeMap.iterator (id);
    return it == this->edgeMap.end () ? nullptr
                                      : &*it->second;
  }

  WingedFace* face (const Id& id) { return this->octree.face (id); }

  bool hasFreeVertexIndex () const {
    return ! this->freeVertexIndices.empty ();
  }

  WingedVertex& addVertex (const glm::vec3& v) {
    return this->addVertex (v, nullptr);
  }

  WingedVertex& addVertex (const glm::vec3& v, unsigned int index) {
    return this->addVertex (v, &index);
  }

  WingedVertex& addVertex (const glm::vec3& v, const unsigned int* index) {
    if (this->hasFreeVertexIndex ()) {
      assert (bool (index) == false || this->freeVertexIndices.count (*index) > 0);
      
      const unsigned int reusedIndex = bool (index) ? *index
                                                    : *this->freeVertexIndices.begin ();

      this->vertices.emplace_back (reusedIndex, nullptr);
      this->vertexMap[reusedIndex] = --this->vertices.end ();
      this->freeVertexIndices.erase (reusedIndex);
    }
    else {
      const unsigned int newIndex = this->mesh.addVertex (v);
      assert (bool (index) == false || newIndex == *index);
      this->vertices .emplace_back (newIndex, nullptr);
      this->vertexMap.emplace_back (--this->vertices.end ());
    }
    return this->vertices.back ();
  }

  WingedEdge& addEdge (WingedEdge&& e) {
    this->edges.push_back (std::move (e));
    this->edgeMap.insert (e.id (), --this->edges.end ());
    return this->edges.back ();
  }

  bool hasFreeFaceIndex () const { 
    return ! this->freeFaceIndices.empty ();
  }
  
  WingedFace& addFace (WingedFace&& face, const PrimTriangle& geometry) {
    unsigned int faceIndex;
    if (this->hasFreeFaceIndex ()) {
      faceIndex = *this->freeFaceIndices.begin ();
    }
    else {
      faceIndex = this->mesh.numIndices ();
      this->mesh.allocateIndices   (this->mesh.numIndices () + 3);
      this->freeFaceIndices.insert (faceIndex);
    }
    return this->addFace (std::move (face), geometry, faceIndex);
  }

  WingedFace& addFace (WingedFace&& face, const PrimTriangle& geometry, unsigned int faceIndex) {
    assert (this->freeFaceIndices.count (faceIndex) > 0);

    this->freeFaceIndices.erase (faceIndex);
    face.index                  (faceIndex);
    return this->octree.insertFace (std::move (face), geometry);
  }

  void setIndex (unsigned int indexNumber, unsigned int index) { 
    return this->mesh.setIndex (indexNumber, index); 
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
    this->freeFaceIndices.insert (face.index ());
    this->octree.deleteFace      (face); 
  }

  void deleteVertex (WingedVertex& vertex) {
    this->freeVertexIndices.insert (vertex.index ());
    this->vertices.erase (this->vertexMap [vertex.index ()]);
  }

  WingedFace& realignFace ( WingedFace&& face, const PrimTriangle& triangle
                          , bool* sameNode, Octree* newOctree = nullptr ) 
  {
    std::vector <WingedEdge*> adjacents = face.adjacentEdges ().collect ();
    WingedFace*               oldFace   = &face;

    WingedFace& newFace = bool (newOctree) 
                        ? newOctree -> insertFace  (std::move (face), triangle)
                        : this->octree.realignFace (std::move (face), triangle, sameNode);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == oldFace)
        e->leftFace (&newFace);
      else if (e->rightFace () == oldFace)
        e->rightFace (&newFace);
      else
        assert (false);
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
    assert (this->mesh.numVertices () == this->vertices.size () + this->freeVertexIndices.size ());
    return this->vertices.size (); 
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

  unsigned int numFreeFaceIndices () const { 
    return this->freeFaceIndices.size (); 
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
    for (WingedVertex& v : this->vertices) {
      v.writeNormal (*this->self, v.interpolatedNormal (*this->self));
    }
  }

  void bufferData  () { 
    if (this->numFaces () > 0) {
      if (this->freeFaceIndices.empty () == false) {
        unsigned int idx = this->octree.someFaceRef ().index ();
        for (unsigned int i : this->freeFaceIndices) {
          this->mesh.setIndex (i + 0, this->mesh.index (idx + 0));
          this->mesh.setIndex (i + 1, this->mesh.index (idx + 1));
          this->mesh.setIndex (i + 2, this->mesh.index (idx + 2));
        }
      }
      this->mesh.bufferData (); 
    }
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
    this->vertices         .clear ();
    this->edges            .clear ();
    this->octree           .reset ();
    this->freeFaceIndices  .clear ();
    this->freeVertexIndices.clear ();
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
      this->realignFace (std::move (face), face.triangle (*this->self), nullptr, &newOctree);
    });

    this->octree = std::move (newOctree);

    this->mesh.position       (glm::vec3   (0.0f));
    this->mesh.scaling        (glm::vec3   (1.0f));
    this->mesh.rotationMatrix (glm::mat4x4 (1.0f));
  }
};

DELEGATE_BIG3_SELF         (WingedMesh)
DELEGATE1_CONSTRUCTOR_SELF (WingedMesh,const Id&)
ID                         (WingedMesh)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vector, unsigned int)
DELEGATE1_CONST (unsigned int   , WingedMesh, index, unsigned int)
DELEGATE1_CONST (glm::vec3      , WingedMesh, normal, unsigned int)
DELEGATE1       (WingedVertex*  , WingedMesh, vertex, unsigned int)
DELEGATE1       (WingedEdge*    , WingedMesh, edge, const Id&)
DELEGATE1       (WingedFace*    , WingedMesh, face, const Id&)

DELEGATE1       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&)
DELEGATE2       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, WingedEdge&&)
DELEGATE2       (WingedFace&    , WingedMesh, addFace, WingedFace&&, const PrimTriangle&)
DELEGATE3       (WingedFace&    , WingedMesh, addFace, WingedFace&&, const PrimTriangle&, unsigned int)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER_CONST    (const Vertices&, WingedMesh, vertices)
GETTER_CONST    (const Edges&   , WingedMesh, edges)
GETTER_CONST    (const Octree&  , WingedMesh, octree)
GETTER_CONST    (const Mesh&    , WingedMesh, mesh)

DELEGATE1       (void        , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (void        , WingedMesh, deleteFace, WingedFace&)
DELEGATE1       (void        , WingedMesh, deleteVertex, WingedVertex&)
DELEGATE3       (WingedFace& , WingedMesh, realignFace, WingedFace&&, const PrimTriangle&, bool*)
DELEGATE        (void        , WingedMesh, realignAllFaces)
 
DELEGATE_CONST  (unsigned int, WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int, WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int, WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int, WingedMesh, numIndices)
DELEGATE_CONST  (unsigned int, WingedMesh, numFreeFaceIndices)
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
