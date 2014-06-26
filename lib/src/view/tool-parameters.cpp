#include <glm/glm.hpp>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMoveEvent>
#include "view/tool-parameters.hpp"
#include "view/main-window.hpp"
#include "view/vector-edit.hpp"
#include "config.hpp"
#include "view/util.hpp"

struct ViewToolParameters::Impl {
  ViewToolParameters* self;
  QVBoxLayout&        vBoxLayout;
  QGridLayout&        gridLayout;
  QPushButton&        closeButton;

  Impl (ViewToolParameters* s, ViewMainWindow& p) 
    : self        (s)
    , vBoxLayout  (*new QVBoxLayout)
    , gridLayout  (*new QGridLayout)
    , closeButton (ViewUtil::pushButton (tr("Close"), true))

  {
    glm::ivec2 pos = Config::get <glm::ivec2> ( "/cache/view/tool-parameters/position"
                                              , glm::ivec2 (100, 100) );

    this->gridLayout.setColumnStretch (0,0);
    this->gridLayout.setColumnStretch (1,1);
    this->vBoxLayout.addLayout        (&this->gridLayout);
    this->self->setParent             (&p, this->self->windowFlags ());
    this->self->setLayout             (&this->vBoxLayout);
    this->self->move                  (pos.x, pos.y);
    this->self->setAttribute          (Qt::WA_DeleteOnClose);
    this->setupDefaultButtons         ();
  }
  
  void setupDefaultButtons () {
    this->closeButton.setDefault (true);
    this->vBoxLayout.addWidget   (&this->closeButton);

    QObject::connect ( &this->closeButton, &QPushButton::released
                     , [this] () { this->self->accept (); } );
  }

  QWidget& addParameter (const QString& label, QWidget& widget) {
    const int r = this->gridLayout.rowCount ();
    this->gridLayout.addWidget (new QLabel (label), r, 0);
    this->gridLayout.addWidget (&widget           , r, 1);
    return widget;
  }

  void moveEvent (QMoveEvent* event) {
    Config::cache <glm::ivec2> ( "/cache/view/tool-parameters/position"
                               , glm::ivec2 (event->pos ().x(), event->pos ().y()) );
  }
};

DELEGATE1_BIG3_SELF (ViewToolParameters, ViewMainWindow&)

DELEGATE2 (QWidget&, ViewToolParameters, addParameter, const QString&, QWidget&)
DELEGATE1 (void    , ViewToolParameters, moveEvent, QMoveEvent*)
