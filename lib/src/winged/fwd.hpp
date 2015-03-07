#ifndef DILAY_WINGED_FWD
#define DILAY_WINGED_FWD

#include <list>
#include <unordered_set>
#include <vector>

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;

typedef std::unordered_set <WingedMesh*>   MeshPtrSet;
typedef std::unordered_set <WingedFace*>   FacePtrSet;
typedef std::unordered_set <WingedEdge*>   EdgePtrSet;
typedef std::unordered_set <WingedVertex*> VertexPtrSet;
typedef std::vector        <WingedMesh*>   MeshPtrVec;
typedef std::vector        <WingedFace*>   FacePtrVec;
typedef std::vector        <WingedEdge*>   EdgePtrVec;
typedef std::vector        <WingedVertex*> VertexPtrVec;
typedef std::list          <WingedMesh*>   MeshPtrList;
typedef std::list          <WingedFace*>   FacePtrList;
typedef std::list          <WingedEdge*>   EdgePtrList;
typedef std::list          <WingedVertex*> VertexPtrList;

#endif
