#include <glm/glm.hpp>
#include <map>
#include "../mesh.hpp"
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "indexable.hpp"
#include "intersection.hpp"
#include "octree.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "primitive/triangle.hpp"
#include "render-mode.hpp"
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

  WingedFace& addFace (const PrimTriangle& geometry) {
    WingedFace& face = this->octree.addFace (geometry);

    if ((3 * face.index ()) + 2 >= this->mesh.numIndices ()) {
      this->mesh.resizeIndices ((3 * face.index ()) + 3);
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
                          , Octree* newOctree = nullptr ) 
  {
    std::vector <WingedEdge*> adjacents = face.adjacentEdges ().collect ();

    WingedFace& newFace = bool (newOctree) 
                        ? newOctree -> addFace  (face.index (), face.edge (), triangle)
                        : this->octree.realignFace (face, triangle);

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
      this->realignFace (*f, f->triangle (*this->self));
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

  void fromMesh (const Mesh& mesh) {
    typedef std::pair <unsigned int,unsigned int> UIPair;
    typedef std::map  <UIPair, WingedEdge*>       EdgeMap;

    /** `findOrAddEdge (m,i1,i2,f)` searches an edge with indices `(i2,i1)` 
     * (in that order) in `m`.
     * If such an edge exists, `f` becomes its new right face.
     * Otherwise a new edge is added to `this` and `m` with `f` being its left face.
     * The found (resp. created) edge is returned
     */
    auto findOrAddEdge = [this] ( EdgeMap& map
                                , unsigned int index1, unsigned int index2
                                , WingedFace& face) -> WingedEdge&
    {
      auto result = map.find (UIPair (index2, index1));
      if (result == map.end ()) {
        WingedVertex* v1    = this->vertex (index1);
        WingedVertex* v2    = this->vertex (index2);
        WingedEdge& newEdge = this->addEdge ();
          
        map.insert (std::pair <UIPair,WingedEdge*> ( UIPair (index1,index2)
                                                   , &newEdge ));
        newEdge.vertex1  (v1);
        newEdge.vertex2  (v2);
        newEdge.leftFace (&face);

        v1-> edge (&newEdge);
        v2-> edge (&newEdge);
        face.edge (&newEdge);

        return newEdge;
      }
      else {
        WingedEdge* existingEdge = result->second;

        existingEdge->rightFace (&face);
        face.edge (existingEdge);

        return *existingEdge;
      }
    };

    // mesh
    this->reset ();
    this->mesh = mesh;

    // octree
    glm::vec3 minVertex, maxVertex;
    this->mesh.minMax (minVertex, maxVertex);

    const glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
    const glm::vec3 delta  =  maxVertex - minVertex;
    const float     width  = glm::max (glm::max (delta.x, delta.y), delta.z);

    this->setupOctreeRoot (center, width);

    // vertices
    this->vertices.reserveIndices (this->mesh.numVertices ());
    for (unsigned int i = 0; i < this->mesh.numVertices (); i++) {
      this->vertices.emplace ();
    }

    // faces & edges
    EdgeMap edgeMap;

    assert (this->mesh.numIndices () % 3 == 0);

    for (unsigned int i = 0; i < this->mesh.numIndices (); i += 3) {
      unsigned int index1 = this->mesh.index (i + 0);
      unsigned int index2 = this->mesh.index (i + 1);
      unsigned int index3 = this->mesh.index (i + 2);

      WingedFace& f = this->addFace (PrimTriangle (*this->self
                                                  , this->self->vertexRef (index1)
                                                  , this->self->vertexRef (index2)
                                                  , this->self->vertexRef (index3) ));
      WingedEdge& e1 = findOrAddEdge (edgeMap, index1, index2, f);
      WingedEdge& e2 = findOrAddEdge (edgeMap, index2, index3, f);
      WingedEdge& e3 = findOrAddEdge (edgeMap, index3, index1, f);

      e1.predecessor (f, &e3);
      e1.successor   (f, &e2);
      e2.predecessor (f, &e1);
      e2.successor   (f, &e3);
      e3.predecessor (f, &e2);
      e3.successor   (f, &e1);
    }
    this->writeAllNormals ();
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
    auto resetFreeFaceIndices = [this] () {
      if (this->numFaces () > 0) {
        unsigned int nonFreeFaceIndex = this->octree.someFaceRef ().index ();
        this->octree.forEachFreeFaceIndex ([&] (unsigned int index) {
          this->setIndex ((3 * index) + 0, this->index ((3 * nonFreeFaceIndex) + 0));
          this->setIndex ((3 * index) + 1, this->index ((3 * nonFreeFaceIndex) + 1));
          this->setIndex ((3 * index) + 2, this->index ((3 * nonFreeFaceIndex) + 2));
        });
      }
    };

    resetFreeFaceIndices  ();
    this->mesh.bufferData (); 
  }

  void render (Camera& camera) const { 
    this->mesh.render   (camera); 
#ifdef DILAY_RENDER_OCTREE
    this->octree.render (camera);
#endif
  }

  void reset () {
    this->mesh    .reset ();
    this->vertices.reset ();
    this->edges   .reset ();
    this->octree  .reset ();
  }

  void setupOctreeRoot (const glm::vec3& center, float width) {
    assert (this->octree.hasRoot () == false);
    this->octree.setupRoot (center,width);
  }

  const RenderMode& renderMode () const { return this->mesh.renderMode (); }
  RenderMode&       renderMode ()       { return this->mesh.renderMode (); }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->octree.intersects (*this->self, ray, intersection);
  }

  bool intersects (const PrimSphere& sphere, AffectedFaces& faces) {
    OctreeIntersection intersection
      ( [&sphere] (const PrimAABox& box) {
          return IntersectionUtil::intersects (sphere, box);
        }
      , [this,&sphere] (const WingedFace& face) {
          return IntersectionUtil::intersects (sphere, *this->self, face);
        }
      );
    return this->octree.intersects (intersection, faces);
  }

  bool intersects (const PrimPlane& plane, AffectedFaces& faces) {
    OctreeIntersection intersection
      ( [&plane] (const PrimAABox& box) {
          return IntersectionUtil::intersects (plane, box);
        }
      , [this,&plane] (const WingedFace& face) {
          return IntersectionUtil::intersects (plane, *this->self, face);
        }
      );
    return this->octree.intersects (intersection, faces);
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
    const glm::mat4x4 model       = this->mesh.modelMatrix ();
    const glm::mat3x3 modelNormal = this->mesh.modelNormalMatrix ();

    glm::vec3   maxVertex (std::numeric_limits <float>::lowest ());
    glm::vec3   minVertex (std::numeric_limits <float>::max    ());

    for (unsigned int i = 0; i < this->numVertices (); ++i) {
      const glm::vec3 v = Util::transformPosition  (model, this->vector (i));
            maxVertex   = glm::max (maxVertex, v);
            minVertex   = glm::min (minVertex, v);

      this->setVertex (i, v);
      this->setNormal (i, glm::normalize (modelNormal * this->normal (i)));
    }

    Octree newOctree;

    this->octree.forEachFace ([&newOctree,this] (WingedFace& face) { 
      this->realignFace (face, face.triangle (*this->self), &newOctree);
    });

    this->octree = std::move (newOctree);

    this->mesh.position       (glm::vec3   (0.0f));
    this->mesh.scaling        (glm::vec3   (1.0f));
    this->mesh.rotationMatrix (glm::mat4x4 (1.0f));
  }

  glm::vec3 center () const {
    return this->mesh.center ();
  }

  const Color& color () const {
    return this->mesh.color ();
  }

  void color (const Color& c) {
    this->mesh.color (c);
  }

  const Color& wireframeColor () const {
    return this->mesh.wireframeColor ();
  }

  void wireframeColor (const Color& c) {
    this->mesh.wireframeColor (c);
  }

  void forEachVertex (const std::function <void (WingedVertex&)>& f) {
    this->vertices.forEachElement (f);
  }

  void forEachConstVertex (const std::function <void (const WingedVertex&)>& f) const {
    this->vertices.forEachConstElement (f);
  }

  void forEachEdge (const std::function <void (WingedEdge&)>& f) {
    this->edges.forEachElement (f);
  }

  void forEachConstEdge (const std::function <void (const WingedEdge&)>& f) const {
    this->edges.forEachConstElement (f);
  }

  void forEachFace (const std::function <void (WingedFace&)>& f) {
    this->octree.forEachFace (f);
  }

  void forEachConstFace (const std::function <void (const WingedFace&)>& f) const {
    this->octree.forEachConstFace (f);
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
DELEGATE        (WingedEdge&    , WingedMesh, addEdge)
DELEGATE1       (WingedFace&    , WingedMesh, addFace, const PrimTriangle&)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER_CONST    (const Octree&  , WingedMesh, octree)
GETTER_CONST    (const Mesh&    , WingedMesh, mesh)

DELEGATE1       (void        , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (void        , WingedMesh, deleteFace, WingedFace&)
DELEGATE1       (void        , WingedMesh, deleteVertex, WingedVertex&)
DELEGATE2       (WingedFace& , WingedMesh, realignFace, WingedFace&, const PrimTriangle&)
DELEGATE        (void        , WingedMesh, realignAllFaces)
 
DELEGATE_CONST  (unsigned int, WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int, WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int, WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int, WingedMesh, numIndices)
DELEGATE_CONST  (bool        , WingedMesh, isEmpty)

DELEGATE1       (void             , WingedMesh, fromMesh, const Mesh&)
DELEGATE        (void             , WingedMesh, writeAllIndices)
DELEGATE        (void             , WingedMesh, writeAllNormals)
DELEGATE        (void             , WingedMesh, bufferData)
DELEGATE1_CONST (void             , WingedMesh, render, Camera&)
DELEGATE        (void             , WingedMesh, reset)
DELEGATE2       (void             , WingedMesh, setupOctreeRoot, const glm::vec3&, float)
DELEGATE_CONST  (const RenderMode&, WingedMesh, renderMode)
DELEGATE        (RenderMode&      , WingedMesh, renderMode)

DELEGATE2       (bool, WingedMesh, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimSphere&, AffectedFaces&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimPlane&, AffectedFaces&)

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
DELEGATE_CONST  (glm::vec3         , WingedMesh, center)
DELEGATE_CONST  (const Color&      , WingedMesh, color)
DELEGATE1       (void              , WingedMesh, color, const Color&)
DELEGATE_CONST  (const Color&      , WingedMesh, wireframeColor)
DELEGATE1       (void              , WingedMesh, wireframeColor, const Color&)
DELEGATE1       (void              , WingedMesh, forEachVertex, const std::function <void (WingedVertex&)>&)
DELEGATE1_CONST (void              , WingedMesh, forEachConstVertex, const std::function <void (const WingedVertex&)>&)
DELEGATE1       (void              , WingedMesh, forEachEdge, const std::function <void (WingedEdge&)>&)
DELEGATE1_CONST (void              , WingedMesh, forEachConstEdge  , const std::function <void (const WingedEdge&)>&)
DELEGATE1       (void              , WingedMesh, forEachFace, const std::function <void (WingedFace&)>&)
DELEGATE1_CONST (void              , WingedMesh, forEachConstFace, const std::function <void (const WingedFace&)>&)
