#include <QPushButton>
#include "view/property/movement.hpp"
#include "view/util.hpp"

ViewPropertyMovement :: ViewPropertyMovement () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo"));
  
}
