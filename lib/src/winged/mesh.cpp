#include <glm/glm.hpp>
#include <set>
#include "../mesh.hpp"
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "config.hpp"
#include "color.hpp"
#include "indexable.hpp"
#include "intersection.hpp"
#include "octree.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct WingedMesh::Impl {
  WingedMesh*                  self;
  const unsigned int           _index;
  Mesh                         mesh;
  IndexableList <WingedVertex> vertices;
  IndexableList <WingedEdge>   edges;
  Octree                       octree;
  Color                        colorNormal; 
  Color                        colorSelection; 

  Impl (WingedMesh* s, unsigned int i) 
    : self   (s)
    , _index (i)
  {}

  unsigned int index  ()               const { return this->_index;          }
  glm::vec3    vector (unsigned int i) const { return this->mesh.vertex (i); }
  unsigned int index  (unsigned int i) const { return this->mesh.index  (i); }
  glm::vec3    normal (unsigned int i) const { return this->mesh.normal (i); }

  WingedVertex* vertex (unsigned int i) const {
    return this->vertices.get (i);
  }

  WingedEdge* edge (unsigned int i) const {
    return this->edges.get (i);
  }

  WingedFace* face (unsigned int index) const { return this->octree.face (index); }

  WingedVertex& addVertex (const glm::vec3& pos) {
    WingedVertex& vertex = this->vertices.emplace ();
    this->addVertexToInternalMesh (vertex, pos);
    return vertex;
  }

  WingedVertex& addVertex (unsigned int index, const glm::vec3& pos) {
    WingedVertex& vertex = this->vertices.emplaceAt (index);
    this->addVertexToInternalMesh (vertex, pos);
    return vertex;
  }

  void addVertexToInternalMesh (WingedVertex& vertex, const glm::vec3& pos) {
    if (vertex.index () == this->mesh.numVertices ()) {
      this->mesh.addVertex (pos);
    }
    else {
      if (vertex.index () >= this->mesh.numVertices ()) {
        this->mesh.resizeVertices (vertex.index () + 1);
      }
      this->mesh.setVertex (vertex.index (), pos);
    }
  }

  WingedEdge& addEdge () {
    return this->edges.emplace ();
  }

  WingedEdge& addEdge (unsigned int index) {
    return this->edges.emplaceAt (index);
  }

  WingedFace& addFace (const PrimTriangle& geometry) {
    WingedFace& face = this->octree.addFace (geometry);

    if ((3 * face.index ()) + 2 >= this->mesh.numIndices ()) {
      this->mesh.resizeIndices ((3 * face.index ()) + 3);
    }
    return face;
  }

  WingedFace& addFace (unsigned int index, const PrimTriangle& geometry) {
    WingedFace& face = this->octree.addFace (index, nullptr, geometry);

    if ((3 * face.index ()) + 2 >= this->mesh.numIndices ()) {
      this->mesh.resizeIndices ((3 * index) + 3);
    }
    return face;
  }

  void setIndex (unsigned int index, unsigned int vertexIndex) { 
    return this->mesh.setIndex (index, vertexIndex); 
  }

  void setVertex (unsigned int index, const glm::vec3& v) {
    assert (this->vertices.isFree (index) == false);
    return this->mesh.setVertex (index,v);
  }

  void setNormal (unsigned int index, const glm::vec3& n) {
    assert (this->vertices.isFree (index) == false);
    return this->mesh.setNormal (index,n);
  }

  void deleteEdge (WingedEdge& edge) { 
    this->edges.deleteElement (edge);
  }

  void deleteFace (WingedFace& face) { 
    this->octree.deleteFace (face); 
  }

  void deleteVertex (WingedVertex& vertex) {
    this->vertices.deleteElement (vertex);
  }

  WingedFace& realignFace ( WingedFace& face, const PrimTriangle& triangle
                          , bool* sameNode, Octree* newOctree = nullptr ) 
  {
    std::vector <WingedEdge*> adjacents = face.adjacentEdges ().collect ();

    WingedFace& newFace = bool (newOctree) 
                        ? newOctree -> addFace  (face.index (), face.edge (), triangle)
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
      this->realignFace (*f, f->triangle (*this->self), nullptr);
    }
  }

  unsigned int numVertices () const {
    return this->vertices.numElements (); 
  }

  unsigned int numEdges () const { 
    return this->edges.numElements (); 
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
    this->octree.forEachFreeFaceIndex ([this] (unsigned int freeFaceIndex) {
      assert (this->vertices.isFree (this->mesh.index (freeFaceIndex + 0)) == false);
      assert (this->vertices.isFree (this->mesh.index (freeFaceIndex + 1)) == false);
      assert (this->vertices.isFree (this->mesh.index (freeFaceIndex + 2)) == false);
    });
#endif
    this->mesh.bufferData (); 
  }

  void render (const Camera& camera, bool isSelected) { 
    if (isSelected) {
      this->mesh.color (this->colorSelection);
    }
    else {
      this->mesh.color (this->colorNormal);
    }
    this->mesh.render   (camera); 
#ifdef DILAY_RENDER_OCTREE
    this->octree.render (camera);
#endif
  }

  void reset () {
    this->mesh             .reset ();
    this->vertices         .reset ();
    this->edges            .reset ();
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

  void forEachEdge (const std::function <void (WingedEdge&)>& f) const {
    this->edges.forEachElement (f);
  }

  void runFromConfig (const ConfigProxy& config) {
    this->colorNormal    = config.get <Color> ("color/normal");
    this->colorSelection = config.get <Color> ("color/selection");
    this->mesh.wireframeColor (config.get <Color> ("color/wireframe"));
  }
};

