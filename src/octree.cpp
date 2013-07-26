#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include "octree.hpp"
#include "macro.hpp"
#include "winged-face.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "util.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "mesh.hpp"
#include "renderer.hpp"
#include "color.hpp"
#endif

typedef std::unique_ptr <OctreeNode::Impl> Child;

/** Container for face to insert */
class FaceToInsert {
  public:
    FaceToInsert (const WingedFace& f, const Triangle& t)
      : face     (f)
      , center   (t.center    ())
      , width    (t.maxExtent ()) {}

    const WingedFace& face;
    const glm::vec3   center;
    const float       width;
};

/** Octree node implementation */
struct OctreeNode::Impl {
  Id                     id;
  OctreeNode             node;
  glm::vec3              center;
  float                  width;
  std::vector <Child>    children;
  int                    depth;
  std::list <WingedFace> faces;
  // cf. Octree.newParent

  static constexpr float relativeMinFaceSize = 0.05f;

#ifdef DILAY_RENDER_OCTREE
  Mesh mesh;
#endif

  Impl (const glm::vec3& c, float w, int d) 
    : node (this), center (c), width  (w), depth (d) {

#ifdef DILAY_RENDER_OCTREE
      float q = w * 0.5f;
      this->mesh.addVertex (glm::vec3 (-q, -q, -q)); 
      this->mesh.addVertex (glm::vec3 (-q, -q,  q)); 
      this->mesh.addVertex (glm::vec3 (-q,  q, -q)); 
      this->mesh.addVertex (glm::vec3 (-q,  q,  q)); 
      this->mesh.addVertex (glm::vec3 ( q, -q, -q)); 
      this->mesh.addVertex (glm::vec3 ( q, -q,  q)); 
      this->mesh.addVertex (glm::vec3 ( q,  q, -q)); 
      this->mesh.addVertex (glm::vec3 ( q,  q,  q)); 

      this->mesh.addIndex (0); this->mesh.addIndex (1); 
      this->mesh.addIndex (1); this->mesh.addIndex (3); 
      this->mesh.addIndex (3); this->mesh.addIndex (2); 
      this->mesh.addIndex (2); this->mesh.addIndex (0); 

      this->mesh.addIndex (4); this->mesh.addIndex (5); 
      this->mesh.addIndex (5); this->mesh.addIndex (7); 
      this->mesh.addIndex (7); this->mesh.addIndex (6); 
      this->mesh.addIndex (6); this->mesh.addIndex (4); 

      this->mesh.addIndex (1); this->mesh.addIndex (5); 
      this->mesh.addIndex (5); this->mesh.addIndex (7); 
      this->mesh.addIndex (7); this->mesh.addIndex (3); 
      this->mesh.addIndex (3); this->mesh.addIndex (1); 

      this->mesh.addIndex (4); this->mesh.addIndex (6); 
      this->mesh.addIndex (6); this->mesh.addIndex (2); 
      this->mesh.addIndex (2); this->mesh.addIndex (0); 
      this->mesh.addIndex (0); this->mesh.addIndex (4); 

      this->mesh.setPosition (this->center);
      this->mesh.bufferData  ();
#endif
  }
        Impl            (const Impl&) = delete;
  const Impl& operator= (const Impl&) = delete;

  Impl (Impl&& source) 
    : node     (this)
    , center   (source.center)
    , width    (source.width)
    , children (std::move (source.children))
    , depth    (source.depth)
    , faces    (std::move (source.faces))
#ifdef DILAY_RENDER_OCTREE
    , mesh     (source.mesh)

  { this->mesh.bufferData  (); }
#else
  {}
#endif

  void render () {
#ifdef DILAY_RENDER_OCTREE
    this->mesh.renderBegin ();
    glDisable (GL_DEPTH_TEST);
    Renderer :: setColor3 (Color (1.0f, 1.0f, 0.0f));
    glDrawElements (GL_LINES, this->mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);
    glEnable (GL_DEPTH_TEST);
    this->mesh.renderEnd ();

    for (Child& c : this->children) 
      c->render ();
#else
    assert (false);
#endif
  }

