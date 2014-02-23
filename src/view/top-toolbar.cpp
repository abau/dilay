#include <QAction>
#include <QToolButton>
#include <QCheckBox>
#include "view/top-toolbar.hpp"
#include "mesh-type.hpp"

struct ViewTopToolbar :: Impl {
  static const unsigned int actionsSize = 2;
  typedef std::array <QAction*, Impl::actionsSize> Actions;

  ViewTopToolbar* self;
  Actions         actions;
  QCheckBox*      hideOthersCheckbox;

  Impl (ViewTopToolbar* s) : self (s) {
    this->self->setMovable   (false);
    this->self->setFloatable (false);
    this->addState      (MeshType::FreeForm, tr("Freeform Mesh"));
    this->addState      (MeshType::Sphere  , tr("Sphere Mesh"));
    this->addHideOthers ();

    emit actions[0]->toggled (true);
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
          if (this->actions[i] == a)
            a->setChecked (true);
          else
            a->setChecked (false);
        }
        if (! this->hideOthersCheckbox->isEnabled ()) {
          this->hideOthersCheckbox->setEnabled (true);
        }
        emit this->self->selectionChanged (meshType);
      }
      else {
        this->hideOthersCheckbox->setEnabled (false);
        emit this->self->selectionReseted ();
      }
    });
  }

  void addHideOthers () {
    this->hideOthersCheckbox = new QCheckBox (tr ("Hide Others"));
    this->hideOthersCheckbox->setChecked     (false);
    this->hideOthersCheckbox->setEnabled     (false);
    this->hideOthersCheckbox->setFocusPolicy (Qt::NoFocus);
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

DELEGATE_BIG3_SELF (ViewTopToolbar)

DELEGATE1_CONST (bool, ViewTopToolbar, selected, MeshType)
DELEGATE1_CONST (bool, ViewTopToolbar, show, MeshType)
