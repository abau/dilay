#include <QObject>
#include <glm/glm.hpp>
#include "action/new-sphere-mesh.hpp"
#include "history.hpp"
#include "state.hpp"
#include "tool/new-sphere-mesh.hpp"

struct ToolNewSphereMesh::Impl {
  ToolNewSphereMesh* self;

  Impl (ToolNewSphereMesh* s) : self (s) {}

  static QString toolName () {
    return QObject::tr ("New Sphere Mesh");
  }

  ToolResponse runInitialize () {
    State::history ().add <ActionNewSphereMesh> ().run (glm::vec3 (0.0f), 1.0f);
    return ToolResponse::Terminate;
  }
};

DELEGATE_BIG3_BASE ( ToolNewSphereMesh, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolNewSphereMesh, toolName)
DELEGATE        (ToolResponse, ToolNewSphereMesh, runInitialize)
