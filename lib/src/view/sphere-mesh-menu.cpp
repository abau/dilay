#include "view/sphere-mesh-menu.hpp"
#include "tool/modify-sphere-mesh-node.hpp"

ViewSphereMeshMenu :: ViewSphereMeshMenu (ViewMainWindow& mW, QContextMenuEvent& mE)
  : ViewToolMenu (mW,mE) 
{
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::NewMesh);
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::NewNode);
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::ModifyNode);
}
