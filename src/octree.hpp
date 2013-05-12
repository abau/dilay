#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include "fwd-winged.hpp"

class Triangle;
class OctreeImpl;
class OctreeNode;
class OctreeNodeImpl;
class OctreeFaceIterator;
class ConstOctreeFaceIterator;
class OctreeNodeIterator;
class ConstOctreeNodeIterator;

/** Internal template for iterators over all faces of a node */
template <class,class> struct OctreeNodeFaceIteratorTemplate;

/** Internal template for iterators over all faces of an octree */
template <class,class,class,class> struct OctreeFaceIteratorTemplate;

// /** Internal template for iterators over all nodes of an octree */
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
  public: OctreeNode            (OctreeNodeImpl*);
          OctreeNode            (const OctreeNode&);
    const OctreeNode& operator= (const OctreeNode&);
         ~OctreeNode            ();

    void  deleteFace            (LinkedFace);

  private:
    OctreeNodeImpl* impl;
};

/** Octree main class */
class Octree { 
  public: Octree            ();
          Octree            (const Octree&);
    const Octree& operator= (const Octree&);
         ~Octree            ();

    LinkedFace              insertFace   (const WingedFace&, const Triangle&);
    void                    render       ();
    OctreeFaceIterator      faceIterator ();
    ConstOctreeFaceIterator faceIterator () const;
    OctreeNodeIterator      nodeIterator ();
    ConstOctreeNodeIterator nodeIterator () const;

  private:
    OctreeImpl* impl;
};

class OctreeFaceIterator {
  public: OctreeFaceIterator            (OctreeImpl&);
          OctreeFaceIterator            (const OctreeFaceIterator&);
    const OctreeFaceIterator& operator= (const OctreeFaceIterator&);
         ~OctreeFaceIterator            ();

    bool         hasFace            () const;
    LinkedFace   face               () const;
    void         next               ();
    unsigned int depth              () const;

  private:
    OctreeFaceIteratorImpl* impl;
};

class ConstOctreeFaceIterator {
  public: ConstOctreeFaceIterator            (const OctreeImpl&);
          ConstOctreeFaceIterator            (const ConstOctreeFaceIterator&);
    const ConstOctreeFaceIterator& operator= (const ConstOctreeFaceIterator&);
         ~ConstOctreeFaceIterator            ();
     
    bool            hasFace            () const;
    ConstLinkedFace face               () const;
    void            next               ();
    unsigned int    depth              () const;

  private:
    ConstOctreeFaceIteratorImpl* impl;
};

class OctreeNodeIterator {
  public: OctreeNodeIterator            (OctreeImpl&);
          OctreeNodeIterator            (const OctreeNodeIterator&);
    const OctreeNodeIterator& operator= (const OctreeNodeIterator&);
         ~OctreeNodeIterator            ();

    bool         hasNode () const;
    OctreeNode&  node    () const;
    void         next    ();
    unsigned int depth   () const;

  private:
    OctreeNodeIteratorImpl* impl;
};

class ConstOctreeNodeIterator {
  public: ConstOctreeNodeIterator            (const OctreeImpl&);
          ConstOctreeNodeIterator            (const ConstOctreeNodeIterator&);
    const ConstOctreeNodeIterator& operator= (const ConstOctreeNodeIterator&);
         ~ConstOctreeNodeIterator            ();

    bool              hasNode () const;
    const OctreeNode& node    () const;
    void              next    ();
    unsigned int      depth   () const;

  private:
    ConstOctreeNodeIteratorImpl* impl;
};

#endif
