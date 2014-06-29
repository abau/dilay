#include <QObject>
#include "tool/delete-mesh.hpp"
#include "mesh-type.hpp"
#include "action/unit.hpp"
#include "action/delete-winged-mesh.hpp"
#include "action/delete-sphere-mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "selection.hpp"

struct ToolDeleteMesh::Impl {
  ToolDeleteMesh* self;
  MeshType        meshType;

  Impl (ToolDeleteMesh* s, MeshType t) : self (s), meshType (t) {}

  static QString toolName (MeshType) {
    return QObject::tr ("Delete Mesh");
  }

  ToolResponse runInitialize () {
    ActionUnit& unit = *new ActionUnit ();

    switch (this->meshType) {
      case MeshType::Freeform:
        this->runDeleteWingedMesh (unit);
        break;

      case MeshType::Sphere:
        this->runDeleteSphereMesh (unit);
        break;
    }

    if (unit.isEmpty () == false) {
      State::history ().addAction (unit);
      State::scene   ().unselectAll ();
    }
    return ToolResponse::Terminate;
  }

  void runDeleteWingedMesh (ActionUnit& unit) {
    State::scene ().selection ().forEachMajor ([this,&unit]
      (const Id& id) {
        WingedMesh& mesh = State::scene ().wingedMesh (id);
        unit.add <ActionDeleteWMesh> ().deleteMesh (this->meshType, mesh);
      }
    );
  }

  void runDeleteSphereMesh (ActionUnit& unit) {
    State::scene ().selection ().forEachMajor ([&unit]
      (const Id& id) {
        SphereMesh& mesh = State::scene ().sphereMesh (id);
        unit.add <ActionDeleteSMesh> ().deleteMesh (mesh);
      }
    );
  }
};

DELEGATE_BIG3_BASE ( ToolDeleteMesh, (const ViewToolMenuParameters& p, MeshType t)
                   , (this,t), Tool, (p) )
DELEGATE1_STATIC (QString     , ToolDeleteMesh, toolName, MeshType)
DELEGATE         (ToolResponse, ToolDeleteMesh, runInitialize)
