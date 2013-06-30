#include <glm/glm.hpp>
#include "octree.hpp"
#include "macro.hpp"
#include "maybe.hpp"
#include "winged-face.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "mesh.hpp"
#include "renderer.hpp"
#include "color.hpp"
#endif

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
  typedef std::vector <Impl> Children;

  Id           id;
  OctreeNode   node;
  glm::vec3    center;
  float        width;
  Children     children;
  Faces        faces;
  int          depth;
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
    , faces    (std::move (source.faces))
    , depth    (source.depth)
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

    for (Impl& c : this->children) 
      c.render ();
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
      this->children.emplace_back (v,childWidth,childDepth);
    };
    
    add (this->center + glm::vec3 (-q, -q, -q)); // order is crucial
    add (this->center + glm::vec3 (-q, -q,  q));
    add (this->center + glm::vec3 (-q,  q, -q));
    add (this->center + glm::vec3 (-q,  q,  q));
    add (this->center + glm::vec3 ( q, -q, -q));
    add (this->center + glm::vec3 ( q, -q,  q));
    add (this->center + glm::vec3 ( q,  q, -q));
    add (this->center + glm::vec3 ( q,  q,  q));
  }

  LinkedFace insertIntoChild (const FaceToInsert& f) {
    if (this->children.size () == 0) {
      this->makeChildren           ();
      return this->insertIntoChild (f);
    }
    else {
      for (Impl& child : this->children) {
        if (child.contains (f))
          return child.insertFace (f);
      }
      assert (false);
    }
  }

  LinkedFace insertFace (const FaceToInsert& f) {
    assert (this->contains (f));

    if (f.width <= this->width * Impl::relativeMinFaceSize) {
      return insertIntoChild (f);
    }
    else {
      this->faces.emplace_back (f.face.edge (), f.face.id ());
      this->faces.back ().octreeNode (&this->node);
      return --this->faces.end ();
    }
  }

  void deleteFace (LinkedFace face) {
    this->faces.erase (face);
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
    for (LinkedFace face = this->faces.begin (); face != this->faces.end (); ++face) {
      Triangle  triangle = face->triangle (mesh);
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

  LinkedFace getFace (const OctreeNodeFaceReference& reference) {
    assert (this->id.get () == reference.nodeId ());

    for (auto it = this->faces.begin (); it != this->faces.end(); ++it) {
      if (it->id () == reference.faceId ())
        return it;
    }
    assert (false);
  }
};

OctreeNode :: OctreeNode (OctreeNode::Impl* i) : impl (i) { }

DELEGATE1      (void, OctreeNode, deleteFace, LinkedFace)
DELEGATE3      (void, OctreeNode, intersectRay, const WingedMesh&, const Ray&, FaceIntersection&)
DELEGATE_CONST (unsigned int, OctreeNode, numFaces)
DELEGATE1      (LinkedFace, OctreeNode, getFace, const OctreeNodeFaceReference&)
GETTER         (int, OctreeNode, depth)
GETTER         (const glm::vec3&, OctreeNode, center)
GETTER         (float, OctreeNode, width)
ID             (OctreeNode)

/** Octree main class */
struct Octree::Impl {
  typedef std::unique_ptr <OctreeNode::Impl> Root;
  Root root;

  LinkedFace insertFace (const WingedFace& face, const Triangle& geometry) {
    FaceToInsert faceToInsert (face,geometry);

    if (! this->root) {
      glm::vec3 rootCenter = (geometry.maximum () + geometry.minimum ()) 
                           * glm::vec3 (0.5f);
      this->root = Root (new OctreeNode::Impl (rootCenter, faceToInsert.width, 0));
    }

    if (this->root->contains (faceToInsert)) {
      return this->root->insertFace (faceToInsert);
    }
    else {
      this->makeParent (faceToInsert);
      return this->insertFace (face,geometry);
    }
  }

  LinkedFace getFace (const OctreeNodeFaceReference& reference) {
    OctreeNode* pNode = nullptr;
    for (auto it = this->nodeIterator (); it.isValid (); it.next ()) {
      if (it.element ().id () == reference.nodeId ())
        pNode = &it.element ();
    }
    assert (pNode);
    return pNode->getFace (reference);
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

    newRoot->children [index].children = std::move (this->root->children);
    newRoot->children [index].faces    = std::move (this->root->faces);
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
    this->root = Root (new OctreeNode::Impl (center, width, 0));
  }

  OctreeFaceIterator      faceIterator ()       { return OctreeFaceIterator      (*this); }
  ConstOctreeFaceIterator faceIterator () const { return ConstOctreeFaceIterator (*this); }
  OctreeNodeIterator      nodeIterator ()       { return OctreeNodeIterator      (*this); }
  ConstOctreeNodeIterator nodeIterator () const { return ConstOctreeNodeIterator (*this); }
};

DELEGATE_CONSTRUCTOR (Octree)
DELEGATE_DESTRUCTOR  (Octree)
DELEGATE2      (LinkedFace, Octree, insertFace, const WingedFace&, const Triangle&)
DELEGATE1      (LinkedFace, Octree, getFace, const OctreeNodeFaceReference&)
DELEGATE       (void, Octree, render)
DELEGATE3      (void, Octree, intersectRay, const WingedMesh&, const Ray&, FaceIntersection&)
DELEGATE       (void, Octree, reset)
DELEGATE2      (void, Octree, reset, const glm::vec3&, float)
DELEGATE       (OctreeFaceIterator, Octree, faceIterator)
DELEGATE_CONST (ConstOctreeFaceIterator, Octree, faceIterator)
DELEGATE       (OctreeNodeIterator, Octree, nodeIterator)
DELEGATE_CONST (ConstOctreeNodeIterator, Octree, nodeIterator)

/** Internal template for iterators over all nodes of an octree */
template <class T_OctreeNodeImpl, class T_LinkedFace>
struct OctreeNodeFaceIteratorTemplate {
  T_OctreeNodeImpl& octreeNode;
  T_LinkedFace      _face;

  OctreeNodeFaceIteratorTemplate (T_OctreeNodeImpl& n) 
    : octreeNode (n) 
    , _face      (n.faces.begin ())
  {}

  bool isValid () const { 
    return this->_face != this->octreeNode.faces.end ();
  }

  T_LinkedFace element () const {
    assert (this->isValid ());
    return this->_face;
  }

  void next () {
    assert (this->isValid ());
    this->_face++;
  }
};

/** Internal template for iterators over all faces of an octree */
template < class T_OctreeImpl
         , class T_OctreeNodeImpl
         , class T_OctreeNodeFaceIterator
         , class T_LinkedFace
         >
struct OctreeFaceIteratorTemplate {
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
      T_OctreeNodeFaceIterator current = *this->faceIterators.begin ();
      if (! current.isValid ()) {
        this->faceIterators.pop_front ();

        for (T_OctreeNodeImpl& c : current.octreeNode.children) {
          this->faceIterators.push_back (T_OctreeNodeFaceIterator (c));
        }
        this->skipEmptyNodes ();
      }
    }
  }

  bool isValid () const { return this->faceIterators.size () > 0; }

  T_LinkedFace element () const {
    assert (this->isValid ());
    return this->faceIterators.begin ()->element ();
  }

  void next () { 
    assert (this->isValid ());
    this->faceIterators.begin ()->next ();
    this->skipEmptyNodes ();
  }

  int depth () const {
    assert (this->isValid ());
    return this->faceIterators.begin ()->octreeNode.depth;
  }
};

