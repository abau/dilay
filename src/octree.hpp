#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include "fwd-winged.hpp"
#include "fwd-glm.hpp"
#include "id.hpp"
#include "iterator.hpp"

class Triangle;
class OctreeNode;
class OctreeNodeFaceIterator;
class ConstOctreeNodeFaceIterator;
class OctreeFaceIterator;
class ConstOctreeFaceIterator;
class OctreeNodeIterator;
class ConstOctreeNodeIterator;
class Ray;
class FaceIntersection;

/** Internal template for iterators over all faces of an octree */
template <class,class,class,class> struct OctreeFaceIteratorTemplate;

/** Internal template for iterators over all nodes of an octree */
template <class,class,class> struct OctreeNodeIteratorTemplate;

/** Internal template for iterators over all faces of a node */
template <class,class> struct OctreeNodeFaceIteratorTemplate;

/** Reference to a face by its id and the enclosing node's id */
class OctreeNodeFaceReference {
  public:
    OctreeNodeFaceReference (IdType n, IdType f) : _nodeId (n), _faceId (f) {}

    IdType nodeId () const { return this->_nodeId; }
    IdType faceId () const { return this->_faceId; }

  private:
    const IdType _nodeId;
    const IdType _faceId;
};

/** Octree node interface */
class OctreeNode {
  public: 
    class Impl;

          OctreeNode               (Impl*);
          OctreeNode               (const OctreeNode&) = delete;
    const OctreeNode& operator=    (const OctreeNode&) = delete;

    IdType            id           () const;
    int               depth        () const;
    const glm::vec3&  center       () const;
    float             width        () const;
    void              deleteFace   (LinkedFace);
    void              intersectRay (const WingedMesh&, const Ray&, FaceIntersection&);
    unsigned int      numFaces     () const;
    LinkedFace        getFace      (const OctreeNodeFaceReference&);

    OctreeNodeFaceIterator      faceIterator ();
    ConstOctreeNodeFaceIterator faceIterator () const;

  private:
    Impl* impl;
};

/** Octree main class */
class Octree { 
  public: 
    class Impl; 
    
          Octree            ();
          Octree            (const Octree&) = delete;
    const Octree& operator= (const Octree&) = delete;
         ~Octree            ();

    LinkedFace  insertFace   (const WingedFace&, const Triangle&);
    LinkedFace  getFace      (const OctreeNodeFaceReference&);
    void        render       ();
    void        intersectRay (const WingedMesh&, const Ray&, FaceIntersection&);
    void        reset        ();
    void        reset        (const glm::vec3&, float);

    OctreeFaceIterator      faceIterator ();
    ConstOctreeFaceIterator faceIterator () const;
    OctreeNodeIterator      nodeIterator ();
    ConstOctreeNodeIterator nodeIterator () const;

  private:
    Impl* impl;
};

/** Iterator over all faces of a node */
class OctreeNodeFaceIterator : public Iterator <LinkedFace> {
  public: OctreeNodeFaceIterator            (OctreeNode::Impl&);
          OctreeNodeFaceIterator            (const OctreeNodeFaceIterator&);
    const OctreeNodeFaceIterator& operator= (const OctreeNodeFaceIterator&);
         ~OctreeNodeFaceIterator            ();

    bool         isValid () const;
    LinkedFace   element () const;
    void         next    ();
    int          depth   () const;

    using Impl = OctreeNodeFaceIteratorTemplate <OctreeNode::Impl, LinkedFace>;
  private:
    Impl* impl;
};

/** Constant iterator over all faces of a node */
class ConstOctreeNodeFaceIterator : public Iterator <ConstLinkedFace> {
  public: ConstOctreeNodeFaceIterator            (const OctreeNode::Impl&);
          ConstOctreeNodeFaceIterator            (const ConstOctreeNodeFaceIterator&);
    const ConstOctreeNodeFaceIterator& operator= (const ConstOctreeNodeFaceIterator&);
         ~ConstOctreeNodeFaceIterator            ();

    bool            isValid () const;
    ConstLinkedFace element () const;
    void            next    ();
    int             depth   () const;

    using Impl = OctreeNodeFaceIteratorTemplate < const OctreeNode::Impl
                                                , ConstLinkedFace>;
  private:
    Impl* impl;
};

/** Iterator over all faces of an octree */
class OctreeFaceIterator : public Iterator <LinkedFace> {
  public: OctreeFaceIterator            (Octree::Impl&);
          OctreeFaceIterator            (const OctreeFaceIterator&);
    const OctreeFaceIterator& operator= (const OctreeFaceIterator&);
         ~OctreeFaceIterator            ();

    bool         isValid () const;
    LinkedFace   element () const;
    void         next    ();
    int          depth   () const;

  private:
    using Impl = OctreeFaceIteratorTemplate < Octree::Impl, OctreeNode::Impl
                                            , OctreeNodeFaceIterator::Impl, LinkedFace>;
    Impl* impl;
};

/** Constant iterator over all faces of an octree */
class ConstOctreeFaceIterator : public Iterator <ConstLinkedFace> {
  public: ConstOctreeFaceIterator            (const Octree::Impl&);
          ConstOctreeFaceIterator            (const ConstOctreeFaceIterator&);
    const ConstOctreeFaceIterator& operator= (const ConstOctreeFaceIterator&);
         ~ConstOctreeFaceIterator            ();
     
    bool            isValid () const;
    ConstLinkedFace element () const;
    void            next    ();
    int             depth   () const;

  private:
    using Impl = OctreeFaceIteratorTemplate < const Octree::Impl, const OctreeNode::Impl
                                            , ConstOctreeNodeFaceIterator::Impl
                                            , ConstLinkedFace>;
    Impl* impl;
};

/** Iterator over all nodes of an octree */
class OctreeNodeIterator : public RefIterator <OctreeNode> {
  public: OctreeNodeIterator            (Octree::Impl&);
          OctreeNodeIterator            (const OctreeNodeIterator&);
    const OctreeNodeIterator& operator= (const OctreeNodeIterator&);
         ~OctreeNodeIterator            ();

    bool         isValid () const;
    OctreeNode&  element ();
    void         next    ();

  private:
    using Impl = OctreeNodeIteratorTemplate < Octree::Impl, OctreeNode
                                            , OctreeNode::Impl >;
    Impl* impl;
};

/** Constant iterator over all nodes of an octree */
class ConstOctreeNodeIterator : public ConstRefIterator <OctreeNode> {
  public: ConstOctreeNodeIterator            (const Octree::Impl&);
          ConstOctreeNodeIterator            (const ConstOctreeNodeIterator&);
    const ConstOctreeNodeIterator& operator= (const ConstOctreeNodeIterator&);
         ~ConstOctreeNodeIterator            ();

    bool              isValid () const;
    const OctreeNode& element () const;
    void              next    ();

  private:
    using Impl = OctreeNodeIteratorTemplate < const Octree::Impl, const OctreeNode
                                            , const OctreeNode::Impl >;
    Impl* impl;
};

#endif
