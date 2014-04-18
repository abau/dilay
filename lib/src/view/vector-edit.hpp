#ifndef DILAY_VIEW_VECTOR_EDIT
#define DILAY_VIEW_VECTOR_EDIT

#include <glm/glm.hpp>
#include <QWidget>
#include "macro.hpp"

class ViewVectorEdit : public QWidget {
  Q_OBJECT
  public:
    DECLARE_BIG3 (ViewVectorEdit, const glm::vec3&, QWidget* = nullptr)

    void vector (const glm::vec3&);
    void x      (float);
    void y      (float);
    void z      (float);

  signals:
    void vectorEdited (const glm::vec3&);

  private:
    class Impl;
    Impl* impl;
};

#endif
