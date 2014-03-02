#ifndef DILAY_VIEW_VECTOR_EDIT
#define DILAY_VIEW_VECTOR_EDIT

#include <QWidget>
#include "macro.hpp"

class ViewVectorEdit : public QWidget {
  public:
    DECLARE_BIG3 (ViewVectorEdit, QWidget*)
    ViewVectorEdit ();

  private:
    class Impl;
    Impl* impl;
};

#endif
