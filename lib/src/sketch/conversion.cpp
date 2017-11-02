/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <thread>
#include <vector>
#include "../mesh.hpp"
#include "distance.hpp"
#include "mesh-util.hpp"
#include "primitive/cone-sphere.hpp"
#include "sketch/conversion.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "util.hpp"

/* vertex layout:          edge layout:          face layout:
 *
 *     2------------3          o-----3------o      - 0: bottom
 *    /|           /|         /|           /|      - 1: top
 *   / |          / |        8 1         11 |      - 2: left
 *  /  |         /  |       /  |         /  4      - 3: right
 * 6------------7   |      o-----9------o   |      - 4: back
 * |   |        |   |      |   |        |   |      - 5: front
 * |   0--------|---1      |   o----0---|---o
 * |  /         |  /       7  /        10  /
 * | /          | /        | 2          | 5
 * |/           |/         |/           |/
 * 4------------5          o-----6------o
 */
namespace
{
  static glm::vec3 invalidVec3 = glm::vec3 (Util::minFloat ());

#ifndef NDEBUG
  static unsigned int configurationBase[256] = {
    0,  1,  1,  2,  1,  2,  3,  5,  1,  3,  2,  5,  2,  5,  5,  8,  1,  2,  3,  5,  3,  5,  7,  9,
    4,  6,  6,  11, 6,  14, 12, 17, 1,  3,  2,  5,  4,  6,  6,  14, 3,  7,  5,  9,  6,  12, 11, 17,
    2,  5,  5,  8,  6,  11, 12, 17, 6,  12, 14, 17, 10, 16, 16, 20, 1,  3,  4,  6,  2,  5,  6,  11,
    3,  7,  6,  12, 5,  9,  14, 17, 2,  5,  6,  14, 5,  8,  12, 17, 6,  12, 10, 16, 11, 17, 16, 20,
    3,  7,  6,  12, 6,  12, 10, 16, 7,  13, 12, 15, 12, 15, 16, 19, 5,  9,  11, 17, 14, 17, 16, 20,
    12, 15, 16, 19, 16, 19, 18, 21, 1,  4,  3,  6,  3,  6,  7,  12, 2,  6,  5,  14, 5,  11, 9,  17,
    3,  6,  7,  12, 7,  12, 13, 15, 6,  10, 12, 16, 12, 16, 15, 19, 2,  6,  5,  11, 6,  10, 12, 16,
    5,  12, 8,  17, 14, 16, 17, 20, 5,  14, 9,  17, 12, 16, 15, 19, 11, 16, 17, 20, 16, 18, 19, 21,
    2,  6,  6,  10, 5,  14, 12, 16, 5,  12, 11, 16, 8,  17, 17, 20, 5,  11, 12, 16, 9,  17, 15, 19,
    14, 16, 16, 18, 17, 20, 19, 21, 5,  12, 14, 16, 11, 16, 16, 18, 9,  15, 17, 19, 17, 19, 20, 21,
    8,  17, 17, 20, 17, 20, 19, 21, 17, 19, 20, 21, 20, 21, 21, 22};
#endif

  static bool nonManifoldConfig[256] = {
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, true,  true,  false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    true,  false, false, true,  false, false, false, false, false, false, false, false, true,
    false, false, false, false, false, false, true,  true,  false, false, false, false, false,
    false, true,  false, false, false, true,  true,  true,  true,  false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, true,
    false, true,  false, true,  false, false, false, false, false, false, false, true,  false,
    false, false, false, false, true,  false, false, false, false, false, false, false, true,
    false, true,  false, true,  false, false, true,  false, true,  false, false, false, false,
    false, false, false, false, true,  false, false, false, true,  false, false, false, false,
    false, false, false, true,  false, false, false, true,  false, true,  true,  false, false,
    false, true,  false, false, false, false, true,  false, true,  true,  false, false, false,
    false, true,  false, true,  false, false, false, false, false, false, false, false, true,
    false, false, true,  false, false, false, false, false, false};

