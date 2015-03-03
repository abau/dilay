#include <glm/glm.hpp>
#include <QSpinBox>
#include "action/new-winged-mesh.hpp"
#include "cache.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"
#include "util.hpp"

struct ToolNewWingedMesh::Impl {
  ToolNewWingedMesh* self;
  QSpinBox&          subdivEdit;
  MeshDefinition     definition;
  Mesh               mesh;

  Impl (ToolNewWingedMesh* s) 
    : self (s) 
    , subdivEdit (ViewUtil::spinBox ( 1
                                    , this->self->cache ().get <int> ("subdiv", 3)
                                    , 5 ))
  {
    this->self->properties ().body ().add (QObject::tr ("Subdivisions"), this->subdivEdit);

    ViewUtil::connect (this->subdivEdit, [this] (int) {
        this->updateMesh           ();
        this->self->updateGlWidget ();
    });
    this->updateMesh ();
  }

  void updateMesh () {
    const int numSubdiv = this->subdivEdit.value ();

    this->self->cache ().set ("subdiv", numSubdiv);

    this->definition = MeshDefinition::Icosphere (numSubdiv);
    this->definition.scale (glm::vec3 (Util::defaultScale ()));

    this->mesh = Mesh (this->definition);
    this->mesh.bufferData ();

    this->mesh.renderMode ().smoothShading   (true);
    this->mesh.renderMode ().renderWireframe (true);

    this->mesh.color          (this->self->config ().get <Color> ("editor/poly-mesh/color/normal"));
    this->mesh.wireframeColor (this->self->config ().get <Color> ("editor/poly-mesh/color/wireframe"));
  }

  void runRender () const {
    this->mesh.render (this->self->state ().camera ());
  }

  void runClose () {
    this->self->state ().history ().add <ActionNewWingedMesh> ()
                        .run (this->self->state ().scene (), this->definition);
  }
};

DELEGATE_TOOL            (ToolNewWingedMesh)
DELEGATE_TOOL_RUN_CLOSE  (ToolNewWingedMesh)
DELEGATE_TOOL_RUN_RENDER (ToolNewWingedMesh)
