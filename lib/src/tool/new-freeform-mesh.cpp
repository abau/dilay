#include <QObject>
#include "tool/new-freeform-mesh.hpp"

#include <iostream>

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;

  Impl (ToolNewFreeformMesh* s) : self (s) {}

  ~Impl () {  std::cout << "ctor\n"; }

  static QString toolName () {
    return QObject::tr ("New Freeform Mesh");
  }

  ToolResponse runExecute () {
    std::cout << "narf\n";
    return ToolResponse::None;
  }
};

DELEGATE_BIG3_BASE ( ToolNewFreeformMesh, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolNewFreeformMesh, toolName)
DELEGATE        (ToolResponse, ToolNewFreeformMesh, runExecute)
