#include <QPushButton>
#include "view/properties/camera.hpp"
#include "view/util.hpp"

ViewPropertiesCamera :: ViewPropertiesCamera () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo"));
  
}
