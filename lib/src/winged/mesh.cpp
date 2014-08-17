#include <glm/glm.hpp>
#include <set>
#include "winged/vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "../mesh.hpp"
#include "intersection.hpp"
#include "primitive/triangle.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "adjacent-iterator.hpp"
#include "octree.hpp"
#include "id.hpp"
#include "selection.hpp"
#include "config.hpp"
#include "id-map.hpp"
#include "../util.hpp"

struct WingedMesh::Impl {
  typedef std::vector <Vertices::iterator> VertexMap;

  WingedMesh*              self;
  const IdObject           id;
  Mesh                     mesh;
  Vertices                 vertices;
  Edges                    edges;
  Octree                   octree;
  std::set  <unsigned int> freeFaceIndices;
  VertexMap                vertexMap;
  IdMap <Edges::iterator>  edgeMap;

  Impl (WingedMesh* s)
    : self   (s)
    , octree (false)
    {}

  Impl (WingedMesh* s, const Id& i) 
    : self   (s)
    , id     (i)
    , octree (false)
    {}

  glm::vec3    vector (unsigned int i) const { return this->mesh.vertex (i); }
  unsigned int index  (unsigned int i) const { return this->mesh.index  (i); }
  glm::vec3    normal (unsigned int i) const { return this->mesh.normal (i); }

  WingedVertex* vertex (unsigned int i) {
    if (i > this->vertexMap.size ()) {
      return nullptr;
    }
    else {
      return &*this->vertexMap[i];
    }
  }

  WingedVertex& lastVertex () { 
    assert (! this->vertices.empty ());
    return this->vertices.back (); 
  }

  WingedEdge* edge (const Id& id) {
    auto it = this->edgeMap.iterator (id);
    if (it == this->edgeMap.end ()) {
      return nullptr;
    }
    else {
      return &*it->second;
    }
  }

  WingedFace* face (const Id& id) { return this->octree.face (id); }

  unsigned int addIndex (unsigned int index) { 
    return this->mesh.addIndex (index); 
  }

  WingedVertex& addVertex (const glm::vec3& v) {
    unsigned int index = this->mesh.addVertex (v);
    this->vertices .emplace_back (index, nullptr);
    this->vertexMap.emplace_back (--this->vertices.end ());
    return this->vertices.back ();
  }

  WingedEdge& addEdge (const WingedEdge& e) {
    this->edges.emplace_back ( e.vertex1          (), e.vertex2        ()
                             , e.leftFace         (), e.rightFace      ()
                             , e.leftPredecessor  (), e.leftSuccessor  ()
                             , e.rightPredecessor (), e.rightSuccessor ()
                             , e.id (), e.isTEdge (), e.faceGradient   ());
    this->edgeMap.insert (e.id (), --this->edges.end ());
    return this->edges.back ();
  }

  bool hasFreeFaceIndex () const { 
    return this->freeFaceIndices.size () > 0;
  }
  
  WingedFace& addFace (const WingedFace& f, const PrimTriangle& geometry) {
    unsigned int faceIndex;
    if (this->hasFreeFaceIndex ()) {
      faceIndex = *this->freeFaceIndices.begin ();
      this->freeFaceIndices.erase (this->freeFaceIndices.begin ());
    }
    else {
      faceIndex = this->mesh.numIndices ();
      this->mesh.allocateIndices (3);
    }
    return this->octree.insertFace (WingedFace (f.edge (), f.id (), nullptr, faceIndex), geometry);
  }

  void setIndex (unsigned int indexNumber, unsigned int index) { 
    return this->mesh.setIndex (indexNumber, index); 
  }

  void setVertex (unsigned int index, const glm::vec3& v) {
    return this->mesh.setVertex (index,v);
  }

  void setNormal (unsigned int index, const glm::vec3& n) {
    return this->mesh.setNormal (index,n);
  }

  void deleteEdge (const WingedEdge& edge) { 
    Id   id = edge.id ();
    auto it = this->edgeMap.iterator (id);
    assert (it != this->edgeMap.end ());
    this->edges.erase    (it->second); 
    this->edgeMap.remove (id);
  }

  void deleteFace (const WingedFace& face) { 
    if (face.index () == this->mesh.numIndices () - 3) {
      this->mesh.popIndices (3);
    }
    else {
      this->freeFaceIndices.insert (face.index ());
    }
    this->octree.deleteFace (face); 
  }

  void popVertex () {
    this->mesh     .popVertex ();
    this->vertices .pop_back  ();
    this->vertexMap.pop_back  ();
  }

