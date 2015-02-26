#ifndef DILAY_VIEW_CURSOR_INNER_RADIUS
#define DILAY_VIEW_CURSOR_INNER_RADIUS

#include "view/cursor.hpp"

class ViewCursorInnerRadius : public ViewCursor {
  public: 
    DECLARE_BIG6 (ViewCursorInnerRadius)

    float innerRadiusFactor () const;
    void  innerRadiusFactor (float);

  private:
    IMPLEMENTATION

    void runUpdate ();
    void runRender (const Camera&) const;
};

#endif
