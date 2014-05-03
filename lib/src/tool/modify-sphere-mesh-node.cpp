#include <QString>
#include <QObject>
#include "tool/modify-sphere-mesh-node.hpp"

struct ToolModifySphereMeshNode::Impl {
  ToolModifySphereMeshNode* self;

  Impl (ToolModifySphereMeshNode* s, Mode) : self (s) {
  }

  ~Impl () {
  }

  static QString toolName (ToolModifySphereMeshNode::Mode mode) {
    switch (mode) {
      case ToolModifySphereMeshNode::Mode::NewMesh:
        return QObject::tr ("New Mesh");
      case ToolModifySphereMeshNode::Mode::NewNode:
        return QObject::tr ("New Node");
      case ToolModifySphereMeshNode::Mode::ModifyNode:
        return QObject::tr ("Modify Node");
    }
    assert (false);
  }
};

DELEGATE1_TOOL  (ToolModifySphereMeshNode, Mode)