  bool contains (const glm::vec3& v) {
    const glm::vec3 min = this->center - glm::vec3 (this->width * 0.5f);
    const glm::vec3 max = this->center + glm::vec3 (this->width * 0.5f);
    return glm::all ( glm::lessThanEqual (min, v) )
       &&  glm::all ( glm::lessThanEqual (v, max) );
  }

  bool contains (const FaceToInsert& f) {
    return this->contains (f.center) && f.width <= this->width;
  }

  void makeChildren () {
    assert (this->children.size () == 0);
    float q          = this->width * 0.25f;
    float childWidth = this->width * 0.5f;
    int   childDepth = this->depth + 1;

    auto add = [this,childWidth,childDepth] (const glm::vec3& v) {
      this->children.emplace_back (new Impl (v,childWidth,childDepth));
    };
    
    this->children.reserve (8);
    add (this->center + glm::vec3 (-q, -q, -q)); // order is crucial
    add (this->center + glm::vec3 (-q, -q,  q));
    add (this->center + glm::vec3 (-q,  q, -q));
    add (this->center + glm::vec3 (-q,  q,  q));
    add (this->center + glm::vec3 ( q, -q, -q));
    add (this->center + glm::vec3 ( q, -q,  q));
    add (this->center + glm::vec3 ( q,  q, -q));
    add (this->center + glm::vec3 ( q,  q,  q));
  }

  WingedFace& insertIntoChild (const FaceToInsert& f) {
    if (this->children.size () == 0) {
      this->makeChildren           ();
      return this->insertIntoChild (f);
    }
    else {
      for (Child& child : this->children) {
        if (child->contains (f))
          return child->insertFace (f);
      }
      assert (false);
    }
  }

  WingedFace& insertFace (const FaceToInsert& f) {
    assert (this->contains (f));

    if (f.width <= this->width * Impl::relativeMinFaceSize) {
      return insertIntoChild (f);
    }
    else {
      this->faces.emplace_back (f.face.edge (), f.face.id ());
      this->faces.back ().octreeNode (&this->node);
      return this->faces.back ();
    }
  }

  void deleteFace (const WingedFace& face) {
    Util :: eraseByAddress <WingedFace> (this->faces, &face);
  }

  bool bboxIntersectRay (const Ray& ray) const {
    glm::vec3 invDir  = glm::vec3 (1.0f) / ray.direction ();
    glm::vec3 lower   = this->center - glm::vec3 (this->width * 0.5f);
    glm::vec3 upper   = this->center + glm::vec3 (this->width * 0.5f);
    glm::vec3 lowerTs = (lower - ray.origin ()) * invDir;
    glm::vec3 upperTs = (upper - ray.origin ()) * invDir;
    glm::vec3 min     = glm::min (lowerTs, upperTs);
    glm::vec3 max     = glm::max (lowerTs, upperTs);

    float tMin = glm::max ( glm::max (min.x, min.y), min.z );
    float tMax = glm::min ( glm::min (max.x, max.y), max.z );

    if (tMax < 0.0f || tMin > tMax)
      return false;
    else
      return true;
  }

  void facesIntersectRay ( const WingedMesh& mesh, const Ray& ray
                         , FaceIntersection& intersection) {
    for (WingedFace& face : this->faces) {
      Triangle  triangle = face.triangle (mesh);
      glm::vec3 p;

      if (triangle.intersectRay (ray,p)) {
        intersection.update (glm::distance (ray.origin (), p),p,face);
      }
    }
  }

  void intersectRay ( const WingedMesh& mesh, const Ray& ray
                    , FaceIntersection& intersection) {
    if (this->bboxIntersectRay (ray)) {
      this->facesIntersectRay (mesh,ray,intersection);
    }
  }

  unsigned int numFaces () const { return this->faces.size (); }

  OctreeNodeFaceIterator faceIterator () { 
    return OctreeNodeFaceIterator (*this);
  }

  ConstOctreeNodeFaceIterator faceIterator () const { 
    return ConstOctreeNodeFaceIterator (*this); 
  }
};

OctreeNode :: OctreeNode (OctreeNode::Impl* i) : impl (i) { }

