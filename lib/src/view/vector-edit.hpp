#ifndef DILAY_VIEW_VECTOR_EDIT
#define DILAY_VIEW_VECTOR_EDIT

#include <QWidget>
#include <glm/glm.hpp>
#include "macro.hpp"

class ViewVectorEdit : public QWidget {
  Q_OBJECT
  public:
    DECLARE_BIG2 (ViewVectorEdit, const glm::vec3&, QWidget* = nullptr)

    void vector (const glm::vec3&);
    void x      (float);
    void y      (float);
    void z      (float);

  signals:
    void vectorEdited (const glm::vec3&);

  private:
    IMPLEMENTATION
};

#endif
