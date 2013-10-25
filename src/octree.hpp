#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include <list>
#include <unordered_set>
#include "fwd-glm.hpp"
#include "id.hpp"
#include "iterator.hpp"
#include "macro.hpp"

class WingedFace;
class WingedVertex;
class WingedMesh;
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
class Sphere;

/** Internal template for iterators over all faces of a node */
template <bool> struct OctreeNodeFaceIteratorTemplate;

/** Internal template for iterators over all faces of an octree */
template <bool> struct OctreeFaceIteratorTemplate;

/** Internal template for iterators over all nodes of an octree */
template <bool> struct OctreeNodeIteratorTemplate;

/** Octree node interface */
class OctreeNode {
  public: 
    class Impl;

          OctreeNode                  (Impl*);
          OctreeNode                  (const OctreeNode&) = delete;
    const OctreeNode& operator=       (const OctreeNode&) = delete;

    Id                id              () const;
    int               depth           () const;
    const glm::vec3&  center          () const;
    float             looseWidth      () const;
    float             width           () const;
    void              intersectRay    (const WingedMesh&, const Ray&, FaceIntersection&);
    void              intersectSphere ( const WingedMesh&, const Sphere&
                                      , std::list<WingedFace*>&);
    void              intersectSphere ( const WingedMesh&, const Sphere&
                                      , std::unordered_set<WingedVertex*>&);
    unsigned int      numFaces        () const;
    OctreeNode*       nodeSLOW        (const Id&);

    OctreeNodeFaceIterator      faceIterator ();
    ConstOctreeNodeFaceIterator faceIterator () const;

  private:
    friend class Octree;
    Impl* impl;
};

/** Octree main class */
class Octree { 
  public: 
    class Impl; 
    
          Octree                ();
          Octree                (const Octree&) = delete;
    const Octree& operator=     (const Octree&) = delete;
         ~Octree                ();

    WingedFace& insertFace      (const WingedFace&, const Triangle&);
    WingedFace& realignFace     (const WingedFace&, const Triangle&);
    void        deleteFace      (const WingedFace&);
    bool        hasFace         (const Id&) const;
    WingedFace* face            (const Id&) const;
    void        render          ();
    void        intersectRay    (const WingedMesh&, const Ray&, FaceIntersection&);
    void        intersectSphere ( const WingedMesh&, const Sphere&
                                , std::list<WingedFace*>&);
    void        intersectSphere ( const WingedMesh&, const Sphere&
                                , std::unordered_set<WingedVertex*>&);
    void        reset           ();
    void        reset           (const glm::vec3&, float);
    OctreeNode& nodeSLOW        (const Id&);

    OctreeFaceIterator      faceIterator ();
    ConstOctreeFaceIterator faceIterator () const;
    OctreeNodeIterator      nodeIterator ();
    ConstOctreeNodeIterator nodeIterator () const;

    SAFE_REF1 (WingedFace,face,const Id&)

  private:
    Impl* impl;
};

/** Iterator over all faces of a node */
class OctreeNodeFaceIterator : public Iterator <WingedFace> {
  public: OctreeNodeFaceIterator            (OctreeNode::Impl&);
          OctreeNodeFaceIterator            (const OctreeNodeFaceIterator&);
    const OctreeNodeFaceIterator& operator= (const OctreeNodeFaceIterator&);
         ~OctreeNodeFaceIterator            ();

    bool         isValid () const;
    WingedFace&  element () const;
    void         next    ();
    int          depth   () const;

    using Impl = OctreeNodeFaceIteratorTemplate <false>;
  private:
    Impl* impl;
};

/** Constant iterator over all faces of a node */
class ConstOctreeNodeFaceIterator : public ConstIterator <WingedFace> {
  public: ConstOctreeNodeFaceIterator            (const OctreeNode::Impl&);
          ConstOctreeNodeFaceIterator            (const ConstOctreeNodeFaceIterator&);
    const ConstOctreeNodeFaceIterator& operator= (const ConstOctreeNodeFaceIterator&);
         ~ConstOctreeNodeFaceIterator            ();

    bool              isValid () const;
    const WingedFace& element () const;
    void              next    ();
    int               depth   () const;

    using Impl = OctreeNodeFaceIteratorTemplate <true>;
  private:
    Impl* impl;
};

/** Iterator over all faces of an octree */
class OctreeFaceIterator : public Iterator <WingedFace> {
  public: OctreeFaceIterator            (Octree::Impl&);
          OctreeFaceIterator            (const OctreeFaceIterator&);
    const OctreeFaceIterator& operator= (const OctreeFaceIterator&);
         ~OctreeFaceIterator            ();

    bool         isValid () const;
    WingedFace&  element () const;
    void         next    ();
    int          depth   () const;

  private:
    using Impl = OctreeFaceIteratorTemplate <false>;
    Impl* impl;
};

/** Constant iterator over all faces of an octree */
class ConstOctreeFaceIterator : public ConstIterator <WingedFace> {
  public: ConstOctreeFaceIterator            (const Octree::Impl&);
          ConstOctreeFaceIterator            (const ConstOctreeFaceIterator&);
    const ConstOctreeFaceIterator& operator= (const ConstOctreeFaceIterator&);
         ~ConstOctreeFaceIterator            ();
     
    bool              isValid () const;
    const WingedFace& element () const;
    void              next    ();
    int               depth   () const;

  private:
    using Impl = OctreeFaceIteratorTemplate <true>;
    Impl* impl;
};

/** Iterator over all nodes of an octree */
class OctreeNodeIterator : public Iterator <OctreeNode> {
  public: OctreeNodeIterator            (Octree::Impl&);
          OctreeNodeIterator            (const OctreeNodeIterator&);
    const OctreeNodeIterator& operator= (const OctreeNodeIterator&);
         ~OctreeNodeIterator            ();

    bool         isValid () const;
    OctreeNode&  element () const;
    void         next    ();

  private:
    using Impl = OctreeNodeIteratorTemplate <false>;
    Impl* impl;
};

/** Constant iterator over all nodes of an octree */
class ConstOctreeNodeIterator : public ConstIterator <OctreeNode> {
  public: ConstOctreeNodeIterator            (const Octree::Impl&);
          ConstOctreeNodeIterator            (const ConstOctreeNodeIterator&);
    const ConstOctreeNodeIterator& operator= (const ConstOctreeNodeIterator&);
         ~ConstOctreeNodeIterator            ();

    bool              isValid () const;
    const OctreeNode& element () const;
    void              next    ();

  private:
    using Impl = OctreeNodeIteratorTemplate <true>;
    Impl* impl;
};

#endif
