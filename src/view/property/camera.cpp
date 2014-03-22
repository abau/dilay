#include <QPushButton>
#include "view/property/camera.hpp"
#include "view/util.hpp"

ViewPropertyCamera :: ViewPropertyCamera () {

  this->add <QPushButton> ("Narf", ViewUtil::pushButton ("foo"));
  
}
