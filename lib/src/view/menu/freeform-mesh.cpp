#include "mesh-type.hpp"
#include "tools.hpp"
#include "view/menu/freeform-mesh.hpp"

ViewMenuFreeformMesh :: ViewMenuFreeformMesh (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolMove>            ();
  this->addAction <ToolCarve>           ();
  this->addAction <ToolSubdivideMesh>   ();
  this->addAction <ToolDeleteMesh>      (MeshType::Freeform);
  this->addSeparator                    ();
  this->addAction <ToolNewFreeformMesh> ();
}
