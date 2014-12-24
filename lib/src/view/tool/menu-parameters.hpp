#ifndef DILAY_VIEW_TOOL_MENU_PARAMETERS
#define DILAY_VIEW_TOOL_MENU_PARAMETERS

#include <glm/fwd.hpp>
#include "macro.hpp"

class State;
class QString;

class ViewToolMenuParameters {
  public:
    DECLARE_BIG4COPY (ViewToolMenuParameters, State&, const glm::ivec2&, const QString&)

          State&      state         () const;
    const glm::ivec2& clickPosition () const;
    const QString&    label         () const;

  private:
    IMPLEMENTATION
};

#endif
