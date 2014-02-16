#ifndef DILAY_VIEW_TOOLBAR
#define DILAY_VIEW_TOOLBAR

#include <QToolBar>
#include "macro.hpp"

enum class MeshType;

class ViewToolbar : public QToolBar {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewToolbar)

    bool selected (MeshType) const;
    bool show     (MeshType) const;

  signals:
    void selectionChanged  (MeshType);
    void hideOthersChanged (bool);
    void selectionReseted  ();

  private:
    class Impl;
    Impl* impl;
};

#endif
