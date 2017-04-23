/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"

struct ToolNewMesh::Impl
{
  ToolNewMesh* self;

  Impl (ToolNewMesh* s)
    : self (s)
  {
  }

  ToolResponse runInitialize () const
  {
    this->self->snapshotDynamicMeshes ();
    this->self->state ().scene ().newDynamicMesh (this->self->state ().config (),
                                                  MeshUtil::icosphere (3));
    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL (ToolNewMesh)
DELEGATE_TOOL_RUN_INITIALIZE (ToolNewMesh)
