#include <QObject>
#include "action/subdivide-mesh.hpp"
#include "action/unit.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/subdivide-mesh.hpp"
#include "winged/mesh.hpp"

struct ToolSubdivideMesh::Impl {
  ToolSubdivideMesh* self;

  Impl (ToolSubdivideMesh* s) : self (s) {}

  static QString toolName () {
    return QObject::tr ("Subdivide Mesh");
  }

  ToolResponse runInitialize () {
    ActionUnit unit;

    State::scene ().selection ().forEachMajor ([&unit] (unsigned int index) {
      WingedMesh& mesh = State::scene ().wingedMeshRef (index);
      unit.add <ActionSubdivideMesh> (mesh).run (mesh);
    });

    if (unit.isEmpty () == false) {
      State::history ().addUnit (std::move (unit));
    }
    return ToolResponse::Terminate;
  }
};

DELEGATE_BIG3_BASE ( ToolSubdivideMesh, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p) )
DELEGATE_STATIC (QString     , ToolSubdivideMesh, toolName)
DELEGATE        (ToolResponse, ToolSubdivideMesh, runInitialize)
