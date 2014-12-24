#include "configurable.hpp"

void Configurable :: fromConfig (const Config& c) {
  this->runFromConfig (c);
}

void ProxyConfigurable :: fromConfig (const ConfigProxy& c) {
  this->runFromConfig (c);
}