ID             (OctreeNode)
GETTER         (int, OctreeNode, depth)
GETTER         (const glm::vec3&, OctreeNode, center)
GETTER         (float, OctreeNode, width)
DELEGATE1      (void, OctreeNode, deleteFace, const WingedFace&)
DELEGATE3      (void, OctreeNode, intersectRay, const WingedMesh&, const Ray&, FaceIntersection&)
DELEGATE_CONST (unsigned int, OctreeNode, numFaces)
DELEGATE       (OctreeNodeFaceIterator, OctreeNode, faceIterator)
DELEGATE_CONST (ConstOctreeNodeFaceIterator, OctreeNode, faceIterator)

/** Octree class */
struct Octree::Impl {
  typedef std::unordered_map <IdType,WingedFace*> IdMap;

  Child root;
  IdMap idMap;

  WingedFace& insertNewFace (const WingedFace& face, const Triangle& geometry) {
    assert (! this->hasFace (face.id ())); 
    return this->insertFace (face,geometry);
  }
  WingedFace& reInsertFace (const WingedFace& face, const Triangle& geometry) {
    assert (this->hasFace (face.id ())); 
    this->deleteFace (face);
    return this->insertFace (face,geometry);
  }

  WingedFace& insertFace (const WingedFace& face, const Triangle& geometry) {
    FaceToInsert faceToInsert (face,geometry);

    if (! this->root) {
      glm::vec3 rootCenter = (geometry.maximum () + geometry.minimum ()) 
                           * glm::vec3 (0.5f);
      this->root = Child (new OctreeNode::Impl (rootCenter, faceToInsert.width, 0));
    }

    WingedFace* wingedFace;
    if (this->root->contains (faceToInsert)) {
      wingedFace = &this->root->insertFace (faceToInsert);
    }
    else {
      this->makeParent (faceToInsert);
      wingedFace = &this->insertFace (face,geometry);
    }
    this->idMap.insert ({{face.id (), wingedFace}});
    return *wingedFace;
  }

  void deleteFace (const WingedFace& face) {
    assert (face.octreeNode ());
    assert (this->hasFace (face.id ())); 
    this->idMap.erase (face.id ());
    face.octreeNodeRef ().impl->deleteFace (face);
  }

  bool hasFace (IdType id) { return this->idMap.count (id) == 1; }

  WingedFace& getFace (IdType id) {
    IdMap::iterator result = this->idMap.find (id);

    assert (result != this->idMap.end ());
    return *result->second;
  }

  void makeParent (const FaceToInsert& f) {
    assert (this->root);

    glm::vec3 parentCenter;
    glm::vec3 rootCenter    = this->root->center;
    float     rootWidth     = this->root->width;
    float     halfRootWidth = this->root->width * 0.5f;
    int       index         = 0;

    if (rootCenter.x < f.center.x) 
      parentCenter.x = rootCenter.x + halfRootWidth;
    else {
      parentCenter.x = rootCenter.x - halfRootWidth;
      index         += 4;
    }
    if (rootCenter.y < f.center.y) 
      parentCenter.y = rootCenter.y + halfRootWidth;
    else {
      parentCenter.y = rootCenter.y - halfRootWidth;
      index         += 2;
    }
    if (rootCenter.z < f.center.z) 
      parentCenter.z = rootCenter.z + halfRootWidth;
    else {
      parentCenter.z = rootCenter.z - halfRootWidth;
      index         += 1;
    }

    OctreeNode::Impl* newRoot = new OctreeNode::Impl ( parentCenter
                                                     , rootWidth * 2.0f
                                                     , this->root->depth - 1);
    newRoot->makeChildren ();
    newRoot->children [index] = std::move (this->root);
    this->root.reset (newRoot);
  }

  void render () { 
#ifdef DILAY_RENDER_OCTREE
    if (this->root) 
      this->root->render ();
#else
    assert (false && "compiled without rendering support for octrees");
#endif
  }

  void intersectRay ( const WingedMesh& mesh, const Ray& ray
                    , FaceIntersection& intersection) {
    for (auto it = this->nodeIterator (); it.isValid (); it.next ()) {
      it.element ().intersectRay (mesh,ray,intersection);
    }
  }

