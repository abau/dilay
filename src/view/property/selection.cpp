#include <QPushButton>
#include "view/property/selection.hpp"
#include "view/util.hpp"

ViewPropertySelection :: ViewPropertySelection () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo"));
  
}
