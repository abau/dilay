#ifndef DILAY_FWD_WINGED
#define DILAY_FWD_WINGED

#include <list>
#include <unordered_set>
#include <memory>

class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;

typedef std::list <WingedVertex>                 Vertices;
typedef std::list <WingedEdge>                   Edges;
typedef std::list <WingedFace>                   Faces;

typedef Vertices::iterator                       LinkedVertex;
typedef Edges   ::iterator                       LinkedEdge;
typedef Faces   ::iterator                       LinkedFace;

typedef Vertices::const_iterator                 ConstLinkedVertex;
typedef Edges   ::const_iterator                 ConstLinkedEdge;
typedef Faces   ::const_iterator                 ConstLinkedFace;

typedef std::list <LinkedVertex>                 VertexList;
typedef std::list <LinkedEdge>                   EdgeList;
typedef std::list <LinkedFace>                   FaceList;

typedef std::unordered_set <LinkedVertex>        VertexSet;
typedef std::unordered_set <LinkedEdge>          EdgeSet;
typedef std::unordered_set <LinkedFace>          FaceSet;

typedef std::unique_ptr <LinkedVertex>           VertexPtr;
typedef std::unique_ptr <LinkedEdge>             EdgePtr;
typedef std::unique_ptr <LinkedFace>             FacePtr;

#endif