  void reset () { this->root.release (); }

  void reset (const glm::vec3& center, float width) {
    this->reset ();
    this->root = Child (new OctreeNode::Impl (center, width, 0));
  }

  OctreeFaceIterator      faceIterator ()       { return OctreeFaceIterator      (*this); }
  ConstOctreeFaceIterator faceIterator () const { return ConstOctreeFaceIterator (*this); }
  OctreeNodeIterator      nodeIterator ()       { return OctreeNodeIterator      (*this); }
  ConstOctreeNodeIterator nodeIterator () const { return ConstOctreeNodeIterator (*this); }
};

DELEGATE_CONSTRUCTOR (Octree)
DELEGATE_DESTRUCTOR  (Octree)
DELEGATE2      (WingedFace& , Octree, insertNewFace, const WingedFace&, const Triangle&)
DELEGATE2      (WingedFace& , Octree, reInsertFace, const WingedFace&, const Triangle&)
DELEGATE1      (void        , Octree, deleteFace, const WingedFace&)
DELEGATE1      (bool        , Octree, hasFace, IdType)
DELEGATE1      (WingedFace& , Octree, getFace, IdType)
DELEGATE       (void, Octree, render)
DELEGATE3      (void, Octree, intersectRay, const WingedMesh&, const Ray&, FaceIntersection&)
DELEGATE       (void, Octree, reset)
DELEGATE2      (void, Octree, reset, const glm::vec3&, float)
DELEGATE       (OctreeFaceIterator, Octree, faceIterator)
DELEGATE_CONST (ConstOctreeFaceIterator, Octree, faceIterator)
DELEGATE       (OctreeNodeIterator, Octree, nodeIterator)
DELEGATE_CONST (ConstOctreeNodeIterator, Octree, nodeIterator)

/** Internal template for iterators over all faces of a node */
template <bool isConstant>
struct OctreeNodeFaceIteratorTemplate {
  typedef typename 
    std::conditional <isConstant, const OctreeNode::Impl, OctreeNode::Impl>::type 
    T_OctreeNodeImpl;
  typedef typename 
    std::conditional <isConstant, std::list <WingedFace>::const_iterator
                                , std::list <WingedFace>::iterator>::type
    T_Iterator;
  typedef typename 
    std::conditional <isConstant, const WingedFace, WingedFace>::type
    T_Element;

  T_OctreeNodeImpl& octreeNode;
  T_Iterator        iterator;

  OctreeNodeFaceIteratorTemplate (T_OctreeNodeImpl& n) 
    : octreeNode (n) 
    , iterator (n.faces.begin ())
  {}

  bool isValid () const { 
    return this->iterator != this->octreeNode.faces.end ();
  }

  T_Element& element () const {
    assert (this->isValid ());
    return * this->iterator;
  }

  void next () {
    assert (this->isValid ());
    this->iterator++;
  }

  int depth () const { 
    return this->octreeNode.depth;
  }
};

DELEGATE1_BIG4 (OctreeNodeFaceIterator,OctreeNode::Impl&)
DELEGATE_CONST (bool       , OctreeNodeFaceIterator, isValid)
DELEGATE_CONST (WingedFace&, OctreeNodeFaceIterator, element)
DELEGATE       (void       , OctreeNodeFaceIterator, next)
DELEGATE_CONST (int        , OctreeNodeFaceIterator, depth)

DELEGATE1_BIG4 (ConstOctreeNodeFaceIterator,const OctreeNode::Impl&)
DELEGATE_CONST (bool             , ConstOctreeNodeFaceIterator, isValid)
DELEGATE_CONST (const WingedFace&, ConstOctreeNodeFaceIterator, element)
DELEGATE       (void             , ConstOctreeNodeFaceIterator, next)
DELEGATE_CONST (int              , ConstOctreeNodeFaceIterator, depth)

