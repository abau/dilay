/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include <limits>
#include "test-misc.hpp"
#include "util.hpp"

void TestMisc::test ()
{
  assert (Util::countOnes (0) == 0);
  assert (Util::countOnes (1) == 1);
  assert (Util::countOnes (2) == 1);
  assert (Util::countOnes (3) == 2);
  assert (Util::countOnes (127) == 7);
  assert (Util::countOnes (128) == 1);
  assert (Util::countOnes (255) == 8);
  assert (Util::countOnes (256) == 1);

  assert (Util::countOnes (std::numeric_limits<unsigned int>::max ()) == sizeof (unsigned int) * 8);
}
