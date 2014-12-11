#ifndef DILAY_VIEW_TOOL_MENU_PARAMETERS
#define DILAY_VIEW_TOOL_MENU_PARAMETERS

#include <glm/fwd.hpp>
#include "macro.hpp"

class QString;

class ViewToolMenuParameters {
  public:
    DECLARE_BIG4COPY (ViewToolMenuParameters, const glm::ivec2&)

    ViewToolMenuParameters (const glm::ivec2&, const QString&);

    const glm::ivec2& clickPosition () const;
          bool        hasLabel      () const;
    const QString&    label         () const;

  private:
    IMPLEMENTATION
};

#endif
