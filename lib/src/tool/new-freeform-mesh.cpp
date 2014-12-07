#include <QSpinBox>
#include "action/new-winged-mesh.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/util.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  QSpinBox&            subdivEdit;
  MeshDefinition       definition;
  Mesh                 mesh;

  Impl (ToolNewFreeformMesh* s) 
    : self (s) 
    , subdivEdit (ViewUtil::spinBox ( 1
                                    , Config::get <int> ("/cache/tool/new-freeform-mesh/subdiv", 3)
                                    , 5 ))
  {
    this->self->properties ().addWidget (QObject::tr ("Subdivisions"), this->subdivEdit);

    ViewUtil::connect (this->subdivEdit, [this] (int) {
        this->updateMesh           ();
        this->self->updateGlWidget ();
    });
    this->updateMesh ();
  }

  void updateMesh () {
    const int numSubdiv = this->subdivEdit.value ();
    this->definition    = MeshDefinition::icosphere (numSubdiv);
    this->mesh          = Mesh (this->definition);
    this->mesh.bufferData ();
    Config::cache <int>   ("/cache/tool/new-freeform-mesh/subdiv", numSubdiv);
  }

  void runRender () {
    this->mesh.render ();
  }

  static QString toolName () {
    return QObject::tr ("New Freeform Mesh");
  }

  void runClose () {
    State::history ().add <ActionNewWingedMesh> ().run (MeshType::Freeform, this->definition);
  }
};

DELEGATE_TOOL            (ToolNewFreeformMesh)
DELEGATE_TOOL_RUN_CLOSE  (ToolNewFreeformMesh)
DELEGATE_TOOL_RUN_RENDER (ToolNewFreeformMesh)