  static int vertexIndicesByConfiguration[256][12] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, -1, -1, -1, 0, 0, -1, -1, -1, -1, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1},
    {-1, 0, -1, 0, -1, -1, -1, -1, 0, -1, -1, -1},
    {0, -1, 0, 0, -1, -1, -1, -1, 0, -1, -1, -1},
    {0, 1, -1, 1, 0, 0, -1, -1, 1, -1, -1, -1},
    {-1, -1, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1},
    {-1, -1, -1, 0, 0, -1, -1, -1, -1, -1, -1, 0},
    {0, 0, 0, 1, 1, -1, -1, -1, -1, -1, -1, 1},
    {0, -1, -1, 0, -1, 0, -1, -1, -1, -1, -1, 0},
    {-1, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, 0},
    {-1, 0, -1, -1, 0, -1, -1, -1, 0, -1, -1, 0},
    {0, -1, 0, -1, 0, -1, -1, -1, 0, -1, -1, 0},
    {0, 0, -1, -1, -1, 0, -1, -1, 0, -1, -1, 0},
    {-1, -1, 0, -1, -1, 0, -1, -1, 0, -1, -1, 0},
    {-1, -1, 0, -1, -1, -1, 0, 0, -1, -1, -1, -1},
    {0, 0, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1},
    {0, -1, 1, -1, 0, 0, 1, 1, -1, -1, -1, -1},
    {-1, 0, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1},
    {-1, 1, 0, 1, -1, -1, 0, 0, 1, -1, -1, -1},
    {0, -1, -1, 0, -1, -1, 0, 0, 0, -1, -1, -1},
    {2, 0, 1, 0, 2, 2, 1, 1, 0, -1, -1, -1},
    {-1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1},
    {-1, -1, 0, 1, 1, -1, 0, 0, -1, -1, -1, 1},
    {0, 0, -1, 1, 1, -1, 0, 0, -1, -1, -1, 1},
    {0, -1, 1, 0, -1, 0, 1, 1, -1, -1, -1, 0},
    {-1, 0, -1, 0, -1, 0, 0, 0, -1, -1, -1, 0},
    {-1, 0, 1, -1, 0, -1, 1, 1, 0, -1, -1, 0},
    {0, -1, -1, -1, 0, -1, 0, 0, 0, -1, -1, 0},
    {1, 1, 0, -1, -1, 1, 0, 0, 1, -1, -1, 1},
    {-1, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, 0},
    {-1, -1, -1, -1, -1, 0, 0, -1, -1, -1, 0, -1},
    {0, 0, 0, -1, -1, 1, 1, -1, -1, -1, 1, -1},
    {0, -1, -1, -1, 0, -1, 0, -1, -1, -1, 0, -1},
    {-1, 0, 0, -1, 0, -1, 0, -1, -1, -1, 0, -1},
    {-1, 1, -1, 1, -1, 0, 0, -1, 1, -1, 0, -1},
    {0, -1, 0, 0, -1, 1, 1, -1, 0, -1, 1, -1},
    {0, 1, -1, 1, 0, -1, 0, -1, 1, -1, 0, -1},
    {-1, -1, 0, 0, 0, -1, 0, -1, 0, -1, 0, -1},
    {-1, -1, -1, 1, 1, 0, 0, -1, -1, -1, 0, 1},
    {1, 1, 1, 0, 0, 2, 2, -1, -1, -1, 2, 0},
    {0, -1, -1, 0, -1, -1, 0, -1, -1, -1, 0, 0},
    {-1, 0, 0, 0, -1, -1, 0, -1, -1, -1, 0, 0},
    {-1, 0, -1, -1, 0, 1, 1, -1, 0, -1, 1, 0},
    {1, -1, 1, -1, 1, 0, 0, -1, 1, -1, 0, 1},
    {0, 0, -1, -1, -1, -1, 0, -1, 0, -1, 0, 0},
    {-1, -1, 0, -1, -1, -1, 0, -1, 0, -1, 0, 0},
    {-1, -1, 0, -1, -1, 0, -1, 0, -1, -1, 0, -1},
    {0, 0, -1, -1, -1, 0, -1, 0, -1, -1, 0, -1},
    {0, -1, 0, -1, 0, -1, -1, 0, -1, -1, 0, -1},
    {-1, 0, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1},
    {-1, 1, 0, 1, -1, 0, -1, 0, 1, -1, 0, -1},
    {0, -1, -1, 0, -1, 0, -1, 0, 0, -1, 0, -1},
    {1, 0, 1, 0, 1, -1, -1, 1, 0, -1, 1, -1},
    {-1, -1, -1, 0, 0, -1, -1, 0, 0, -1, 0, -1},
    {-1, -1, 0, 1, 1, 0, -1, 0, -1, -1, 0, 1},
    {1, 1, -1, 0, 0, 1, -1, 1, -1, -1, 1, 0},
    {0, -1, 0, 0, -1, -1, -1, 0, -1, -1, 0, 0},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, -1, 0, 0},
    {-1, 1, 0, -1, 1, 0, -1, 0, 1, -1, 0, 1},
    {1, -1, -1, -1, 1, 1, -1, 0, 0, -1, 0, 0},
    {1, 1, 1, -1, -1, -1, -1, 0, 0, -1, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, -1},
    {0, 0, 0, -1, -1, -1, -1, 1, 1, 1, -1, -1},
    {1, -1, -1, -1, 1, 1, -1, 0, 0, 0, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, 1, 1, 1, -1, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1, -1},
    {0, -1, 0, 0, -1, -1, -1, 0, -1, 0, -1, -1},
    {1, 0, -1, 0, 1, 1, -1, 0, -1, 0, -1, -1},
    {-1, -1, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1},
    {-1, -1, -1, 0, 0, -1, -1, 1, 1, 1, -1, 0},
    {2, 2, 2, 0, 0, -1, -1, 1, 1, 1, -1, 0},
    {0, -1, -1, 0, -1, 0, -1, 1, 1, 1, -1, 0},
    {-1, 1, 1, 1, -1, 1, -1, 0, 0, 0, -1, 1},
    {-1, 0, -1, -1, 0, -1, -1, 0, -1, 0, -1, 0},
    {0, -1, 0, -1, 0, -1, -1, 0, -1, 0, -1, 0},
    {0, 0, -1, -1, -1, 0, -1, 0, -1, 0, -1, 0},
    {-1, -1, 0, -1, -1, 0, -1, 0, -1, 0, -1, 0},
    {-1, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, -1},
    {0, 0, -1, -1, -1, -1, 0, -1, 0, 0, -1, -1},
    {1, -1, 0, -1, 1, 1, 0, -1, 0, 0, -1, -1},
    {-1, 0, -1, -1, 0, 0, 0, -1, 0, 0, -1, -1},
    {-1, 0, 0, 0, -1, -1, 0, -1, -1, 0, -1, -1},
    {0, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1, -1},
    {0, 1, 1, 1, 0, 0, 1, -1, -1, 1, -1, -1},
    {-1, -1, -1, 0, 0, 0, 0, -1, -1, 0, -1, -1},
    {-1, -1, 0, 1, 1, -1, 0, -1, 0, 0, -1, 1},
    {1, 1, -1, 0, 0, -1, 1, -1, 1, 1, -1, 0},
    {1, -1, 0, 1, -1, 1, 0, -1, 0, 0, -1, 1},
    {-1, 1, -1, 1, -1, 0, 0, -1, 1, 0, -1, 0},
    {-1, 0, 0, -1, 0, -1, 0, -1, -1, 0, -1, 0},
    {0, -1, -1, -1, 0, -1, 0, -1, -1, 0, -1, 0},
    {1, 1, 1, -1, -1, 0, 0, -1, -1, 0, -1, 0},
    {-1, -1, -1, -1, -1, 0, 0, -1, -1, 0, -1, 0},
    {-1, -1, -1, -1, -1, 0, 0, 1, 1, 1, 0, -1},
    {1, 1, 1, -1, -1, 0, 0, 2, 2, 2, 0, -1},
    {0, -1, -1, -1, 0, -1, 0, 1, 1, 1, 0, -1},
    {-1, 1, 1, -1, 1, -1, 1, 0, 0, 0, 1, -1},
    {-1, 0, -1, 0, -1, 1, 1, 0, -1, 0, 1, -1},
    {1, -1, 1, 1, -1, 0, 0, 1, -1, 1, 0, -1},
    {0, 1, -1, 1, 0, -1, 0, 1, -1, 1, 0, -1},
    {-1, -1, 1, 0, 0, -1, 1, 1, -1, 0, 0, -1},
    {-1, -1, -1, 1, 1, 2, 2, 0, 0, 0, 2, 1},
    {2, 2, 2, 3, 3, 0, 0, 1, 1, 1, 0, 3},
    {1, -1, -1, 1, -1, -1, 1, 0, 0, 0, 1, 1},
    {-1, 0, 0, 0, -1, -1, 0, 1, 1, 1, 0, 0},
    {-1, 1, -1, -1, 1, 0, 0, 1, -1, 1, 0, 1},
    {0, -1, 0, -1, 0, 1, 1, 0, -1, 0, 1, 0},
    {0, 0, -1, -1, -1, -1, 0, 0, -1, 1, 1, 1},
    {-1, -1, 0, -1, -1, -1, 0, 0, -1, 1, 1, 1},
    {-1, -1, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1},
    {0, 0, -1, -1, -1, 0, -1, -1, 0, 0, 0, -1},
    {0, -1, 0, -1, 0, -1, -1, -1, 0, 0, 0, -1},
    {-1, 0, -1, -1, 0, -1, -1, -1, 0, 0, 0, -1},
    {-1, 0, 0, 0, -1, 0, -1, -1, -1, 0, 0, -1},
    {0, -1, -1, 0, -1, 0, -1, -1, -1, 0, 0, -1},
    {1, 1, 1, 0, 0, -1, -1, -1, -1, 0, 0, -1},
    {-1, -1, -1, 0, 0, -1, -1, -1, -1, 0, 0, -1},
    {-1, -1, 1, 0, 0, 1, -1, -1, 1, 1, 1, 0},
    {0, 0, -1, 1, 1, 0, -1, -1, 0, 0, 0, 1},
    {0, -1, 0, 0, -1, -1, -1, -1, 0, 1, 1, 1},
    {-1, 0, -1, 0, -1, -1, -1, -1, 0, 1, 1, 1},
    {-1, 0, 0, -1, 0, 0, -1, -1, -1, 1, 1, 1},
    {0, -1, -1, -1, 0, 0, -1, -1, -1, 1, 1, 1},
    {1, 1, 1, -1, -1, -1, -1, -1, -1, 0, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0},
    {1, 1, 1, -1, -1, -1, -1, -1, -1, 0, 0, 0},
    {0, -1, -1, -1, 0, 0, -1, -1, -1, 1, 1, 1},
    {-1, 0, 0, -1, 0, 0, -1, -1, -1, 1, 1, 1},
    {-1, 0, -1, 0, -1, -1, -1, -1, 0, 1, 1, 1},
    {0, -1, 0, 0, -1, -1, -1, -1, 0, 1, 1, 1},
    {2, 1, -1, 1, 2, 2, -1, -1, 1, 0, 0, 0},
    {-1, -1, 1, 1, 1, 1, -1, -1, 1, 0, 0, 0},
    {-1, -1, -1, 0, 0, -1, -1, -1, -1, 0, 0, -1},
    {1, 1, 1, 0, 0, -1, -1, -1, -1, 0, 0, -1},
    {0, -1, -1, 0, -1, 0, -1, -1, -1, 0, 0, -1},
    {-1, 0, 0, 0, -1, 0, -1, -1, -1, 0, 0, -1},
    {-1, 0, -1, -1, 0, -1, -1, -1, 0, 0, 0, -1},
    {0, -1, 0, -1, 0, -1, -1, -1, 0, 0, 0, -1},
    {0, 0, -1, -1, -1, 0, -1, -1, 0, 0, 0, -1},
    {-1, -1, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1},
    {-1, -1, 0, -1, -1, -1, 0, 0, -1, 1, 1, 1},
    {0, 0, -1, -1, -1, -1, 0, 0, -1, 1, 1, 1},
    {1, -1, 2, -1, 1, 1, 2, 2, -1, 0, 0, 0},
    {-1, 1, -1, -1, 1, 1, 1, 1, -1, 0, 0, 0},
    {-1, 1, 0, 1, -1, -1, 0, 0, 1, 2, 2, 2},
    {1, -1, -1, 1, -1, -1, 1, 1, 1, 0, 0, 0},
    {3, 2, 0, 2, 3, 3, 0, 0, 2, 1, 1, 1},
    {-1, -1, -1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {-1, -1, 1, 0, 0, -1, 1, 1, -1, 0, 0, -1},
    {1, 1, -1, 0, 0, -1, 1, 1, -1, 0, 0, -1},
    {1, -1, 0, 1, -1, 1, 0, 0, -1, 1, 1, -1},
    {-1, 0, -1, 0, -1, 1, 1, 0, -1, 0, 1, -1},
    {-1, 1, 0, -1, 1, -1, 0, 0, 1, 1, 1, -1},
    {0, -1, -1, -1, 0, -1, 0, 1, 1, 1, 0, -1},
    {0, 0, 1, -1, -1, 0, 1, 1, 0, 0, 0, -1},
    {-1, -1, -1, -1, -1, 0, 0, 1, 1, 1, 0, -1},
    {-1, -1, -1, -1, -1, 0, 0, -1, -1, 0, -1, 0},
    {1, 1, 1, -1, -1, 0, 0, -1, -1, 0, -1, 0},
    {0, -1, -1, -1, 0, -1, 0, -1, -1, 0, -1, 0},
    {-1, 0, 0, -1, 0, -1, 0, -1, -1, 0, -1, 0},
    {-1, 1, -1, 1, -1, 0, 0, -1, 1, 0, -1, 0},
    {1, -1, 1, 1, -1, 0, 0, -1, 1, 0, -1, 0},
    {1, 0, -1, 0, 1, -1, 1, -1, 0, 1, -1, 1},
    {-1, -1, 0, 1, 1, -1, 0, -1, 0, 0, -1, 1},
    {-1, -1, -1, 0, 0, 0, 0, -1, -1, 0, -1, -1},
    {0, 0, 0, 1, 1, 1, 1, -1, -1, 1, -1, -1},
    {0, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1, -1},
    {-1, 0, 0, 0, -1, -1, 0, -1, -1, 0, -1, -1},
    {-1, 0, -1, -1, 0, 0, 0, -1, 0, 0, -1, -1},
    {1, -1, 0, -1, 1, 1, 0, -1, 0, 0, -1, -1},
    {0, 0, -1, -1, -1, -1, 0, -1, 0, 0, -1, -1},
    {-1, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, -1},
    {-1, -1, 0, -1, -1, 0, -1, 0, -1, 0, -1, 0},
    {0, 0, -1, -1, -1, 0, -1, 0, -1, 0, -1, 0},
    {0, -1, 0, -1, 0, -1, -1, 0, -1, 0, -1, 0},
    {-1, 0, -1, -1, 0, -1, -1, 0, -1, 0, -1, 0},
    {-1, 0, 1, 0, -1, 1, -1, 1, 0, 1, -1, 1},
    {0, -1, -1, 0, -1, 0, -1, 1, 1, 1, -1, 0},
    {0, 1, 0, 1, 0, -1, -1, 0, 1, 0, -1, 0},
    {-1, -1, -1, 0, 0, -1, -1, 1, 1, 1, -1, 0},
    {-1, -1, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1},
    {1, 0, -1, 0, 1, 1, -1, 0, -1, 0, -1, -1},
    {0, -1, 0, 0, -1, -1, -1, 0, -1, 0, -1, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, 1, 1, 1, -1, -1},
    {1, -1, -1, -1, 1, 1, -1, 0, 0, 0, -1, -1},
    {0, 0, 0, -1, -1, -1, -1, 1, 1, 1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0},
    {1, 1, 1, -1, -1, -1, -1, 0, 0, -1, 0, 0},
    {1, -1, -1, -1, 1, 1, -1, 0, 0, -1, 0, 0},
    {-1, 1, 1, -1, 1, 1, -1, 0, 0, -1, 0, 0},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, -1, 0, 0},
    {0, -1, 0, 0, -1, -1, -1, 0, -1, -1, 0, 0},
    {0, 1, -1, 1, 0, 0, -1, 1, -1, -1, 1, 1},
    {-1, -1, 0, 1, 1, 0, -1, 0, -1, -1, 0, 1},
    {-1, -1, -1, 0, 0, -1, -1, 0, 0, -1, 0, -1},
    {0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1},
    {0, -1, -1, 0, -1, 0, -1, 0, 0, -1, 0, -1},
    {-1, 1, 0, 1, -1, 0, -1, 0, 1, -1, 0, -1},
    {-1, 0, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1},
    {0, -1, 0, -1, 0, -1, -1, 0, -1, -1, 0, -1},
    {0, 0, -1, -1, -1, 0, -1, 0, -1, -1, 0, -1},
    {-1, -1, 0, -1, -1, 0, -1, 0, -1, -1, 0, -1},
    {-1, -1, 0, -1, -1, -1, 0, -1, 0, -1, 0, 0},
    {0, 0, -1, -1, -1, -1, 0, -1, 0, -1, 0, 0},
    {0, -1, 1, -1, 0, 0, 1, -1, 1, -1, 1, 1},
    {-1, 0, -1, -1, 0, 1, 1, -1, 0, -1, 1, 0},
    {-1, 0, 0, 0, -1, -1, 0, -1, -1, -1, 0, 0},
    {0, -1, -1, 0, -1, -1, 0, -1, -1, -1, 0, 0},
    {1, 0, 0, 0, 1, 1, 0, -1, -1, -1, 0, 0},
    {-1, -1, -1, 1, 1, 0, 0, -1, -1, -1, 0, 1},
    {-1, -1, 0, 0, 0, -1, 0, -1, 0, -1, 0, -1},
    {0, 1, -1, 1, 0, -1, 0, -1, 1, -1, 0, -1},
    {0, -1, 0, 0, -1, 1, 1, -1, 0, -1, 1, -1},
    {-1, 1, -1, 1, -1, 0, 0, -1, 1, -1, 0, -1},
    {-1, 0, 0, -1, 0, -1, 0, -1, -1, -1, 0, -1},
    {0, -1, -1, -1, 0, -1, 0, -1, -1, -1, 0, -1},
    {0, 0, 0, -1, -1, 1, 1, -1, -1, -1, 1, -1},
    {-1, -1, -1, -1, -1, 0, 0, -1, -1, -1, 0, -1},
    {-1, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, 0},
    {0, 0, 0, -1, -1, 1, 1, 1, 1, -1, -1, 1},
    {0, -1, -1, -1, 0, -1, 0, 0, 0, -1, -1, 0},
    {-1, 0, 1, -1, 0, -1, 1, 1, 0, -1, -1, 0},
    {-1, 0, -1, 0, -1, 0, 0, 0, -1, -1, -1, 0},
    {0, -1, 1, 0, -1, 0, 1, 1, -1, -1, -1, 0},
    {0, 0, -1, 1, 1, -1, 0, 0, -1, -1, -1, 1},
    {-1, -1, 0, 1, 1, -1, 0, 0, -1, -1, -1, 1},
    {-1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, -1, -1, -1},
    {0, -1, -1, 0, -1, -1, 0, 0, 0, -1, -1, -1},
    {-1, 1, 0, 1, -1, -1, 0, 0, 1, -1, -1, -1},
    {-1, 0, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1},
    {0, -1, 1, -1, 0, 0, 1, 1, -1, -1, -1, -1},
    {0, 0, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1},
    {-1, -1, 0, -1, -1, -1, 0, 0, -1, -1, -1, -1},
    {-1, -1, 0, -1, -1, 0, -1, -1, 0, -1, -1, 0},
    {0, 0, -1, -1, -1, 0, -1, -1, 0, -1, -1, 0},
    {0, -1, 0, -1, 0, -1, -1, -1, 0, -1, -1, 0},
    {-1, 0, -1, -1, 0, -1, -1, -1, 0, -1, -1, 0},
    {-1, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, 0},
    {0, -1, -1, 0, -1, 0, -1, -1, -1, -1, -1, 0},
    {0, 0, 0, 1, 1, -1, -1, -1, -1, -1, -1, 1},
    {-1, -1, -1, 0, 0, -1, -1, -1, -1, -1, -1, 0},
    {-1, -1, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1},
    {0, 1, -1, 1, 0, 0, -1, -1, 1, -1, -1, -1},
    {0, -1, 0, 0, -1, -1, -1, -1, 0, -1, -1, -1},
    {-1, 0, -1, 0, -1, -1, -1, -1, 0, -1, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1},
    {0, -1, -1, -1, 0, 0, -1, -1, -1, -1, -1, -1},
    {0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

  static int vertexIndicesByEdge[12][2] = {{0, 1}, {0, 2}, {0, 4}, {2, 3}, {1, 3}, {1, 5},
                                           {4, 5}, {4, 6}, {2, 6}, {6, 7}, {5, 7}, {3, 7}};

  static int edgeIndicesByFace[6][4] = {{0, 2, 5, 6},   {3, 8, 9, 11}, {1, 2, 7, 8},
                                        {4, 5, 10, 11}, {0, 1, 3, 4},  {6, 7, 9, 10}};

  unsigned int numVertices (unsigned int configuration)
  {
    assert (configuration < 256);

    int n = -1;
    for (unsigned int i = 0; i < 12; i++)
    {
      n = glm::max (n, vertexIndicesByConfiguration[configuration][i]);
    }
    return n + 1;
  }

  struct Cube
  {
    unsigned int              configuration;
    glm::vec3                 vertex;
    std::vector<unsigned int> vertexIndicesInMesh;
    bool                      nonManifold;

    Cube ()
      : configuration (Util::invalidIndex ())
      , vertex (invalidVec3)
      , nonManifold (false)
    {
    }

#ifndef NDEBUG
    unsigned int configurationBase () const
    {
      assert (this->configuration < 256);
      return ::configurationBase[this->configuration];
    }
#endif

    bool nonManifoldConfig () const
    {
      assert (this->configuration < 256);
      return ::nonManifoldConfig[this->configuration];
    }

    bool collapseNonManifoldConfig () const
    {
      return this->nonManifoldConfig () && this->nonManifold == false;
    }

    unsigned int vertexIndex (unsigned int edge) const
    {
      assert (edge < 12);
      assert (this->configuration < 256);
      assert (vertexIndicesByConfiguration[this->configuration][edge] >= 0);
      assert (this->nonManifold == false || this->nonManifoldConfig ());

      unsigned int i;
      if (this->collapseNonManifoldConfig ())
      {
        i = 0;
      }
      else
      {
        i = (unsigned int) vertexIndicesByConfiguration[this->configuration][edge];
      }
      assert (i < this->vertexIndicesInMesh.size ());

      return this->vertexIndicesInMesh.at (i);
    }

    unsigned int getAmbiguousFaceOfNonManifoldConfig () const
    {
      assert (this->configuration < 256);
      assert (this->nonManifoldConfig ());

      for (unsigned int i = 0; i < 6; i++)
      {
        const unsigned int edge1 = edgeIndicesByFace[i][0];
        const unsigned int edge2 = edgeIndicesByFace[i][1];
        const unsigned int edge3 = edgeIndicesByFace[i][2];
        const unsigned int edge4 = edgeIndicesByFace[i][3];

        const int vertex1 = vertexIndicesByConfiguration[this->configuration][edge1];
        const int vertex2 = vertexIndicesByConfiguration[this->configuration][edge2];
        const int vertex3 = vertexIndicesByConfiguration[this->configuration][edge3];
        const int vertex4 = vertexIndicesByConfiguration[this->configuration][edge4];

        if (vertex1 != -1 && vertex2 != -1 && vertex3 != -1 && vertex4 != -1)
        {
          return i;
        }
      }
      DILAY_IMPOSSIBLE
    }
  };

  struct Parameters
  {
    float              resolution;
    glm::vec3          sampleOrigin;
    glm::uvec3         numSamples;
    std::vector<float> samples;
    glm::uvec3         numCubes;
    std::vector<Cube>  grid;

    Parameters ()
      : resolution (0.0f)
      , sampleOrigin (glm::vec3 (0.0f))
      , numSamples (glm::uvec3 (0))
    {
    }

    glm::vec3 samplePos (unsigned int x, unsigned int y, unsigned int z) const
    {
      assert (x < (unsigned int) this->numSamples.x);
      assert (y < (unsigned int) this->numSamples.y);
      assert (z < (unsigned int) this->numSamples.z);

      return this->sampleOrigin +
             (glm::vec3 (this->resolution) * glm::vec3 (float(x), float(y), float(z)));
    }

    glm::vec3 samplePos (unsigned int i) const
    {
      const std::div_t divZ = std::div (int(i), int(this->numSamples.x * this->numSamples.y));
      const std::div_t divY = std::div (divZ.rem, int(this->numSamples.x));

      const unsigned int x = (unsigned int) (divY.rem);
      const unsigned int y = (unsigned int) (divY.quot);
      const unsigned int z = (unsigned int) (divZ.quot);

      assert (this->sampleIndex (x, y, z) == i);

      return this->samplePos (x, y, z);
    }

    unsigned int sampleIndex (unsigned int x, unsigned int y, unsigned int z) const
    {
      return (z * this->numSamples.x * this->numSamples.y) + (y * this->numSamples.x) + x;
    }

    unsigned int sampleIndex (unsigned int cubeIndex, unsigned int vertex) const
    {
      assert (vertex < 8);

      const std::div_t   divZ = std::div (int(cubeIndex), int(this->numCubes.x * this->numCubes.y));
      const std::div_t   divY = std::div (divZ.rem, int(this->numCubes.x));
      const unsigned int x = (unsigned int) (divY.rem);
      const unsigned int y = (unsigned int) (divY.quot);
      const unsigned int z = (unsigned int) (divZ.quot);

      assert (this->cubeIndex (x, y, z) == cubeIndex);

      switch (vertex)
      {
        case 0:
          return this->sampleIndex (x, y, z);
        case 1:
          return this->sampleIndex (x + 1, y, z);
        case 2:
          return this->sampleIndex (x, y + 1, z);
        case 3:
          return this->sampleIndex (x + 1, y + 1, z);
        case 4:
          return this->sampleIndex (x, y, z + 1);
        case 5:
          return this->sampleIndex (x + 1, y, z + 1);
        case 6:
          return this->sampleIndex (x, y + 1, z + 1);
        case 7:
          return this->sampleIndex (x + 1, y + 1, z + 1);
        default:
          DILAY_IMPOSSIBLE
      }
    }

    unsigned int cubeIndex (unsigned int x, unsigned int y, unsigned int z) const
    {
      return (z * this->numCubes.x * this->numCubes.y) + (y * this->numCubes.x) + x;
    }
  };

  void setupSampling (const SketchMesh& mesh, Parameters& params)
  {
    glm::vec3 min, max;
    mesh.minMax (min, max);

    min = min - glm::vec3 (Util::epsilon () + params.resolution);
    max = max + glm::vec3 (Util::epsilon () + params.resolution);

    params.sampleOrigin = min;
    params.numSamples = glm::vec3 (1.0f) + glm::ceil ((max - min) / glm::vec3 (params.resolution));
  }

  float sampleAt (const SketchMesh& mesh, const glm::vec3& pos)
  {
    float distance = Util::maxFloat ();

    if (mesh.tree ().hasRoot ())
    {
      mesh.tree ().root ().forEachConstNode ([&pos, &distance](const SketchNode& node) {
        const float d =
          node.parent ()
            ? Distance::distance (PrimConeSphere (node.data (), node.parent ()->data ()), pos)
            : Distance::distance (node.data (), pos);

        distance = glm::min (distance, d);
      });
    }
    for (const SketchPath& p : mesh.paths ())
    {
      for (const PrimSphere& s : p.spheres ())
      {
        distance = glm::min (distance, Distance::distance (s, pos));
      }
    }
    return distance;
  }

  void sampleThread (const SketchMesh& mesh, Parameters& params, unsigned int numThreads,
                     unsigned int threadId)
  {
    for (unsigned int z = 0; z < params.numSamples.z; z++)
    {
      for (unsigned int y = 0; y < params.numSamples.y; y++)
      {
        for (unsigned int x = 0; x < params.numSamples.x; x++)
        {
          const unsigned int index = params.sampleIndex (x, y, z);

          if (index % numThreads == threadId)
          {
            const glm::vec3 pos = params.samplePos (x, y, z);

            assert (params.samples.at (index) == Util::maxFloat ());
            params.samples.at (index) = sampleAt (mesh, pos);

            assert (Util::isNaN (params.samples.at (index)) == false);
            assert (params.samples.at (index) != Util::maxFloat ());
            assert ((x > 0 && x < params.numSamples.x - 1) || params.samples.at (index) > 0.0f);
            assert ((y > 0 && y < params.numSamples.y - 1) || params.samples.at (index) > 0.0f);
            assert ((z > 0 && z < params.numSamples.z - 1) || params.samples.at (index) > 0.0f);
          }
        }
      }
    }
  }

  void sample (const SketchMesh& mesh, Parameters& params)
  {
    const unsigned int numSamples = params.numSamples.x * params.numSamples.y * params.numSamples.z;
    params.samples.resize (numSamples, Util::maxFloat ());

    const unsigned int       numThreads = std::thread::hardware_concurrency ();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.emplace_back (sampleThread, std::ref (mesh), std::ref (params), numThreads, i);
    }
    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.at (i).join ();
    }
  }

  bool isIntersecting (float s1, float s2)
  {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }

  void setCubeVertex (Parameters& params, unsigned int cubeIndex)
  {
    glm::vec3    vertex = glm::vec3 (0.0f);
    unsigned int numCrossedEdges = 0;
    Cube&        cube = params.grid.at (cubeIndex);

    const unsigned int indices[] = {
      params.sampleIndex (cubeIndex, 0), params.sampleIndex (cubeIndex, 1),
      params.sampleIndex (cubeIndex, 2), params.sampleIndex (cubeIndex, 3),
      params.sampleIndex (cubeIndex, 4), params.sampleIndex (cubeIndex, 5),
      params.sampleIndex (cubeIndex, 6), params.sampleIndex (cubeIndex, 7)};

    const float samples[] = {params.samples.at (indices[0]), params.samples.at (indices[1]),
                             params.samples.at (indices[2]), params.samples.at (indices[3]),
                             params.samples.at (indices[4]), params.samples.at (indices[5]),
                             params.samples.at (indices[6]), params.samples.at (indices[7])};

    const glm::vec3 positions[] = {params.samplePos (indices[0]), params.samplePos (indices[1]),
                                   params.samplePos (indices[2]), params.samplePos (indices[3]),
                                   params.samplePos (indices[4]), params.samplePos (indices[5]),
                                   params.samplePos (indices[6]), params.samplePos (indices[7])};

    auto checkEdge = [&numCrossedEdges, &vertex, &samples, &positions](unsigned short vertex1,
                                                                       unsigned short vertex2) {
      if (isIntersecting (samples[vertex1], samples[vertex2]))
      {
        const float     factor = samples[vertex1] / (samples[vertex1] - samples[vertex2]);
        const glm::vec3 delta = positions[vertex2] - positions[vertex1];

        vertex += positions[vertex1] + (delta * factor);
        numCrossedEdges++;
      }
    };

    assert (cube.configuration == Util::invalidIndex ());

    cube.configuration = 0;
    for (unsigned int edge = 0; edge < 12; edge++)
    {
      const unsigned int vertex1 = vertexIndicesByEdge[edge][0];
      const unsigned int vertex2 = vertexIndicesByEdge[edge][1];

      if (samples[vertex1] < 0.0f)
      {
        cube.configuration |= (1 << vertex1);
      }

      if (samples[vertex2] < 0.0f)
      {
        cube.configuration |= (1 << vertex2);
      }
      checkEdge (vertex1, vertex2);
    }
    assert (cube.configuration < 256);

#ifndef NDEBUG
    for (unsigned int edge = 0; edge < 12; edge++)
    {
      const unsigned int vertex1 = vertexIndicesByEdge[edge][0];
      const unsigned int vertex2 = vertexIndicesByEdge[edge][1];

      if (samples[vertex1] < 0.0f)
      {
        assert ((cube.configuration & (1 << vertex1)) != 0);
      }
      else
      {
        assert ((cube.configuration & (1 << vertex1)) == 0);
      }

      if (samples[vertex2] < 0.0f)
      {
        assert ((cube.configuration & (1 << vertex2)) != 0);
      }
      else
      {
        assert ((cube.configuration & (1 << vertex2)) == 0);
      }

      if (isIntersecting (samples[vertex1], samples[vertex2]))
      {
        assert (vertexIndicesByConfiguration[cube.configuration][edge] != -1);
      }
    }
#endif

    if (numCrossedEdges > 0)
    {
      cube.vertex = vertex / float(numCrossedEdges);
      cube.vertexIndicesInMesh.resize (numVertices (cube.configuration), Util::invalidIndex ());
    }
  }

  void makeGrid (Parameters& params)
  {
    params.numCubes = params.numSamples - glm::uvec3 (1);
    params.grid.resize (params.numCubes.x * params.numCubes.y * params.numCubes.z);

    for (unsigned int z = 0; z < params.numCubes.z; z++)
    {
      for (unsigned int y = 0; y < params.numCubes.y; y++)
      {
        for (unsigned int x = 0; x < params.numCubes.x; x++)
        {
          setCubeVertex (params, params.cubeIndex (x, y, z));
        }
      }
    }

#ifndef NDEBUG
    for (unsigned int z = 0; z < params.numCubes.z; z++)
    {
      for (unsigned int y = 0; y < params.numCubes.y; y++)
      {
        for (unsigned int x = 0; x < params.numCubes.x; x++)
        {
          unsigned int config = params.grid.at (params.cubeIndex (x, y, z)).configuration;

          if (x > 0)
          {
            unsigned int left = params.grid.at (params.cubeIndex (x - 1, y, z)).configuration;

            assert (((config & (1 << 0)) == 0) == ((left & (1 << 1)) == 0));
            assert (((config & (1 << 2)) == 0) == ((left & (1 << 3)) == 0));
            assert (((config & (1 << 4)) == 0) == ((left & (1 << 5)) == 0));
            assert (((config & (1 << 6)) == 0) == ((left & (1 << 7)) == 0));
          }
          if (y > 0)
          {
            unsigned int below = params.grid.at (params.cubeIndex (x, y - 1, z)).configuration;

            assert (((config & (1 << 0)) == 0) == ((below & (1 << 2)) == 0));
            assert (((config & (1 << 1)) == 0) == ((below & (1 << 3)) == 0));
            assert (((config & (1 << 4)) == 0) == ((below & (1 << 6)) == 0));
            assert (((config & (1 << 5)) == 0) == ((below & (1 << 7)) == 0));
          }
          if (z > 0)
          {
            unsigned int behind = params.grid.at (params.cubeIndex (x, y, z - 1)).configuration;

            assert (((config & (1 << 0)) == 0) == ((behind & (1 << 4)) == 0));
            assert (((config & (1 << 1)) == 0) == ((behind & (1 << 5)) == 0));
            assert (((config & (1 << 2)) == 0) == ((behind & (1 << 6)) == 0));
            assert (((config & (1 << 3)) == 0) == ((behind & (1 << 7)) == 0));
          }
        }
      }
    }
#endif
  }

  void resolveNonManifolds (Parameters& params)
  {
    auto check = [&params](const Cube& cube, unsigned int x, unsigned int y, unsigned int z,
                           unsigned int ambiguousFace, int dim) -> bool {
      assert (cube.nonManifoldConfig ());
      assert (dim == -3 || dim == -2 || dim == -1 || dim == 1 || dim == 2 || dim == 3);
      unused (cube);

      Cube& other = params.grid.at (params.cubeIndex (dim == -1 ? x - 1 : (dim == 1 ? x + 1 : x),
                                                      dim == -2 ? y - 1 : (dim == 2 ? y + 1 : y),
                                                      dim == -3 ? z - 1 : (dim == 3 ? z + 1 : z)));
      if (other.nonManifoldConfig ())
      {
        const unsigned int otherAmbiguousFace = other.getAmbiguousFaceOfNonManifoldConfig ();

        const bool nx = dim == -1 && ambiguousFace == 2 && otherAmbiguousFace == 3;
        const bool px = dim == 1 && ambiguousFace == 3 && otherAmbiguousFace == 2;
        const bool ny = dim == -2 && ambiguousFace == 0 && otherAmbiguousFace == 1;
        const bool py = dim == 2 && ambiguousFace == 1 && otherAmbiguousFace == 0;
        const bool nz = dim == -3 && ambiguousFace == 4 && otherAmbiguousFace == 5;
        const bool pz = dim == 3 && ambiguousFace == 5 && otherAmbiguousFace == 4;

        return nx || px || ny || py || nz || pz;
      }
      else
      {
        return false;
      }
    };

    for (unsigned int z = 0; z < params.numCubes.z; z++)
    {
      for (unsigned int y = 0; y < params.numCubes.y; y++)
      {
        for (unsigned int x = 0; x < params.numCubes.x; x++)
        {
          Cube& cube = params.grid.at (params.cubeIndex (x, y, z));

          if (cube.nonManifoldConfig ())
          {
            const unsigned int ambiguousFace = cube.getAmbiguousFaceOfNonManifoldConfig ();

            const bool nx = x > 0 && check (cube, x, y, z, ambiguousFace, -1);
            const bool px = x < params.numCubes.x - 1 && check (cube, x, y, z, ambiguousFace, 1);
            const bool ny = y > 0 && check (cube, x, y, z, ambiguousFace, -2);
            const bool py = y < params.numCubes.y - 1 && check (cube, x, y, z, ambiguousFace, 2);
            const bool nz = z > 0 && check (cube, x, y, z, ambiguousFace, -3);
            const bool pz = z < params.numCubes.z - 1 && check (cube, x, y, z, ambiguousFace, 3);

            cube.nonManifold = nx || px || ny || py || nz || pz;
          }
          else
          {
            cube.nonManifold = false;
          }
        }
      }
    }
  }

  Mesh makeMesh (Parameters& params)
  {
    Mesh mesh;

    for (Cube& cube : params.grid)
    {
      for (unsigned int i = 0; i < cube.vertexIndicesInMesh.size (); i++)
      {
        assert (cube.vertex != invalidVec3);
        cube.vertexIndicesInMesh.at (i) = mesh.addVertex (cube.vertex);

        if (cube.collapseNonManifoldConfig ())
        {
          break;
        }
      }
    }

    auto makeQuad = [&params, &mesh](unsigned int edge, bool swap, unsigned int i, unsigned int iu,
                                     unsigned int iv, unsigned int iuv) {
      unsigned int v1, v2, v3, v4;

      if (edge == 0)
      {
        v1 = params.grid.at (i).vertexIndex (0);
        v2 = params.grid.at (iu).vertexIndex (3);
        v3 = params.grid.at (iuv).vertexIndex (9);
        v4 = params.grid.at (iv).vertexIndex (6);
      }
      else if (edge == 1)
      {
        v1 = params.grid.at (i).vertexIndex (1);
        v2 = params.grid.at (iu).vertexIndex (7);
        v3 = params.grid.at (iuv).vertexIndex (10);
        v4 = params.grid.at (iv).vertexIndex (4);
      }
      else if (edge == 2)
      {
        v1 = params.grid.at (i).vertexIndex (2);
        v2 = params.grid.at (iu).vertexIndex (5);
        v3 = params.grid.at (iuv).vertexIndex (11);
        v4 = params.grid.at (iv).vertexIndex (8);
      }
      else
      {
        DILAY_IMPOSSIBLE
      }

      if (swap)
      {
        std::swap (v2, v4);
      }

      if (glm::distance2 (mesh.vertex (v1), mesh.vertex (v3)) <=
          glm::distance2 (mesh.vertex (v2), mesh.vertex (v4)))
      {
        mesh.addIndex (v1);
        mesh.addIndex (v2);
        mesh.addIndex (v3);
        mesh.addIndex (v1);
        mesh.addIndex (v3);
        mesh.addIndex (v4);
      }
      else
      {
        mesh.addIndex (v2);
        mesh.addIndex (v3);
        mesh.addIndex (v4);
        mesh.addIndex (v2);
        mesh.addIndex (v4);
        mesh.addIndex (v1);
      }
    };

    auto makeFaces = [&params, &makeQuad](unsigned int edge, unsigned int x, unsigned int y,
                                          unsigned int z) {
      assert (edge == 0 || edge == 1 || edge == 2);

      const float s1 = params.samples.at (params.sampleIndex (x, y, z));
      const float s2 = params.samples.at (
        params.sampleIndex (edge == 0 ? x + 1 : x, edge == 1 ? y + 1 : y, edge == 2 ? z + 1 : z));

      if (isIntersecting (s1, s2))
      {
        const unsigned int i = params.cubeIndex (x, y, z);

        unsigned int iu, iv, iuv;
        if (edge == 0)
        {
          iu = params.cubeIndex (x, y - 1, z);
          iv = params.cubeIndex (x, y, z - 1);
          iuv = params.cubeIndex (x, y - 1, z - 1);
        }
        else if (edge == 1)
        {
          iu = params.cubeIndex (x, y, z - 1);
          iv = params.cubeIndex (x - 1, y, z);
          iuv = params.cubeIndex (x - 1, y, z - 1);
        }
        else if (edge == 2)
        {
          iu = params.cubeIndex (x - 1, y, z);
          iv = params.cubeIndex (x, y - 1, z);
          iuv = params.cubeIndex (x - 1, y - 1, z);
        }
        else
        {
          DILAY_IMPOSSIBLE
        }
        makeQuad (edge, s1 >= 0.0f, i, iu, iv, iuv);
      }
    };

    for (unsigned int z = 0; z < params.numCubes.z; z++)
    {
      for (unsigned int y = 0; y < params.numCubes.y; y++)
      {
        for (unsigned int x = 0; x < params.numCubes.x; x++)
        {
          if (y > 0 && z > 0)
          {
            makeFaces (0, x, y, z);
          }
          if (x > 0 && z > 0)
          {
            makeFaces (1, x, y, z);
          }
          if (x > 0 && y > 0)
          {
            makeFaces (2, x, y, z);
          }
        }
      }
    }

    assert (MeshUtil::checkConsistency (mesh));
    return mesh;
  }
}

Mesh SketchConversion::convert (const SketchMesh& mesh, float resolution)
{
  assert (mesh.isEmpty () == false);

  Parameters params;
  params.resolution = resolution;

  setupSampling (mesh, params);

  if (params.numSamples.x > 0 && params.numSamples.y > 0 && params.numSamples.z > 0)
  {
    sample (mesh, params);
    makeGrid (params);
    resolveNonManifolds (params);
    return makeMesh (params);
  }
  else
  {
    return Mesh ();
  }
}
