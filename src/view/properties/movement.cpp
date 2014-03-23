#include <QPushButton>
#include "view/properties/movement.hpp"
#include "view/util.hpp"

ViewPropertiesMovement :: ViewPropertiesMovement () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo",true));
  
}
