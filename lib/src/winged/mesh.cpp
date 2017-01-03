/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "../mesh.hpp"
#include "../util.hpp"
#include "action/finalize.hpp"
#include "affected-faces.hpp"
#include "config.hpp"
#include "edge-map.hpp"
#include "hash.hpp"
#include "index-octree.hpp"
#include "intersection.hpp"
#include "mesh-util.hpp"
#include "monotone-deque.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct WingedMesh::Impl {
  WingedMesh*                  self;
  const unsigned int          _index;
  Mesh                         mesh;
  MonotoneDeque <WingedVertex> vertices;
  MonotoneDeque <WingedEdge>   edges;
  MonotoneDeque <WingedFace>   faces;
  IndexOctree                  octree;

  Impl (WingedMesh* s, unsigned int i) 
    :  self  (s)
    , _index (i)
  {}

  Impl (WingedMesh* s, const Impl& other, bool restorePointer)
    :  self     (s)
    , _index    (other._index)
    ,  mesh     (other.mesh)
    ,  vertices (other.vertices)
    ,  edges    (other.edges)
    ,  faces    (other.faces)
    ,  octree   (other.octree)
  {
    if (restorePointer) {
      this->forEachVertex ([this] (WingedVertex& vertex) {
        vertex.edge (this->edges.element (vertex.edgeIndex ()));
      });
      this->forEachFace ([this] (WingedFace& face) {
        face.edge (this->edges.element (face.edgeIndex ()));
      });
      this->forEachEdge ([this] (WingedEdge& edge) {
        edge.setGeometry ( this->vertices.element (edge.vertex1Index ())
                         , this->vertices.element (edge.vertex2Index ())
                         , this->faces.element    (edge.leftFaceIndex ())
                         , this->faces.element    (edge.rightFaceIndex ())
                         , this->edges.element    (edge.leftPredecessorIndex ())
                         , this->edges.element    (edge.leftSuccessorIndex ())
                         , this->edges.element    (edge.rightPredecessorIndex ())
                         , this->edges.element    (edge.rightSuccessorIndex ())
                         );
      });
    }
  }

  bool operator== (const WingedMesh& other) const {
    return this->_index == other.index ();
  }

  bool operator!= (const WingedMesh& other) const {
    return ! this->operator== (other);
  }

  unsigned int     index       ()               const { return this->_index; }
  const glm::vec3& vector      (unsigned int i) const { return this->mesh.vertex (i); }
  unsigned int     index       (unsigned int i) const { return this->mesh.index  (i); }
  const glm::vec3& normal      (unsigned int i) const { return this->mesh.normal (i); }
  bool             isNewVertex (unsigned int i) const { return this->mesh.isNewVertex (i); }

  WingedVertex* vertex (unsigned int i) {
    return this->vertices.element (i);
  }

  WingedEdge* edge (unsigned int i) {
    return this->edges.element (i);
  }

  WingedFace* face (unsigned int i) {
    return this->faces.element (i);
  }

  WingedFace* someDegeneratedFace () {
    return this->octree.numDegeneratedElements () == 0
      ? nullptr
      : this->face (this->octree.someDegeneratedElement ());
  }

  WingedVertex& addVertex (const glm::vec3& pos) {
    WingedVertex& vertex = this->vertices.emplaceElement ();

    if (vertex.index () == this->mesh.numVertices ()) {
      this->mesh.addVertex (pos);
    }
    else {
      assert (vertex.index () < this->mesh.numVertices ());
      this->mesh.vertex (vertex.index (), pos);
    }
    return vertex;
  }

  WingedEdge& addEdge () {
    return this->edges.emplaceElement ();
  }

  void addFaceToOctree (const WingedFace& face, const PrimTriangle& geometry) {
    if (geometry.isDegenerated ()) {
      this->octree.addDegeneratedElement (face.index ());
    }
    else {
      this->octree.addElement (face.index (), geometry.center (), geometry.maxDimExtent ());
    }
  }

  WingedFace& addFace (const PrimTriangle& geometry) {
    WingedFace& face = this->faces.emplaceElement ();

    this->addFaceToOctree (face, geometry);

    if (3 * face.index () == this->mesh.numIndices ()) {
      this->mesh.addIndex (Util::invalidIndex ());
      this->mesh.addIndex (Util::invalidIndex ());
      this->mesh.addIndex (Util::invalidIndex ());
    }
    return face;
  }

  void index (unsigned int index, unsigned int vertexIndex) { 
    return this->mesh.index (index, vertexIndex); 
  }

  void vertex (unsigned int index, const glm::vec3& v) {
    assert (this->vertices.isFree (index) == false);
    return this->mesh.vertex (index,v);
  }

  void normal (unsigned int index, const glm::vec3& n) {
    assert (this->vertices.isFree (index) == false);
    return this->mesh.normal (index,n);
  }

  void isNewVertex (unsigned int index, bool value) {
    assert (this->vertices.isFree (index) == false);
    return this->mesh.isNewVertex (index, value);
  }

  void deleteEdge (WingedEdge& edge) { 
    this->edges.deleteElement (edge);
  }

  void deleteFace (WingedFace& face) { 
    this->octree.deleteElement (face.index ()); 
    this->faces.deleteElement (face);
  }

  void deleteVertex (WingedVertex& vertex) {
    this->vertices.deleteElement (vertex);
  }

  void realignFace (const WingedFace& face, const PrimTriangle& geometry) {
    this->octree.deleteElement (face.index ());
    this->addFaceToOctree (face, geometry);
  }

  void realignFace (const WingedFace& face) {
    this->realignFace (face.index (), face.triangle (*this->self));
  }

  void realignAllFaces () {
    this->forEachFace ([this] (WingedFace& f) { 
      this->realignFace (f);
    });
  }

  void sanitize () {
    this->octree.deleteEmptyChildren ();
    this->octree.shrinkRoot          ();
  }

  unsigned int numVertices () const {
    return this->vertices.numElements (); 
  }

  unsigned int numEdges () const { 
    return this->edges.numElements (); 
  }

  unsigned int numFaces () const { 
    return this->faces.numElements ();
  }

  unsigned int numIndices () const { 
    return this->mesh.numIndices (); 
  }

  bool isEmpty () const {
    return this->numVertices () == 0
        && this->numFaces    () == 0
        && this->numIndices  () == 0;
  }

  Mesh makePrunedMesh ( std::vector <unsigned int>* newFaceIndices
                      , std::vector <unsigned int>* newVertexIndices ) const
  {
    Mesh prunedMesh (this->mesh, false);

    if (this->vertices.hasFreeIndices () || this->faces.hasFreeIndices ()) {
      std::vector <unsigned int>  localNewVertexIndices;
      std::vector <unsigned int>& refNewVertexIndices = newVertexIndices ? *newVertexIndices
                                                                         :  localNewVertexIndices;

      refNewVertexIndices.clear  ();
      refNewVertexIndices.resize (this->mesh.numVertices (), Util::invalidIndex ());
      prunedMesh.reserveVertices (this->mesh.numVertices ());
      prunedMesh.reserveIndices  (this->mesh.numIndices  ());

      if (newFaceIndices) {
        assert (this->mesh.numIndices () % 3 == 0);

        newFaceIndices->clear ();
        newFaceIndices->resize (this->mesh.numIndices () / 3, Util::invalidIndex ());
      }

      this->forEachConstVertex ([&prunedMesh, &refNewVertexIndices, this] (const WingedVertex& v) {
        const unsigned int newIndex = prunedMesh.addVertex ( v.position    (*this->self)
                                                           , v.savedNormal (*this->self) );
        refNewVertexIndices [v.index ()] = newIndex;
      });
      this->forEachConstFace ( [&prunedMesh, &refNewVertexIndices, &newFaceIndices]
                               (const WingedFace& f) 
      {
        const unsigned int newV1 = refNewVertexIndices [f.vertexRef (0).index ()];
        const unsigned int newV2 = refNewVertexIndices [f.vertexRef (1).index ()];
        const unsigned int newV3 = refNewVertexIndices [f.vertexRef (2).index ()];

        assert (newV1 != Util::invalidIndex ());
        assert (newV2 != Util::invalidIndex ());
        assert (newV3 != Util::invalidIndex ());

        const unsigned int newI = prunedMesh.addIndex (newV1);
                                  prunedMesh.addIndex (newV2);
                                  prunedMesh.addIndex (newV3);

        assert (std::div (newI, 3).rem == 0);
        if (newFaceIndices) {
          (*newFaceIndices)[f.index ()] = std::div (newI, 3).quot;
        }
      });
    }
    else {
      prunedMesh = this->mesh;
    }
    return prunedMesh;
  }

  void fromMesh (const Mesh& mesh, const PrimPlane* mirror) {
    EdgeMap <WingedEdge*> edgeMap;

    /** `findOrAddEdge (m,i1,i2,f)` searches an edge between vertices 
     * `i1` and `i2` in `m`.
     * If such an edge exists, `f` becomes its new right face.
     * Otherwise a new edge is added to `this` and `m`, with `f` being its left face.
     * The found (resp. created) edge is returned.
     */
    auto findOrAddEdge = [this, &edgeMap] ( unsigned int index1, unsigned int index2
                                          , WingedFace& face ) -> WingedEdge&
    {
      WingedEdge** result = edgeMap.find (index1, index2);

      if (result) {
        WingedEdge* existingEdge = *result;

        existingEdge->rightFace (&face);
        face.edge (existingEdge);

        return *existingEdge;
      }
      else {
        WingedVertex* v1    = this->vertex (index1);
        WingedVertex* v2    = this->vertex (index2);
        WingedEdge& newEdge = this->addEdge ();
          
        edgeMap.add (index1, index2, &newEdge);
        newEdge.vertex1  (v1);
        newEdge.vertex2  (v2);
        newEdge.leftFace (&face);

        v1-> edge (&newEdge);
        v2-> edge (&newEdge);
        face.edge (&newEdge);

        return newEdge;
      }
    };

    // mesh
    this->reset ();

    this->mesh = bool (mirror) ? MeshUtil::mirror (mesh, *mirror)
                               : mesh;

    // octree
    this->setupOctreeRoot ();

    // vertices
    for (unsigned int i = 0; i < this->mesh.numVertices (); i++) {
      this->vertices.emplaceElement ();
    }

    // faces & edges
    assert (this->mesh.numIndices () % 3 == 0);

    edgeMap.resize (this->mesh.numVertices ());

    for (unsigned int i = 0; i < this->mesh.numIndices (); i += 3) {
      unsigned int index1 = this->mesh.index (i + 0);
      unsigned int index2 = this->mesh.index (i + 1);
      unsigned int index3 = this->mesh.index (i + 2);

      WingedFace& f = this->addFace (PrimTriangle (*this->self
                                                  , this->self->vertexRef (index1)
                                                  , this->self->vertexRef (index2)
                                                  , this->self->vertexRef (index3) ));

      WingedEdge& e1 = findOrAddEdge (index1, index2, f);
      WingedEdge& e2 = findOrAddEdge (index2, index3, f);
      WingedEdge& e3 = findOrAddEdge (index3, index1, f);

      e1.predecessor (f, &e3);
      e1.successor   (f, &e2);
      e2.predecessor (f, &e1);
      e2.successor   (f, &e3);
      e3.predecessor (f, &e2);
      e3.successor   (f, &e1);
    }

    if (this->octree.numDegeneratedElements () > 0) {
      Action::collapseDegeneratedFaces (*this->self);
    }

    this->vertices.forEachElement ([this] (WingedVertex& v) {
      v.setInterpolatedNormal (*this->self);
    });

    this->bufferData             ();
  }

  void bufferData  () { 
    if (this->numFaces () > 0 && this->faces.hasFreeIndices ()) {
      const unsigned int nonFree = this->faces.findSomeNonFreeIndex ();

      this->faces.forEachFreeIndex ([this, nonFree] (unsigned int index) {
        this->index ((3 * index) + 0, this->index ((3 * nonFree) + 0));
        this->index ((3 * index) + 1, this->index ((3 * nonFree) + 1));
        this->index ((3 * index) + 2, this->index ((3 * nonFree) + 2));
      });
    }
    this->mesh.bufferData (); 
  }

  void render (Camera& camera) { 
    this->mesh.render   (camera); 
#ifdef DILAY_RENDER_OCTREE
    this->octree.render (camera);
#endif
  }

  void reset () {
    this->mesh    .reset ();
    this->vertices.reset ();
    this->edges   .reset ();
    this->faces   .reset ();
    this->octree  .reset ();
  }

  void mirror (const PrimPlane& plane) {
    this->fromMesh (this->makePrunedMesh (nullptr, nullptr), &plane);
  }

  void setupOctreeRoot () {
    assert (this->octree.hasRoot () == false);

    glm::vec3 minVertex, maxVertex;
    this->mesh.minMax (minVertex, maxVertex);

    const glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
    const glm::vec3 delta  =  maxVertex - minVertex;
    const float     width  = glm::max (glm::max (delta.x, delta.y), delta.z);

    this->octree.setupRoot (center,width);
  }

  const RenderMode& renderMode () const { return this->mesh.renderMode (); }
  RenderMode&       renderMode ()       { return this->mesh.renderMode (); }

  bool intersects (const PrimRay& ray, Intersection& intersection) const {
    this->octree.intersects (ray, [this, &ray, &intersection] (unsigned int i) {
      const PrimTriangle tri = this->self->faceRef (i).triangle (*this->self);
      float              t;

      if (IntersectionUtil::intersects (ray, tri, &t)) {
        intersection.update (t, ray.pointAt (t), tri.normal ());
      }
    });
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    this->octree.intersects (ray, [this, &ray, &intersection] (unsigned int i) {
      WingedFace&        face = this->self->faceRef (i);
      const PrimTriangle tri  = face.triangle (*this->self);
      float              t;

      if (IntersectionUtil::intersects (ray, tri, &t)) {
        intersection.update (t, ray.pointAt (t), tri.normal (), *this->self, face);
      }
    });
    return intersection.isIntersection ();
  }

  template <typename T, typename ... Ts>
  bool intersectsT (const T& t, AffectedFaces& faces, const Ts& ... args) {
    this->octree.intersects (t, [this, &t, &faces, &args ...] (unsigned int i) {
      WingedFace& face = this->self->faceRef (i);

      if (IntersectionUtil::intersects (t, face.triangle (*this->self), args ...)) {
        faces.insert (face);
      }
    });
    faces.commit ();
    return faces.isEmpty () == false;
  }

  bool intersects (const PrimRay& ray, AffectedFaces& faces) {
    return this->intersectsT <PrimRay> (ray, faces, nullptr);
  }

  bool intersects (const PrimSphere& sphere, AffectedFaces& faces) {
    this->octree.intersects (sphere, [this, &sphere, &faces] (unsigned int i) {
      WingedFace& face = this->self->faceRef (i);
      glm::vec3 a,b,c;
      face.triangle (*this->self, a, b, c);

      if (IntersectionUtil::intersects (sphere, a, b, c)) {
        faces.insert (face);
      }
    });
    faces.commit ();
    return faces.isEmpty () == false;
  }

  bool intersects (const PrimPlane& plane, AffectedFaces& faces) {
    return this->intersectsT <PrimPlane> (plane, faces);
  }

  bool intersects (const PrimAABox& box, AffectedFaces& faces) {
    return this->intersectsT <PrimAABox> (box, faces);
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
    this->mesh.normalize ();
    this->octree.reset ();
    this->setupOctreeRoot ();

    this->forEachFace ([this] (WingedFace& face) { 
      this->addFaceToOctree (face, face.triangle (*this->self));
    });
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
    this->faces.forEachElement (f);
  }

  void forEachConstFace (const std::function <void (const WingedFace&)>& f) const {
    this->faces.forEachConstElement (f);
  }

  void runFromConfig (const Config& config) {
    this->color (config.get <Color> ("editor/mesh/color/normal"));
    this->wireframeColor (config.get <Color> ("editor/mesh/color/wireframe"));
  }
};

