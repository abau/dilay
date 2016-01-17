/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QHBoxLayout>
#include <QLineEdit>
#include "view/util.hpp"
#include "view/vector-edit.hpp"

struct ViewVectorEdit::Impl {
  ViewVectorEdit*   self;
  glm::vec3         vectorData;
  QLineEdit*        edit [3];

  static const int  numDecimals = 2;

  Impl (ViewVectorEdit* s, const glm::vec3& v) 
    : self       (s) 
    , vectorData (v)
  { 
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing         (0);
    layout->setContentsMargins (0,11,0,11);
    this->self->setLayout      (layout);

    for (int i = 0; i <= 2; i++) {
      this->edit[i] = &ViewUtil::lineEdit (v[i], Impl::numDecimals);

      ViewUtil::connect (*this->edit[i], [this,i] (float v) {
        this->vectorData[i] = v;
        emit this->self->vectorEdited (this->vectorData);
      });

      layout->addWidget (this->edit[i]);
    }
  };

  void vector (const glm::vec3& v) { this->x (v.x); this->y (v.y); this->z (v.z); }
  void x      (float v)            { this->changeComponent (0,v); }
  void y      (float v)            { this->changeComponent (1,v); }
  void z      (float v)            { this->changeComponent (2,v); }

  void changeComponent (int i, float v) {
    this->vectorData[i] = v;
    this->edit[i]->setText (QString::number (v, 'f', Impl::numDecimals));
  }
};

DELEGATE_BIG2_BASE ( ViewVectorEdit, (const glm::vec3& v, QWidget* p)
                   , (this,v), QWidget, (p) )
DELEGATE1 (void, ViewVectorEdit, vector, const glm::vec3&)
DELEGATE1 (void, ViewVectorEdit, x     , float)
DELEGATE1 (void, ViewVectorEdit, y     , float)
DELEGATE1 (void, ViewVectorEdit, z     , float)
