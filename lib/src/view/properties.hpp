#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include <QWidget>
#include "macro.hpp"

class ViewProperties : public QWidget {
  public:
    DECLARE_BIG3 (ViewProperties)

    void setLabel (const QString&);

    template <typename T>
    T& add (const QString& label, T& widget) {
      this->addWidget (label, static_cast <QWidget&> (widget));
      return widget;
    }

    template <typename T>
    T& add (T& widget) {
      this->addWidget (static_cast <QWidget&> (widget));
      return widget;
    }

    QWidget& addWidget (const QString&, QWidget&);
    QWidget& addWidget (QWidget&);

  private:
    IMPLEMENTATION
};

#endif