/** Internal template for iterators over all faces of an octree */
template <bool isConstant>
struct OctreeFaceIteratorTemplate {
  typedef typename 
    std::conditional <isConstant, const Octree::Impl, Octree::Impl>::type 
    T_OctreeImpl;
  typedef typename 
    std::conditional <isConstant, const WingedFace, WingedFace>::type
    T_Element;
  typedef 
    OctreeNodeFaceIteratorTemplate <isConstant>
    T_OctreeNodeFaceIterator;

  std::list <T_OctreeNodeFaceIterator> faceIterators;

  OctreeFaceIteratorTemplate (T_OctreeImpl& octree) {
    if (octree.root) {
      T_OctreeNodeFaceIterator rootIterator (*octree.root);
      this->faceIterators.push_back (rootIterator);
      this->skipEmptyNodes ();
    }
  }

  void skipEmptyNodes () {
    if (this->faceIterators.size () > 0) {
      T_OctreeNodeFaceIterator& current = this->faceIterators.front ();
      if (! current.isValid ()) {
        for (auto & c : current.octreeNode.children) {
          this->faceIterators.push_back (T_OctreeNodeFaceIterator (*c.get ()));
        }
        this->faceIterators.pop_front ();
        this->skipEmptyNodes ();
      }
    }
  }

  bool isValid () const { return this->faceIterators.size () > 0; }

  T_Element & element () const {
    assert (this->isValid ());
    return this->faceIterators.front ().element ();
  }

  void next () { 
    assert (this->isValid ());
    this->faceIterators.front ().next ();
    this->skipEmptyNodes ();
  }

  int depth () const {
    assert (this->isValid ());
    return this->faceIterators.front ().octreeNode.depth;
  }
};

DELEGATE1_BIG4 (OctreeFaceIterator,Octree::Impl&)
DELEGATE_CONST (bool       , OctreeFaceIterator, isValid)
DELEGATE_CONST (WingedFace&, OctreeFaceIterator, element)
DELEGATE       (void       , OctreeFaceIterator, next)
DELEGATE_CONST (int        , OctreeFaceIterator, depth)

DELEGATE1_BIG4 (ConstOctreeFaceIterator,const Octree::Impl&)
DELEGATE_CONST (bool             , ConstOctreeFaceIterator, isValid)
DELEGATE_CONST (const WingedFace&, ConstOctreeFaceIterator, element)
DELEGATE       (void             , ConstOctreeFaceIterator, next)
DELEGATE_CONST (int              , ConstOctreeFaceIterator, depth)

/** Internal template for iterators over all nodes of an octree */
template <bool isConstant>
struct OctreeNodeIteratorTemplate {
  typedef typename 
    std::conditional <isConstant, const Octree::Impl, Octree::Impl>::type 
    T_OctreeImpl;
  typedef typename 
    std::conditional <isConstant, const OctreeNode,OctreeNode>::type 
    T_OctreeNode;
  typedef typename 
    std::conditional <isConstant, const OctreeNode::Impl,OctreeNode::Impl>::type 
    T_OctreeNodeImpl;

  std::list <T_OctreeNodeImpl*> nodes;

  OctreeNodeIteratorTemplate (T_OctreeImpl& octree) {
    if (octree.root) {
      this->nodes.push_back (octree.root.get ());
    }
  }

  T_OctreeNode& element () const {
    assert (this->isValid ());
    return this->nodes.front ()->node;
  }

  bool isValid () const { return this->nodes.size () > 0; }

  void next () { 
    assert (this->isValid ());
    for (auto & c : this->nodes.front ()->children) {
      this->nodes.push_back (c.get ());
    }
    this->nodes.pop_front ();
  }
};

DELEGATE1_BIG4 (OctreeNodeIterator, Octree::Impl&)    
DELEGATE_CONST (bool              , OctreeNodeIterator, isValid)
DELEGATE_CONST (OctreeNode&       , OctreeNodeIterator, element)
DELEGATE       (void              , OctreeNodeIterator, next)

DELEGATE1_BIG4 (ConstOctreeNodeIterator, const Octree::Impl&)    
DELEGATE_CONST (bool              , ConstOctreeNodeIterator, isValid)
DELEGATE_CONST (const OctreeNode& , ConstOctreeNodeIterator, element)
DELEGATE       (void              , ConstOctreeNodeIterator, next)
