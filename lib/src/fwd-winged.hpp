#ifndef DILAY_FWD_WINGED
#define DILAY_FWD_WINGED

#include <list>
#include <unordered_set>
#include <vector>

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;

typedef std::list          <WingedFace>    Faces;
typedef std::list          <WingedEdge>    Edges;
typedef std::list          <WingedVertex>  Vertices;

typedef std::unordered_set <WingedFace*>   FaceSet;
typedef std::unordered_set <WingedEdge*>   EdgeSet;
typedef std::unordered_set <WingedVertex*> VertexSet;
typedef std::vector        <WingedFace*>   FaceVec;
typedef std::vector        <WingedEdge*>   EdgeVec;
typedef std::vector        <WingedVertex*> VertexVec;

#endif
