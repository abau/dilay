#include "selection/selection.hpp"

#ifndef VERTEX_SELECTION
#define VERTEX_SELECTION

class VertexSelection : public Selection {
  public:
    Selection     (const VertexSelection&);

    void reset    ();
    void fromRay  (const Ray&, bool);

  private:
    std::list<LinkedVertex&> vertices;
};

#endif
