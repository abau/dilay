#ifndef DILAY_VIEW_PROPERTIES_MOVEMENT
#define DILAY_VIEW_PROPERTIES_MOVEMENT

#include "view/properties.hpp"
#include "macro.hpp"

class ViewPropertiesMovement : public ViewProperties {
  public:
    DECLARE_BIG3 (ViewPropertiesMovement)

    bool xy () const;
    bool yz () const;
    bool xz () const;
    bool x  () const;
    bool y  () const;
    bool z  () const;

  private:
    class Impl;
    Impl* impl;
};

#endif
