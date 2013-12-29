#ifndef DILAY_ACTION_WINGED_MESH_UNIT
#define DILAY_ACTION_WINGED_MESH_UNIT

#include "action.hpp"

class WMeshUnit : public Action {

  public: ActionUnit            ();
          ActionUnit            (const ActionUnit&) = delete;
    const ActionUnit& operator= (const ActionUnit&) = delete;
         ~ActionUnit            ();

};
#endif
