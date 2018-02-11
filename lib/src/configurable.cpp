/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "configurable.hpp"

void Configurable::fromConfig (const Config& c) { this->runFromConfig (c); }

void ProxyConfigurable::fromConfig (const ConfigProxy& c) { this->runFromConfig (c); }
