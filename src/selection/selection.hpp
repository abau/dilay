#include "ray.hpp"

#ifndef SELECTION
#define SELECTION

class VertexSelection;
//class EdgeSeletion;
//class MeshSelection;

class Selection {
  public:
    Selection     (const VertexSelection&) = 0;
    //Selection     (const EdgeSelection&)   = 0;
    //Selection     (const MeshSelection&)   = 0;

    void reset    ()                       = 0;
    void fromRay  (const Ray&, bool)       = 0;
};

#endif
