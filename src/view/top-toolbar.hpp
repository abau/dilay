#ifndef DILAY_VIEW_TOP_TOOLBAR
#define DILAY_VIEW_TOP_TOOLBAR

#include <QToolBar>
#include "macro.hpp"

enum class MeshType;

class ViewTopToolbar : public QToolBar {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewTopToolbar)

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
