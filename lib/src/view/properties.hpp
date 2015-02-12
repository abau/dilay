#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include <vector>
#include <QWidget>
#include "macro.hpp"

class QButtonGroup;
class QString;

class ViewProperties : public QWidget {
  public:
    DECLARE_BIG3 (ViewProperties)

    void label           (const QString&);
    void resetBody       ();
    void resetFooter     ();
    void reset           ();
    void addWidget       (const QString&, QWidget&);
    void addWidget       (QWidget&);
    void addRadioButtons (QButtonGroup&, const std::vector <QString>&);
    void setFooter       (QWidget&);

  private:
    IMPLEMENTATION
};

#endif
