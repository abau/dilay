#include <glm/glm.hpp>
#include "octree.hpp"
#include "macro.hpp"
#include "maybe.hpp"
#include "winged-face.hpp"
#include "triangle.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "mesh.hpp"
#include "renderer.hpp"
#include "color.hpp"
#endif

/** Container for face to insert */
class FaceToInsert {
  public:
    FaceToInsert (const WingedFace& f, const Triangle& g)
      : face     (f)
      , center   (g.center    ())
      , width    (g.maxExtent ()) 
    {}

    const WingedFace& face;
    const glm::vec3   center;
    const float       width;
};

/** Octree node implementation */
struct OctreeNodeImpl {
  typedef std::vector <OctreeNodeImpl> Children;

  OctreeNode   node;
  glm::vec3    center;
  float        width;
  Children     children;
  Faces        faces;
  unsigned int depth;

#ifdef DILAY_RENDER_OCTREE
  Mesh mesh;
#endif

  OctreeNodeImpl (const glm::vec3& c, float w, unsigned int d) 
    : node (OctreeNode (this)), center (c), width  (w), depth (d) {

#ifdef DILAY_RENDER_OCTREE
      float q = w * 0.5;
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

  OctreeNodeImpl (const OctreeNodeImpl& source) 
    : node     (source.node)
    , center   (source.center)
    , width    (source.width)
    , children (source.children)
    , faces    (source.faces)
    , depth    (source.depth)
#ifdef DILAY_RENDER_OCTREE
    , mesh     (source.mesh) {
    this->mesh.bufferData ();
  }
#else
    {}
#endif

  const OctreeNodeImpl& operator= (const OctreeNodeImpl& source) {
    if (this == &source) return *this;
    OctreeNodeImpl tmp (source);
    std::swap (this->node, tmp.node);
    std::swap (this->center, tmp.center);
    std::swap (this->width, tmp.width);
    std::swap (this->children, tmp.children);
    std::swap (this->faces, tmp.faces);
    std::swap (this->depth, tmp.depth);
#ifdef DILAY_RENDER_OCTREE
    std::swap (this->mesh, tmp.mesh);
#endif
    this->mesh.bufferData ();
    return *this;
  }

  void render () {
#ifdef DILAY_RENDER_OCTREE
    this->mesh.renderBegin ();
    glDisable (GL_DEPTH_TEST);
    Renderer :: global ().setColor3 (Color (1.0f, 1.0f, 0.0f));
    glDrawElements (GL_LINES, this->mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);
    glEnable (GL_DEPTH_TEST);
    this->mesh.renderEnd ();

    for (OctreeNodeImpl& c : this->children) 
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
    float        q          = this->width * 0.25f;
    float        childWidth = this->width * 0.5f;
    unsigned int childDepth = this->depth + 1;

    auto add = [this,childWidth,childDepth] (const glm::vec3& v) {
      OctreeNodeImpl child (v,childWidth,childDepth);
      this->children.push_back (child);
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
      for (OctreeNodeImpl& child : this->children) {
        if (child.contains (f))
          return child.insertFace (f);
      }
      assert (false);
    }
  }

  LinkedFace insertFace (const FaceToInsert& f) {
    assert (this->contains (f));

    //if (f.width <= this->width * 0.5f) {
    if (f.width <= this->width * 0.01f) {
      return insertIntoChild (f);
    }
    else {
      this->faces.push_back (f.face);
      return --this->faces.end ();
    }
  }

  void deleteFace (LinkedFace face) {
    this->faces.erase (face);
  }
};

OctreeNode :: OctreeNode (OctreeNodeImpl* i) : impl (i) { }

OctreeNode :: OctreeNode (const OctreeNode& source) : impl (source.impl) {}

const OctreeNode& OctreeNode :: operator= (const OctreeNode& source) {
  this->impl = source.impl;
  return *this;
}
OctreeNode :: ~OctreeNode () {}

/** Octree main class */
struct OctreeImpl {
  MaybePtr <OctreeNodeImpl> root;

  OctreeImpl () { }

  LinkedFace insertFace (const WingedFace& face, const Triangle& geometry) {
    FaceToInsert faceToInsert (face,geometry);

    if (this->root.isUndefined ()) {
      OctreeNodeImpl r (faceToInsert.center, faceToInsert.width, 0);
      this->root.data (r);
    }

    if (this->root.data ()->contains (faceToInsert)) {
      return this->root.data ()->insertFace (faceToInsert);
    }
    else {
      makeParent (faceToInsert);
      return this->root.data ()->insertFace (faceToInsert);
    }
  }

  void makeParent (const FaceToInsert& f) {
    assert (this->root.isDefined ());

    glm::vec3 parentCenter;
    glm::vec3 rootCenter    = this->root.data ()->center;
    float     rootWidth     = this->root.data ()->width;
    float     halfRootWidth = this->root.data ()->width * 0.5f;
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
    
    OctreeNodeImpl newRoot (parentCenter, rootWidth * 2.0f, 0);

    newRoot.makeChildren ();
    newRoot.children [index] = *this->root.data ();
    this->root.data (newRoot);
  }

