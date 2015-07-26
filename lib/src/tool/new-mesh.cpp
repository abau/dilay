/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"

struct ToolNewMesh::Impl {
  ToolNewMesh* self;

  Impl (ToolNewMesh* s) : self (s) {}

  ToolResponse runInitialize () const {
    this->self->snapshotScene ();
    this->self->state ().scene ().newWingedMesh ( this->self->state ().config ()
                                                , MeshUtil::icosphere (3) );
    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL                 (ToolNewMesh)
DELEGATE_TOOL_RUN_INITIALIZE  (ToolNewMesh)
