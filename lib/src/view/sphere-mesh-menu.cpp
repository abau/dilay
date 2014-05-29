#include "view/sphere-mesh-menu.hpp"
//#include "tool/modify-sphere-mesh-node.hpp"

ViewSphereMeshMenu :: ViewSphereMeshMenu (ViewMainWindow& mW, QContextMenuEvent& mE)
  : ViewToolMenu (mW,mE) 
{
 // this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::NewMesh);
}
