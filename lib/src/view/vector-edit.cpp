#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "view/vector-edit.hpp"

struct ViewVectorEdit::Impl {
  ViewVectorEdit*   self;
  glm::vec3         vectorData;
  QDoubleValidator* validator [3];
  QLineEdit*        edit [3];

  static const int  numDecimals = 2;

  Impl (ViewVectorEdit* s, const QString& label, const glm::vec3& v) 
    : self       (s) 
    , vectorData (v)
  { 
    QGridLayout* layout = new QGridLayout;
    layout->setSpacing         (0);
    layout->setContentsMargins (0,11,0,11);
    this->self->setLayout      (layout);

    for (int i = 0; i <= 2; i++) {
      this->edit[i]      = new QLineEdit;
      this->validator[i] = new QDoubleValidator (this->edit[i]);

      this->validator[i]->setDecimals (Impl::numDecimals);
      this->edit[i]->setValidator     (this->validator[i]);
      this->changeComponent           (i, v[i]);

      QString component (i == 0 ? "x" : (i == 1 ? "y" : "z"));
      
      layout->addWidget (new QLabel (label + "-" + component + ": "), i, 0);
      layout->addWidget (this->edit[i], i, 1);

      QObject::connect (this->edit[i], &QLineEdit::textEdited, [this,i] (const QString& text)
        { this->componentChanged (i, text); });
    }
  };

  void componentChanged (int i, const QString& text) {
    if (this->edit[i]->hasAcceptableInput ()) {
      bool  ok;
      float f = text.toFloat (&ok);
      assert (ok);
      this->vectorData[i] = f;
      emit this->self->vectorEdited (this->vectorData);
    }
  }

  void vector (const glm::vec3& v) { this->x (v.x); this->y (v.y); this->z (v.z); }
  void x      (float v)            { this->changeComponent (0,v); }
  void y      (float v)            { this->changeComponent (1,v); }
  void z      (float v)            { this->changeComponent (2,v); }

  void changeComponent (int i, float v) {
    this->vectorData[i] = v;
    this->edit[i]->setText (QString::number (v, 'f', Impl::numDecimals));
  }
};

DELEGATE_BIG2_BASE ( ViewVectorEdit, (const QString& l, const glm::vec3& v, QWidget* p)
                   , (this,l,v), QWidget, (p) )
DELEGATE1 (void, ViewVectorEdit, vector, const glm::vec3&)
DELEGATE1 (void, ViewVectorEdit, x     , float)
DELEGATE1 (void, ViewVectorEdit, y     , float)
DELEGATE1 (void, ViewVectorEdit, z     , float)
