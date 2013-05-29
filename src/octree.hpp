#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include "fwd-winged.hpp"
#include "fwd-glm.hpp"

class Triangle;
class OctreeImpl;
class OctreeNode;
class OctreeNodeImpl;
class OctreeFaceIterator;
class ConstOctreeFaceIterator;
class OctreeNodeIterator;
class ConstOctreeNodeIterator;
class Ray;
class FaceIntersection;

/** Internal template for iterators over all faces of a node */
template <class,class> struct OctreeNodeFaceIteratorTemplate;

/** Internal template for iterators over all faces of an octree */
template <class,class,class,class> struct OctreeFaceIteratorTemplate;

/** Internal template for iterators over all nodes of an octree */
template <class,class,class> struct OctreeNodeIteratorTemplate;

/** Specializations of non constant iterators */
using OctreeNodeFaceIteratorImpl = OctreeNodeFaceIteratorTemplate 
                                   <OctreeNodeImpl, LinkedFace>;

using OctreeFaceIteratorImpl = OctreeFaceIteratorTemplate 
                               < OctreeImpl, OctreeNodeImpl
                               , OctreeNodeFaceIteratorImpl, LinkedFace>;

using OctreeNodeIteratorImpl = OctreeNodeIteratorTemplate
                             < OctreeImpl, OctreeNode, OctreeNodeImpl >;

/** Specializations of constant iterators */
using ConstOctreeNodeFaceIteratorImpl = OctreeNodeFaceIteratorTemplate 
                                        <const OctreeNodeImpl, ConstLinkedFace>;

using ConstOctreeFaceIteratorImpl = OctreeFaceIteratorTemplate 
                                  < const OctreeImpl, const OctreeNodeImpl
                                  , ConstOctreeNodeFaceIteratorImpl, ConstLinkedFace>;

using ConstOctreeNodeIteratorImpl = OctreeNodeIteratorTemplate
                                  < const OctreeImpl, const OctreeNode
                                  , const OctreeNodeImpl >;

/** Octree node interface */
class OctreeNode {
  public: OctreeNode               (OctreeNodeImpl*);
          OctreeNode               (const OctreeNode&) = delete;
    const OctreeNode& operator=    (const OctreeNode&) = delete;

    void              deleteFace   (LinkedFace);
    void              intersectRay (const WingedMesh&, const Ray&, FaceIntersection&);
    unsigned int      numFaces     () const;
    int               depth        () const;
    const glm::vec3&  center       () const;
    float             width        () const;

  private:
    OctreeNodeImpl* impl;
};

/** Octree main class */
class Octree { 
  public: Octree            ();
          Octree            (const Octree&) = delete;
    const Octree& operator= (const Octree&) = delete;
         ~Octree            ();

    LinkedFace  insertFace   (const WingedFace&, const Triangle&);
    void        render       ();
    void        intersectRay (const WingedMesh&, const Ray&, FaceIntersection&);
    void        reset        ();
    void        reset        (const glm::vec3&, float);

    OctreeFaceIterator      faceIterator ();
    ConstOctreeFaceIterator faceIterator () const;
    OctreeNodeIterator      nodeIterator ();
    ConstOctreeNodeIterator nodeIterator () const;

  private:
    OctreeImpl* impl;
};

/** Iterator over all faces of an octree */
class OctreeFaceIterator {
  public: OctreeFaceIterator            (OctreeImpl&);
          OctreeFaceIterator            (const OctreeFaceIterator&);
    const OctreeFaceIterator& operator= (const OctreeFaceIterator&);
         ~OctreeFaceIterator            ();

    bool         hasFace            () const;
    LinkedFace   face               () const;
    void         next               ();
    int          depth              () const;

  private:
    OctreeFaceIteratorImpl* impl;
};

/** Constant iterator over all faces of an octree */
class ConstOctreeFaceIterator {
  public: ConstOctreeFaceIterator            (const OctreeImpl&);
          ConstOctreeFaceIterator            (const ConstOctreeFaceIterator&);
    const ConstOctreeFaceIterator& operator= (const ConstOctreeFaceIterator&);
         ~ConstOctreeFaceIterator            ();
     
    bool            hasFace            () const;
    ConstLinkedFace face               () const;
    void            next               ();
    int             depth              () const;

  private:
    ConstOctreeFaceIteratorImpl* impl;
};

/** Iterator over all nodes of an octree */
class OctreeNodeIterator {
  public: OctreeNodeIterator            (OctreeImpl&);
          OctreeNodeIterator            (const OctreeNodeIterator&);
    const OctreeNodeIterator& operator= (const OctreeNodeIterator&);
         ~OctreeNodeIterator            ();

    bool         hasNode () const;
    OctreeNode&  node    () const;
    void         next    ();

  private:
    OctreeNodeIteratorImpl* impl;
};

/** Constant iterator over all nodes of an octree */
class ConstOctreeNodeIterator {
  public: ConstOctreeNodeIterator            (const OctreeImpl&);
          ConstOctreeNodeIterator            (const ConstOctreeNodeIterator&);
    const ConstOctreeNodeIterator& operator= (const ConstOctreeNodeIterator&);
         ~ConstOctreeNodeIterator            ();

    bool              hasNode () const;
    const OctreeNode& node    () const;
    void              next    ();

  private:
    ConstOctreeNodeIteratorImpl* impl;
};
#endif
