#ifndef DILAY_FWD_WINGED
#define DILAY_FWD_WINGED

#include <list>

class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;

typedef std::list<WingedVertex>::iterator               LinkedVertex;
typedef std::list<WingedEdge>  ::iterator               LinkedEdge;
typedef std::list<WingedFace>  ::iterator               LinkedFace;

typedef std::list<WingedVertex>::const_iterator         ConstLinkedVertex;
typedef std::list<WingedEdge>  ::const_iterator         ConstLinkedEdge;
typedef std::list<WingedFace>  ::const_iterator         ConstLinkedFace;

#endif
