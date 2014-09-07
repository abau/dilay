#ifndef DILAY_VIEW_PROPERTIES_SELECTION
#define DILAY_VIEW_PROPERTIES_SELECTION

#include "macro.hpp"
#include "view/properties.hpp"

enum class SelectionMode;

class ViewPropertiesSelection : public ViewProperties {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewPropertiesSelection)

    SelectionMode selectionMode () const;

  signals:
    void selectionModeChanged (SelectionMode);

  private:
    IMPLEMENTATION
};

#endif
