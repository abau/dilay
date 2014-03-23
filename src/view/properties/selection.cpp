#include <QPushButton>
#include "view/properties/selection.hpp"
#include "view/util.hpp"

ViewPropertiesSelection :: ViewPropertiesSelection () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo"));
  
}