  WingedFace& realignFace ( const WingedFace& face, const PrimTriangle& triangle
                          , bool* sameNode, Octree* newOctree = nullptr ) 
  {
    std::vector <WingedEdge*> adjacents = face.adjacentEdges ().collect ();

    for (WingedEdge* e : adjacents) {
      e->face (face,nullptr);
    }

    WingedFace& newFace = bool (newOctree) 
                        ? newOctree -> insertFace  (face, triangle)
                        : this->octree.realignFace (face, triangle, sameNode);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == nullptr)
        e->leftFace (&newFace);
      else if (e->rightFace () == nullptr)
        e->rightFace (&newFace);
      else
        assert (false);
    }
    return newFace;
  }

  unsigned int numVertices () const { 
    assert (this->mesh.numVertices () == this->vertices.size ());
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

  bool isEmpty () const {
    return this->numVertices () == 0
        && this->numFaces    () == 0
        && this->numIndices  () == 0;
  }

  void writeAllIndices () {
    if (this->hasFreeFaceIndex ()) {
      unsigned int fin = 0;
      this->mesh.resizeIndices (this->numFaces () * 3);

      this->octree.forEachFace ([this,&fin] (WingedFace& face) {
        face.writeIndices (*this->self, &fin);
        fin = fin + 3;
      });
      this->freeFaceIndices.clear ();
    }
    else {
      this->octree.forEachFace ([this] (WingedFace& face) {
        face.writeIndices (*this->self);
      });
    }
  }

  void writeAllInterpolatedNormals () {
    for (WingedVertex& v : this->vertices) {
      v.writeNormal (*this->self, v.interpolatedNormal (*this->self));
    }
  }

  void bufferData  () { 
    assert (this->hasFreeFaceIndex () == false);
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
    this->mesh    .reset ();
    this->vertices.clear ();
    this->edges   .clear ();
    this->octree  .reset ();
  }

  void setupOctreeRoot (const glm::vec3& center, float width) {
    assert (this->isEmpty ());
    this->octree.setupRoot (center,width);
  }

  void toggleRenderMode () { this->mesh.toggleRenderMode (); }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->octree.intersects (*this->self, ray, intersection);
  }

  bool intersects (const PrimSphere& sphere, std::vector <WingedFace*>& faces) {
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

    Octree newOctree (false);

    this->octree.forEachFace ([&newOctree,this] (WingedFace& face) { 
      this->realignFace (face, face.triangle (*this->self), nullptr, &newOctree);
    });

    this->octree = std::move (newOctree);

    this->mesh.position       (glm::vec3 (0.0f));
    this->mesh.scaling        (glm::vec3 (1.0f));
    this->mesh.rotationMatrix (glm::mat4x4 (1.0f));
    this->mesh.bufferData     ();
  }
};

DELEGATE_BIG3_SELF         (WingedMesh)
DELEGATE1_CONSTRUCTOR_SELF (WingedMesh,const Id&)
ID                         (WingedMesh)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vector, unsigned int)
DELEGATE1_CONST (unsigned int   , WingedMesh, index, unsigned int)
DELEGATE1_CONST (glm::vec3      , WingedMesh, normal, unsigned int)
DELEGATE1       (WingedVertex*  , WingedMesh, vertex, unsigned int)
DELEGATE        (WingedVertex&  , WingedMesh, lastVertex)
DELEGATE1       (WingedEdge*    , WingedMesh, edge, const Id&)
DELEGATE1       (WingedFace*    , WingedMesh, face, const Id&)

DELEGATE1       (unsigned int   , WingedMesh, addIndex, unsigned int)
DELEGATE1       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, const WingedEdge&)
DELEGATE2       (WingedFace&    , WingedMesh, addFace, const WingedFace&, const PrimTriangle&)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER_CONST    (const Vertices&, WingedMesh, vertices)
GETTER_CONST    (const Edges&   , WingedMesh, edges)
GETTER_CONST    (const Octree&  , WingedMesh, octree)
GETTER_CONST    (const Mesh&    , WingedMesh, mesh)

DELEGATE1       (void        , WingedMesh, deleteEdge, const WingedEdge&)
DELEGATE1       (void        , WingedMesh, deleteFace, const WingedFace&)
DELEGATE        (void        , WingedMesh, popVertex)
DELEGATE3       (WingedFace& , WingedMesh, realignFace, const WingedFace&, const PrimTriangle&, bool*)
 
DELEGATE_CONST  (unsigned int, WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int, WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int, WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int, WingedMesh, numIndices)
DELEGATE_CONST  (bool        , WingedMesh, isEmpty)

DELEGATE        (void, WingedMesh, writeAllIndices)
DELEGATE        (void, WingedMesh, writeAllInterpolatedNormals)
DELEGATE        (void, WingedMesh, bufferData)
DELEGATE1       (void, WingedMesh, render, const Selection&)
DELEGATE        (void, WingedMesh, reset)
DELEGATE2       (void, WingedMesh, setupOctreeRoot, const glm::vec3&, float)
DELEGATE        (void, WingedMesh, toggleRenderMode)

DELEGATE2       (bool, WingedMesh, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimSphere&, std::vector<WingedFace*>&)

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
