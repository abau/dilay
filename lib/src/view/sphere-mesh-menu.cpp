#include "view/sphere-mesh-menu.hpp"
//#include "tool/modify-sphere-mesh-node.hpp"

ViewSphereMeshMenu :: ViewSphereMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  /*
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::NewMesh);
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::NewNode);
  this->addAction <ToolModifySphereMeshNode> (ToolModifySphereMeshNode::Mode::ModifyNode);
  */
}
