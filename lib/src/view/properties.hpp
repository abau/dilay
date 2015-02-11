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

    void setLabel (const QString&);

    void addWidget       (const QString&, QWidget&);
    void addWidget       (QWidget&);
    void addRadioButtons (QButtonGroup&, const std::vector <QString>&);
    void setFooter       (QWidget&);
    void resetWidgets    ();

  private:
    IMPLEMENTATION
};

#endif
