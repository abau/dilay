#ifndef DILAY_VIEW_TOOL_MENU_PARAMETERS
#define DILAY_VIEW_TOOL_MENU_PARAMETERS

#include <glm/fwd.hpp>
#include "macro.hpp"

class QString;
class ViewMainWindow;

class ViewToolMenuParameters {
  public:
    DECLARE_BIG4COPY (ViewToolMenuParameters, ViewMainWindow&, const glm::ivec2&)

    ViewToolMenuParameters (ViewMainWindow&, const glm::ivec2&, const QString&);

    ViewMainWindow&   mainWindow    () const;
    const glm::ivec2& clickPosition () const;
          bool        hasLabel      () const;
    const QString&    label         () const;

  private:
    IMPLEMENTATION
};

#endif