DELEGATE1_BIG3_SELF (WingedMesh, unsigned int)

DELEGATE_CONST  (unsigned int   , WingedMesh, index)
DELEGATE1_CONST (glm::vec3      , WingedMesh, vector, unsigned int)
DELEGATE1_CONST (unsigned int   , WingedMesh, index, unsigned int)
DELEGATE1_CONST (glm::vec3      , WingedMesh, normal, unsigned int)
DELEGATE1_CONST (WingedVertex*  , WingedMesh, vertex, unsigned int)
DELEGATE1_CONST (WingedEdge*    , WingedMesh, edge, unsigned int)
DELEGATE1_CONST (WingedFace*    , WingedMesh, face, unsigned int)

DELEGATE1       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&)
DELEGATE2       (WingedVertex&  , WingedMesh, addVertex, unsigned int, const glm::vec3&)
DELEGATE        (WingedEdge&    , WingedMesh, addEdge)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, unsigned int)
DELEGATE1       (WingedFace&    , WingedMesh, addFace, const PrimTriangle&)
DELEGATE2       (WingedFace&    , WingedMesh, addFace, unsigned int, const PrimTriangle&)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER_CONST    (const Octree&  , WingedMesh, octree)
GETTER_CONST    (const Mesh&    , WingedMesh, mesh)

DELEGATE1       (void        , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (void        , WingedMesh, deleteFace, WingedFace&)
DELEGATE1       (void        , WingedMesh, deleteVertex, WingedVertex&)
DELEGATE3       (WingedFace& , WingedMesh, realignFace, WingedFace&, const PrimTriangle&, bool*)
DELEGATE        (void        , WingedMesh, realignAllFaces)
 
DELEGATE_CONST  (unsigned int, WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int, WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int, WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int, WingedMesh, numIndices)
DELEGATE_CONST  (bool        , WingedMesh, isEmpty)

DELEGATE        (void, WingedMesh, writeAllIndices)
DELEGATE        (void, WingedMesh, writeAllNormals)
DELEGATE        (void, WingedMesh, bufferData)
DELEGATE2       (void, WingedMesh, render, const Camera&, bool)
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
DELEGATE1_CONST (void              , WingedMesh, forEachEdge  , const std::function <void (WingedEdge&)>&)
DELEGATE1       (void              , WingedMesh, runFromConfig, const ConfigProxy&);
