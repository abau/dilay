#include "tools.hpp"
#include "view/menu/winged-mesh.hpp"

ViewMenuWingedMesh :: ViewMenuWingedMesh (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolMove>          ();
  this->addAction <ToolCarve>         ();
  this->addAction <ToolSubdivideMesh> ();
  this->addAction <ToolDeleteMesh>    ();
  this->addSeparator                  ();
  this->addAction <ToolNewWingedMesh> ();
}
