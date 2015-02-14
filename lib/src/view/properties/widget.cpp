#include <QStackedLayout>
#include <QVBoxLayout>
#include "view/properties/selection.hpp"
#include "view/properties/widget.hpp"

struct ViewPropertiesWidget::Impl {
  ViewPropertiesWidget*   self;
  ViewPropertiesSelection selection;
  ViewProperties          tool;

  Impl (ViewPropertiesWidget* s) 
    : self      (s) 
  {
    this->setupNonToolProperties ();
    this->setupToolProperties    ();
  }

  void setupNonToolProperties () {
    QWidget*     widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    layout->setSpacing    (0);
    layout->addWidget     (&this->selection);
    layout->addStretch    (1);
    widget->setLayout     (layout);
    this->self->addWidget (widget);
  }

  void setupToolProperties () {
    QWidget*     widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    layout->setSpacing    (0);
    layout->addWidget     (&this->tool);
    layout->addStretch    (1);
    widget->setLayout     (layout);
    this->self->addWidget (widget);
  }

  void showTool (const QString& name) {
    this->self->setCurrentIndex (1);
    this->tool.label            (name);
  }

  void resetTool () {
    this->tool.reset ();
    this->self->setCurrentIndex (0);
  }
};

DELEGATE_BIG2_SELF (ViewPropertiesWidget)
GETTER    (ViewPropertiesSelection&, ViewPropertiesWidget, selection)
GETTER    (ViewProperties&, ViewPropertiesWidget, tool)
DELEGATE1 (void, ViewPropertiesWidget, showTool, const QString&)
DELEGATE  (void, ViewPropertiesWidget, resetTool)
