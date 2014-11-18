#include <QObject>
#include "action/delete-winged-mesh.hpp"
#include "action/unit.hpp"
#include "history.hpp"
#include "mesh-type.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/delete-mesh.hpp"

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
    }

    if (unit.isEmpty () == false) {
      State::history ().addAction (unit);
      State::scene   ().unselectAll ();
    }
    return ToolResponse::Terminate;
  }

  void runDeleteWingedMesh (ActionUnit& unit) {
    State::scene ().selection ().forEachMajor ([this,&unit]
      (unsigned int index) {
        WingedMesh& mesh = State::scene ().wingedMeshRef (index);
        unit.add <ActionDeleteWMesh> ().deleteMesh (this->meshType, mesh);
      }
    );
  }
};

DELEGATE_BIG3_BASE ( ToolDeleteMesh, (const ViewToolMenuParameters& p, MeshType t)
                   , (this,t), Tool, (p) )
DELEGATE1_STATIC (QString     , ToolDeleteMesh, toolName, MeshType)
DELEGATE         (ToolResponse, ToolDeleteMesh, runInitialize)
