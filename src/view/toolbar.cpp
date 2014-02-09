#include <QAction>
#include <QToolButton>
#include <QCheckBox>
#include "view/toolbar.hpp"
#include "mesh-type.hpp"

struct ViewToolbar :: Impl {
  static const unsigned int actionsSize = 2;
  typedef std::array <QAction*, Impl::actionsSize> Actions;

  ViewToolbar* self;
  Actions      actions;
  QCheckBox*   hideOthersCheckbox;

  Impl (ViewToolbar* s) : self (s) {
    this->self->setMovable (false);
    this->addState      (MeshType::FreeForm, tr("Freeform Mesh"));
    this->addState      (MeshType::Sphere  , tr("Sphere Mesh"));
    this->addHideOthers ();
  }

  unsigned int actionIndex (MeshType t) const {
    if (MeshType::FreeForm == t)
      return 0;
    else if (MeshType::Sphere == t)
      return 1;
    else
      assert (false);
  }

  void addState (MeshType meshType, const QString& label) {
    unsigned int i   = this->actionIndex (meshType);
    this->actions[i] = this->self->addAction (label);
    this->actions[i]->setCheckable (true);
    QToolButton* button = (QToolButton*)this->self->widgetForAction (this->actions[i]);
    button->setAutoRaise (false);

    QObject::connect (this->actions[i], &QAction::toggled, [meshType,i,this] (bool checked) {
      if (checked) {
        for (QAction* a : this->actions) {
          if (this->actions[i] != a) 
            a->setChecked (false);
        }
        emit this->self->selectionChanged (meshType);
      }
      else {
        emit this->self->selectionReseted ();
      }
    });
  }

  void addHideOthers () {
    this->hideOthersCheckbox = new QCheckBox (tr ("Hide Others"));
    this->self->addWidget (this->hideOthersCheckbox);

    QObject::connect (this->hideOthersCheckbox, &QCheckBox::stateChanged, [this] (int state) {
      if (state == Qt::Checked)
        emit this->self->hideOthersChanged (true);
      else if (state == Qt::Unchecked)
        emit this->self->hideOthersChanged (false);
    });
  }

  bool selected (MeshType t) const { 
    return this->actions[this->actionIndex (t)]->isChecked (); 
  }

  bool hideOthers () const { 
    return this->hideOthersCheckbox->isChecked (); 
  }

  bool show (MeshType t) const { 
    return this->selected (t) || ! this->hideOthers (); 
  }
}; 

DELEGATE1_WIDGET_BIG6 (ViewToolbar, QToolBar, QWidget*)

DELEGATE1_CONST (bool, ViewToolbar, selected, MeshType)
DELEGATE1_CONST (bool, ViewToolbar, show, MeshType)
