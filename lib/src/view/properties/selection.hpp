#ifndef DILAY_VIEW_PROPERTIES_SELECTION
#define DILAY_VIEW_PROPERTIES_SELECTION

#include "view/properties.hpp"
#include "macro.hpp"

enum class MeshType;

class ViewPropertiesSelection : public ViewProperties {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewPropertiesSelection)

    bool     selected (MeshType) const;
    MeshType selected () const;
    bool     show     (MeshType) const;

  signals:
    void selectionChanged  (MeshType);
    void hideOthersChanged (bool);

  private:
    class Impl;
    Impl* impl;
};

#endif