DELEGATE1_BIG3_SELF (WingedMesh, unsigned int)

WingedMesh :: WingedMesh (const WingedMesh& other, bool restorePointer)
  : impl (new Impl (this, *other.impl, restorePointer))
{}

DELEGATE1_CONST (bool            , WingedMesh, operator==, const WingedMesh&)
DELEGATE1_CONST (bool            , WingedMesh, operator!=, const WingedMesh&)

DELEGATE_CONST  (unsigned int    , WingedMesh, index)
DELEGATE1_CONST (const glm::vec3&, WingedMesh, vector, unsigned int)
DELEGATE1_CONST (unsigned int    , WingedMesh, index, unsigned int)
DELEGATE1_CONST (const glm::vec3&, WingedMesh, normal, unsigned int)
DELEGATE1_CONST (bool            , WingedMesh, isNewVertex, unsigned int)
DELEGATE1       (WingedVertex*   , WingedMesh, vertex, unsigned int)
DELEGATE1       (WingedEdge*     , WingedMesh, edge, unsigned int)
DELEGATE1       (WingedFace*     , WingedMesh, face, unsigned int)
DELEGATE        (WingedFace*     , WingedMesh, someDegeneratedFace)

DELEGATE1       (WingedVertex&   , WingedMesh, addVertex, const glm::vec3&)
DELEGATE        (WingedEdge&     , WingedMesh, addEdge)
DELEGATE1       (WingedFace&     , WingedMesh, addFace, const PrimTriangle&)
DELEGATE2       (void            , WingedMesh, index, unsigned int, unsigned int)
DELEGATE2       (void            , WingedMesh, vertex, unsigned int, const glm::vec3&)
DELEGATE2       (void            , WingedMesh, normal, unsigned int, const glm::vec3&)
DELEGATE2       (void            , WingedMesh, isNewVertex, unsigned int, bool)