  void render () { 
#ifdef DILAY_RENDER_OCTREE
    if (this->root.isDefined ()) 
      this->root.data ()->render ();
#else
    assert (false && "compiled without rendering support for octrees");
#endif
  }

  OctreeFaceIterator faceIterator () { 
    return OctreeFaceIterator (*this); 
  }

  ConstOctreeFaceIterator faceIterator () const { 
    return ConstOctreeFaceIterator (*this); 
  }

  OctreeNodeIterator nodeIterator () { 
    return OctreeNodeIterator (*this); 
  }

  ConstOctreeNodeIterator nodeIterator () const { 
    return ConstOctreeNodeIterator (*this); 
  }
};

DELEGATE_BIG4  (Octree)
DELEGATE2      (LinkedFace, Octree, insertFace, const WingedFace&, const Triangle&)
DELEGATE       (void, Octree, render)
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
    , _face      (n.faces.end ())
  {}

  bool hasFace () const { 
    return this->_face != this->octreeNode.faces.end ();
  }

  T_LinkedFace face () const {
    assert (this->hasFace ());
    return this->_face;
  }

  void next () {
    assert (this->hasFace ());
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
    if (octree.root.isDefined ()) {
      this->faceIterators.push_back (
          T_OctreeNodeFaceIterator (*octree.root.data ())
      );
    }
  }

  bool hasFace () const { return this->faceIterators.size () > 0; }

  T_LinkedFace face () const {
    assert (this->hasFace ());
    return this->faceIterators.begin ()->face ();
  }

  void next () { 
    std::function <void ()>  check = [this,&check] () {
      T_OctreeNodeFaceIterator current = *this->faceIterators.begin ();

      if (! current.hasFace ()) {

        for (T_OctreeNodeImpl& c : current.octreeNode.children) {
          this->faceIterators.push_back (T_OctreeNodeFaceIterator (c));
        }
        this->faceIterators.pop_front ();
        check ();
      }
    };

    assert (this->hasFace ());
    this->faceIterators.begin ()->next ();
    check ();
  }

  unsigned int depth () const {
    assert (this->hasFace ());
    return this->faceIterators.begin ()->octreeNode.depth;
  }
};

DELEGATE1_BIG4        (OctreeFaceIterator,OctreeImpl&)
DELEGATE_CONST        (bool            , OctreeFaceIterator, hasFace)
DELEGATE_CONST        (LinkedFace      , OctreeFaceIterator, face)
DELEGATE              (void            , OctreeFaceIterator, next)
DELEGATE_CONST        (unsigned int    , OctreeFaceIterator, depth)

DELEGATE1_BIG4        (ConstOctreeFaceIterator,const OctreeImpl&)
DELEGATE_CONST        (bool            , ConstOctreeFaceIterator, hasFace)
DELEGATE_CONST        (ConstLinkedFace , ConstOctreeFaceIterator, face)
DELEGATE              (void            , ConstOctreeFaceIterator, next)
DELEGATE_CONST        (unsigned int    , ConstOctreeFaceIterator, depth)

/** Internal template for iterators over all nodes of an octree */
template < class T_OctreeImpl
         , class T_OctreeNode
         , class T_OctreeNodeImpl
         >
struct OctreeNodeIteratorTemplate {
  std::list <T_OctreeNodeImpl*> nodes;

  OctreeNodeIteratorTemplate (T_OctreeImpl& octree) {
    if (octree.root.isDefined ()) {
      this->nodes.push_back (octree.root.data ());
    }
  }

  T_OctreeNode& node () const {
    return (*this->nodes.begin ())->node;
  }

  bool hasNode () const { return this->nodes.size () > 0; }

  void next () { 
    assert (this->hasNode ());
    for (T_OctreeNodeImpl& c : (*this->nodes.begin ())->children) {
      this->nodes.push_back (&c);
    }
    this->nodes.pop_front ();
  }

  unsigned int depth () const {
    assert (this->hasNode ());
    return (*this->nodes.begin ())->depth;
  }
};

DELEGATE1_BIG4 (OctreeNodeIterator, OctreeImpl&)    
DELEGATE_CONST (bool              , OctreeNodeIterator, hasNode)
DELEGATE_CONST (OctreeNode&       , OctreeNodeIterator, node)
DELEGATE       (void              , OctreeNodeIterator, next)
DELEGATE_CONST (unsigned int      , OctreeNodeIterator, depth)

DELEGATE1_BIG4 (ConstOctreeNodeIterator, const OctreeImpl&)    
DELEGATE_CONST (bool              , ConstOctreeNodeIterator, hasNode)
DELEGATE_CONST (const OctreeNode& , ConstOctreeNodeIterator, node)
DELEGATE       (void              , ConstOctreeNodeIterator, next)
DELEGATE_CONST (unsigned int      , ConstOctreeNodeIterator, depth)
