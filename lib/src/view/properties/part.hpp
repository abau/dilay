#ifndef DILAY_VIEW_PROPERTIES_PART
#define DILAY_VIEW_PROPERTIES_PART

#include "macro.hpp"

class QWidget;
class QButtonGroup;
class QString;
class QVBoxLayout;

class ViewPropertiesPart {
  public:
    DECLARE_BIG3 (ViewPropertiesPart, QVBoxLayout&, unsigned int)

    void reset ();
    void add   (QWidget&);
    void add   (const QString&, QWidget&);
    void add   (QButtonGroup&, const std::vector <QString>&);

  private:
    IMPLEMENTATION
};

#endif
