#include "tools.hpp"
#include "view/menu/winged-mesh.hpp"

ViewMenuWingedMesh :: ViewMenuWingedMesh (State& s, const glm::ivec2& p) 
  : ViewToolMenu (s, p) 
{
  this->addAction <ToolMove>          (QObject::tr ("Move"));
  this->addAction <ToolCarve>         (QObject::tr ("Carve"));
  this->addAction <ToolSubdivideMesh> (QObject::tr ("Subdivide"));
  this->addAction <ToolDeleteMesh>    (QObject::tr ("Delete"));
  this->addSeparator                  ();
  this->addAction <ToolNewWingedMesh> (QObject::tr ("New"));
}