GETTER_CONST    (const IndexOctree&, WingedMesh, octree)
GETTER_CONST    (const Mesh&       , WingedMesh, mesh)

DELEGATE1       (void, WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (void, WingedMesh, deleteFace, WingedFace&)
DELEGATE1       (void, WingedMesh, deleteVertex, WingedVertex&)
DELEGATE2       (void, WingedMesh, realignFace, const WingedFace&, const PrimTriangle&)
DELEGATE1       (void, WingedMesh, realignFace, const WingedFace&)
DELEGATE        (void, WingedMesh, realignAllFaces)
DELEGATE        (void, WingedMesh, sanitize)
 
DELEGATE_CONST  (unsigned int     , WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int     , WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int     , WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int     , WingedMesh, numIndices)
DELEGATE_CONST  (bool             , WingedMesh, isEmpty)

DELEGATE2_CONST (Mesh             , WingedMesh, makePrunedMesh, std::vector <unsigned int>*, std::vector <unsigned int>*)
DELEGATE2       (void             , WingedMesh, fromMesh, const Mesh&, const PrimPlane*)
DELEGATE        (void             , WingedMesh, bufferData)
DELEGATE1       (void             , WingedMesh, render, Camera&)
DELEGATE        (void             , WingedMesh, reset)
DELEGATE1       (void             , WingedMesh, mirror, const PrimPlane&)
DELEGATE_CONST  (const RenderMode&, WingedMesh, renderMode)
DELEGATE        (RenderMode&      , WingedMesh, renderMode)

DELEGATE2_CONST (bool, WingedMesh, intersects, const PrimRay&, Intersection&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimRay&, AffectedFaces&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimSphere&, AffectedFaces&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimPlane&, AffectedFaces&)
DELEGATE2       (bool, WingedMesh, intersects, const PrimAABox&, AffectedFaces&)

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
DELEGATE1       (void              , WingedMesh, runFromConfig, const Config&)