DELEGATE1_BIG4 (OctreeFaceIterator,Octree::Impl&)
DELEGATE_CONST (bool       , OctreeFaceIterator, isValid)
DELEGATE_CONST (LinkedFace , OctreeFaceIterator, element)
DELEGATE       (void       , OctreeFaceIterator, next)
DELEGATE_CONST (int        , OctreeFaceIterator, depth)

DELEGATE1_BIG4 (ConstOctreeFaceIterator,const Octree::Impl&)
DELEGATE_CONST (bool            , ConstOctreeFaceIterator, isValid)
DELEGATE_CONST (ConstLinkedFace , ConstOctreeFaceIterator, element)
DELEGATE       (void            , ConstOctreeFaceIterator, next)
DELEGATE_CONST (int             , ConstOctreeFaceIterator, depth)

/** Internal template for iterators over all nodes of an octree */
template < class T_OctreeImpl
         , class T_OctreeNode
         , class T_OctreeNodeImpl
         >
struct OctreeNodeIteratorTemplate {
  std::list <T_OctreeNodeImpl*> nodes;

  OctreeNodeIteratorTemplate (T_OctreeImpl& octree) {
    if (octree.root) {
      this->nodes.push_back (octree.root.get ());
    }
  }

  T_OctreeNode& element () const {
    assert (this->isValid ());
    return (*this->nodes.begin ())->node;
  }

  bool isValid () const { return this->nodes.size () > 0; }

  void next () { 
    assert (this->isValid ());
    for (T_OctreeNodeImpl& c : (*this->nodes.begin ())->children) {
      this->nodes.push_back (&c);
    }
    this->nodes.pop_front ();
  }
};

DELEGATE1_BIG4 (OctreeNodeIterator, Octree::Impl&)    
DELEGATE_CONST (bool              , OctreeNodeIterator, isValid)
DELEGATE       (OctreeNode&       , OctreeNodeIterator, element)
DELEGATE       (void              , OctreeNodeIterator, next)

DELEGATE1_BIG4 (ConstOctreeNodeIterator, const Octree::Impl&)    
DELEGATE_CONST (bool              , ConstOctreeNodeIterator, isValid)
DELEGATE_CONST (const OctreeNode& , ConstOctreeNodeIterator, element)
DELEGATE       (void              , ConstOctreeNodeIterator, next)
