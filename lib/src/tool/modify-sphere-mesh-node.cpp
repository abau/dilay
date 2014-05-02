#include <QString>
#include <QObject>
#include "tool/modify-sphere-mesh-node.hpp"

struct ToolModifySphereMeshNode::Impl {
  ToolModifySphereMeshNode* self;

  Impl (ToolModifySphereMeshNode* s, Mode) : self (s) {
  }

  ~Impl () {
  }

  static QString toolName () {
    return QObject::tr ("Modify");
  }
};

DELEGATE1_TOOL  (ToolModifySphereMeshNode, Mode)
