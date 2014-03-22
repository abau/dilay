#ifndef DILAY_VIEW_PROPERTY
#define DILAY_VIEW_PROPERTY

#include <QWidget>
#include "macro.hpp"

class ViewProperty : public QWidget {
  public:
    DECLARE_BIG3 (ViewProperty)

    template <typename T>
    T* add (const QString& label, T* widget) {
      this->addWidget (label, static_cast <QWidget*> (widget));
      return widget;
    }

    QWidget* addWidget (const QString&, QWidget*);

  private:
    class Impl;
    Impl* impl;
};

#endif
