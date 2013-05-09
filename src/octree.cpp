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

  glm::vec3    center;
  float        width;
  Children     children;
  Faces        faces;
  unsigned int depth;

#ifdef DILAY_RENDER_OCTREE
  Mesh mesh;
#endif

  OctreeNodeImpl (const glm::vec3& c, float w, unsigned int d) 
    : center (c), width  (w), depth (d) {

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
    : center (source.center)
    , width (source.width)
    , children (source.children)
    , faces (source.faces)
    , depth (source.depth)
#ifdef DILAY_RENDER_OCTREE
    , mesh (source.mesh) {
    this->mesh.bufferData ();
  }
#else
    {}
#endif

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
      this->children.push_back (OctreeNodeImpl (v,childWidth,childDepth));
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
};

/** Octree implemenation */
struct OctreeImpl {
  MaybePtr <OctreeNodeImpl> root;

  OctreeImpl () { }

  LinkedFace insertFace (const WingedFace& face, const Triangle& geometry) {
    FaceToInsert faceToInsert (face,geometry);

    if (this->root.isUndefined ()) {
      this->root.data (OctreeNodeImpl (faceToInsert.center, faceToInsert.width, 0));
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
    
    OctreeNodeImpl newRoot (parentCenter, this->root.data ()->width * 2.0f, 0);

    newRoot.makeChildren ();

    std::swap (newRoot.children [index].center  , this->root.data ()->center);
    std::swap (newRoot.children [index].width   , this->root.data ()->width);
    std::swap (newRoot.children [index].children, this->root.data ()->children);
    std::swap (newRoot.children [index].faces   , this->root.data ()->faces);
    std::swap (newRoot.children [index].depth   , this->root.data ()->depth);
#ifdef DILAY_RENDER_OCTREE
    std::swap (newRoot.children [index].mesh    , this->root.data ()->mesh);
#endif
    this->root.data (newRoot);
  }

  void render () { 
#ifdef DILAY_RENDER_OCTREE
    if (this->root.isDefined ()) this->root.data ()->render ();
#else
    assert (false && "compiled without rendering support for octrees");
#endif
  }
};

DELEGATE_BIG4 (Octree)
DELEGATE2     (LinkedFace, Octree, insertFace, const WingedFace&, const Triangle&)
DELEGATE      (void, Octree, render)

/** Iterates over all faces of an octree node */
struct OctreeNodeIteratorImpl {
  OctreeNodeImpl& octreeNode;
  LinkedFace      _face;

  OctreeNodeIteratorImpl (OctreeNodeImpl& n) 
    : octreeNode (n) 
    , _face      (n.faces.end ())
  {}

  bool hasFace () const { 
    return this->_face != this->octreeNode.faces.end ();
  }

  LinkedFace face () const {
    assert (this->hasFace ());
    return this->_face;
  }

  void next () {
    assert (this->hasFace ());
    this->_face++;
  }
};

/** Iterates over all faces of an octree */
struct OctreeIteratorImpl {
  typedef std::list <OctreeNodeIteratorImpl> NodeIterators;

  NodeIterators nodeIterators;

  OctreeIteratorImpl (Octree& octree) {
    if (octree.impl->root.isDefined ()) {
      this->nodeIterators.push_back (
          OctreeNodeIteratorImpl (*octree.impl->root.data ())
      );
    }
  }

  bool hasFace () const { return this->nodeIterators.size () > 0; }

  LinkedFace face () const {
    assert (this->hasFace ());
    return this->nodeIterators.begin ()->face ();
  }

  void next () { 
    std::function <void ()>  check = [this,&check] () {
      OctreeNodeIteratorImpl octreeNodeIterator = *this->nodeIterators.begin ();

      if (! octreeNodeIterator.hasFace ()) {

        for (OctreeNodeImpl& c : octreeNodeIterator.octreeNode.children) {
          this->nodeIterators.push_back (OctreeNodeIteratorImpl (c));
        }
        this->nodeIterators.pop_front ();
        check ();
      }
    };

    assert (this->hasFace ());
    this->nodeIterators.begin ()->next ();
    check ();
  }

  unsigned int depth () {
    assert (this->hasFace ());
    return this->nodeIterators.begin ()->octreeNode.depth;
  }
};

DELEGATE1_CONSTRUCTOR (OctreeIterator, Octree&)    
DELEGATE_CONST        (bool          , OctreeIterator, hasFace)
DELEGATE_CONST        (LinkedFace    , OctreeIterator, face)
DELEGATE              (void          , OctreeIterator, next)
DELEGATE_CONST        (unsigned int  , OctreeIterator, depth)
