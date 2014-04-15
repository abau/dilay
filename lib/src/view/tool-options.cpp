#include <glm/glm.hpp>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMoveEvent>
#include "view/tool-options.hpp"
#include "view/main-window.hpp"
#include "view/vector-edit.hpp"
#include "config.hpp"
#include "view/util.hpp"

struct ViewToolOptions::Impl {
  ViewToolOptions* self;
  QVBoxLayout*     vBoxLayout;
  QGridLayout*     gridLayout;
  QPushButton*     closeButton;

  Impl (ViewToolOptions* s, ViewMainWindow* p) 
    : self   (s)
  {
    glm::ivec2 pos   = Config::get <glm::ivec2> ( "/cache/view/tool-options/position"
                                                , glm::ivec2 (100, 100) );
    this->vBoxLayout = new QVBoxLayout;
    this->gridLayout = new QGridLayout;

    this->gridLayout->setColumnStretch (0,0);
    this->gridLayout->setColumnStretch (1,1);
    this->vBoxLayout->addLayout        (this->gridLayout);
    this->self->setParent              (p, this->self->windowFlags ());
    this->self->setLayout              (this->vBoxLayout);
    this->self->move                   (pos.x, pos.y);
    this->self->setAttribute           (Qt::WA_DeleteOnClose);
    this->makeDefaultButtons           ();
  }
  
  void makeDefaultButtons () {
    this->closeButton = ViewUtil::pushButton (tr("Close"), true);

    this->closeButton->setDefault (true);
    this->vBoxLayout->addWidget   (this->closeButton);

    QObject::connect ( this->closeButton, &QPushButton::released
                     , [this] () { this->self->accept (); } );
  }

  QWidget* addOption (const QString& label, QWidget* widget) {
    const int r = this->gridLayout->rowCount ();
    this->gridLayout->addWidget (new QLabel (label), r, 0);
    this->gridLayout->addWidget (widget            , r, 1);
    return widget;
  }

  void moveEvent (QMoveEvent* event) {
    Config::cache <glm::ivec2> ( "/cache/view/tool-options/position"
                               , glm::ivec2 (event->pos ().x(), event->pos ().y()) );
  }
};

DELEGATE1_BIG3_SELF (ViewToolOptions, ViewMainWindow*)

DELEGATE2 (QWidget*, ViewToolOptions, addOption, const QString&, QWidget*)
DELEGATE1 (void    , ViewToolOptions, moveEvent, QMoveEvent*)
