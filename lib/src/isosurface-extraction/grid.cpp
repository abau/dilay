/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <array>
#include <glm/gtx/norm.hpp>
#include "dynamic/mesh.hpp"
#include "isosurface-extraction/grid.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
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
  static const glm::vec3 invalidVec3 = glm::vec3 (Util::minFloat ());

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

  static char vertexIndicesByConfiguration[256][12] = {
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

  static unsigned char edgeIndicesByFace[6][4] = {{0, 2, 5, 6},   {3, 8, 9, 11}, {1, 2, 7, 8},
                                                  {4, 5, 10, 11}, {0, 1, 3, 4},  {6, 7, 9, 10}};

  unsigned char numVertices (unsigned char configuration)
  {
    char n = -1;
    for (unsigned char i = 0; i < 12; i++)
    {
      n = std::max (n, vertexIndicesByConfiguration[configuration][i]);
    }
    return (unsigned char) (n + 1);
  }

  bool isIntersecting (float s1, float s2)
  {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }

  struct Cube
  {
    unsigned char               configuration;
    glm::vec3                   vertex;
    unsigned char               numVertexIndicesInMesh;
    std::array<unsigned int, 3> vertexIndicesInMesh;
    bool                        nonManifold;

    Cube ()
      : configuration (0)
      , vertex (invalidVec3)
      , numVertexIndicesInMesh (0)
      , nonManifold (false)
    {
    }

    bool nonManifoldConfig () const { return ::nonManifoldConfig[this->configuration]; }

    bool collapseNonManifoldConfig () const
    {
      return this->nonManifoldConfig () && this->nonManifold == false;
    }

    unsigned int vertexIndex (unsigned char edge) const
    {
      assert (edge < 12);
      assert (vertexIndicesByConfiguration[this->configuration][edge] >= 0);
      assert (this->nonManifold == false || this->nonManifoldConfig ());

      const unsigned char i = this->collapseNonManifoldConfig ()
                                ? 0
                                : vertexIndicesByConfiguration[this->configuration][edge];
      assert (i < this->vertexIndicesInMesh.size ());
      assert (i < this->numVertexIndicesInMesh);

      return this->vertexIndicesInMesh[i];
    }

    unsigned char getAmbiguousFaceOfNonManifoldConfig () const
    {
      assert (this->nonManifoldConfig ());

      for (unsigned int i = 0; i < 6; i++)
      {
        const unsigned char edge1 = edgeIndicesByFace[i][0];
        const unsigned char edge2 = edgeIndicesByFace[i][1];
        const unsigned char edge3 = edgeIndicesByFace[i][2];
        const unsigned char edge4 = edgeIndicesByFace[i][3];

        const char vertex1 = vertexIndicesByConfiguration[this->configuration][edge1];
        const char vertex2 = vertexIndicesByConfiguration[this->configuration][edge2];
        const char vertex3 = vertexIndicesByConfiguration[this->configuration][edge3];
        const char vertex4 = vertexIndicesByConfiguration[this->configuration][edge4];

        if (vertex1 != -1 && vertex2 != -1 && vertex3 != -1 && vertex4 != -1)
        {
          return (unsigned char) i;
        }
      }
      DILAY_IMPOSSIBLE
    }
  };
}

const unsigned char IsosurfaceExtractionGrid::vertexIndicesByEdge[12][2] = {
  {0, 1}, {0, 2}, {0, 4}, {2, 3}, {1, 3}, {1, 5}, {4, 5}, {4, 6}, {2, 6}, {6, 7}, {5, 7}, {3, 7}};

struct IsosurfaceExtractionGrid::Impl
{
  float              resolution;
  glm::vec3          sampleMin;
  glm::vec3          sampleMax;
  glm::uvec3         numSamples;
  std::vector<float> samples;
  glm::uvec3         numCubes;
  std::vector<Cube>  cubes;

  Impl (const PrimAABox& bounds, float r)
    : resolution (r)
  {
    const glm::vec3 min = bounds.minimum () - glm::vec3 (Util::epsilon () + r);
    const glm::vec3 max = bounds.maximum () + glm::vec3 (Util::epsilon () + r);

    this->sampleMin = min;
    this->numSamples = glm::vec3 (1.0f) + glm::ceil ((max - min) / glm::vec3 (r));
    this->numCubes = this->numSamples - glm::uvec3 (1);

    const unsigned int totalNumSamples =
      this->numSamples.x * this->numSamples.y * this->numSamples.z;
    const unsigned int totalNumCubes = this->numCubes.x * this->numCubes.y * this->numCubes.z;

    this->samples.resize (totalNumSamples, Util::maxFloat ());
    this->cubes.resize (totalNumCubes);
  }

  glm::vec3 samplePos (unsigned int x, unsigned int y, unsigned int z) const
  {
    assert (x < (unsigned int) this->numSamples.x);
    assert (y < (unsigned int) this->numSamples.y);
    assert (z < (unsigned int) this->numSamples.z);

    return this->sampleMin +
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

  unsigned int sampleIndex (unsigned int cubeIndex, unsigned char vertex) const
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

  unsigned int cubeVertexIndex (unsigned int cubeIndex, unsigned char edge) const
  {
    return this->cubes[cubeIndex].vertexIndex (edge);
  }

  void setCubeVertex (unsigned int cubeIndex)
  {
    glm::vec3    vertex = glm::vec3 (0.0f);
    unsigned int numCrossedEdges = 0;
    Cube&        cube = this->cubes[cubeIndex];

    const unsigned int indices[] = {
      this->sampleIndex (cubeIndex, 0), this->sampleIndex (cubeIndex, 1),
      this->sampleIndex (cubeIndex, 2), this->sampleIndex (cubeIndex, 3),
      this->sampleIndex (cubeIndex, 4), this->sampleIndex (cubeIndex, 5),
      this->sampleIndex (cubeIndex, 6), this->sampleIndex (cubeIndex, 7)};

    const float samples[] = {this->samples[indices[0]], this->samples[indices[1]],
                             this->samples[indices[2]], this->samples[indices[3]],
                             this->samples[indices[4]], this->samples[indices[5]],
                             this->samples[indices[6]], this->samples[indices[7]]};

    const glm::vec3 positions[] = {this->samplePos (indices[0]), this->samplePos (indices[1]),
                                   this->samplePos (indices[2]), this->samplePos (indices[3]),
                                   this->samplePos (indices[4]), this->samplePos (indices[5]),
                                   this->samplePos (indices[6]), this->samplePos (indices[7])};

    cube.configuration = 0;
    for (unsigned char edge = 0; edge < 12; edge++)
    {
      const unsigned char vertex1 = vertexIndicesByEdge[edge][0];
      const unsigned char vertex2 = vertexIndicesByEdge[edge][1];

      cube.configuration |= ((samples[vertex1] < 0.0f) << vertex1);
      cube.configuration |= ((samples[vertex2] < 0.0f) << vertex2);

      if (isIntersecting (samples[vertex1], samples[vertex2]))
      {
        const float     factor = samples[vertex1] / (samples[vertex1] - samples[vertex2]);
        const glm::vec3 delta = positions[vertex2] - positions[vertex1];

        vertex += positions[vertex1] + (delta * factor);
        numCrossedEdges++;
      }
    }

#ifndef NDEBUG
    for (unsigned char edge = 0; edge < 12; edge++)
    {
      const unsigned char vertex1 = vertexIndicesByEdge[edge][0];
      const unsigned char vertex2 = vertexIndicesByEdge[edge][1];

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
    }
    else
    {
      assert (numVertices (cube.configuration) == 0);
    }
  }

  void setCubeVertices ()
  {
    for (unsigned int z = 0; z < this->numCubes.z; z++)
    {
      for (unsigned int y = 0; y < this->numCubes.y; y++)
      {
        for (unsigned int x = 0; x < this->numCubes.x; x++)
        {
          this->setCubeVertex (this->cubeIndex (x, y, z));
        }
      }
    }

#ifndef NDEBUG
    for (unsigned int z = 0; z < this->numCubes.z; z++)
    {
      for (unsigned int y = 0; y < this->numCubes.y; y++)
      {
        for (unsigned int x = 0; x < this->numCubes.x; x++)
        {
          unsigned char config = this->cubes[this->cubeIndex (x, y, z)].configuration;

          if (x > 0)
          {
            unsigned char left = this->cubes[this->cubeIndex (x - 1, y, z)].configuration;

            assert (((config & (1 << 0)) == 0) == ((left & (1 << 1)) == 0));
            assert (((config & (1 << 2)) == 0) == ((left & (1 << 3)) == 0));
            assert (((config & (1 << 4)) == 0) == ((left & (1 << 5)) == 0));
            assert (((config & (1 << 6)) == 0) == ((left & (1 << 7)) == 0));
          }
          if (y > 0)
          {
            unsigned char below = this->cubes[this->cubeIndex (x, y - 1, z)].configuration;

            assert (((config & (1 << 0)) == 0) == ((below & (1 << 2)) == 0));
            assert (((config & (1 << 1)) == 0) == ((below & (1 << 3)) == 0));
            assert (((config & (1 << 4)) == 0) == ((below & (1 << 6)) == 0));
            assert (((config & (1 << 5)) == 0) == ((below & (1 << 7)) == 0));
          }
          if (z > 0)
          {
            unsigned char behind = this->cubes[this->cubeIndex (x, y, z - 1)].configuration;

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

  bool hasAmbiguousNeighbor (const Cube& cube, unsigned int x, unsigned int y, unsigned int z,
                             unsigned char ambiguousFace, char dim)
  {
    assert (cube.nonManifoldConfig ());
    assert (dim == -3 || dim == -2 || dim == -1 || dim == 1 || dim == 2 || dim == 3);
    unused (cube);

    Cube& other = this->cubes[this->cubeIndex (dim == -1 ? x - 1 : (dim == 1 ? x + 1 : x),
                                               dim == -2 ? y - 1 : (dim == 2 ? y + 1 : y),
                                               dim == -3 ? z - 1 : (dim == 3 ? z + 1 : z))];
    if (other.nonManifoldConfig ())
    {
      const unsigned char otherAmbiguousFace = other.getAmbiguousFaceOfNonManifoldConfig ();

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
  }

  void resolveNonManifold (unsigned int x, unsigned int y, unsigned int z)
  {
    Cube& cube = this->cubes[this->cubeIndex (x, y, z)];

    if (cube.nonManifoldConfig ())
    {
      const unsigned char ambiguousFace = cube.getAmbiguousFaceOfNonManifoldConfig ();

      const bool nx = x > 0 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, -1);
      const bool px =
        x < this->numCubes.x - 1 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, 1);
      const bool ny = y > 0 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, -2);
      const bool py =
        y < this->numCubes.y - 1 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, 2);
      const bool nz = z > 0 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, -3);
      const bool pz =
        z < this->numCubes.z - 1 && this->hasAmbiguousNeighbor (cube, x, y, z, ambiguousFace, 3);

      cube.nonManifold = nx || px || ny || py || nz || pz;
    }
    else
    {
      cube.nonManifold = false;
    }
  }

  void resolveNonManifolds ()
  {
    for (unsigned int z = 0; z < this->numCubes.z; z++)
    {
      for (unsigned int y = 0; y < this->numCubes.y; y++)
      {
        for (unsigned int x = 0; x < this->numCubes.x; x++)
        {
          this->resolveNonManifold (x, y, z);
        }
      }
    }
  }

  void addCubeVerticesToMesh (Cube& cube, DynamicMesh& mesh)
  {
    cube.numVertexIndicesInMesh =
      cube.collapseNonManifoldConfig () ? 1 : numVertices (cube.configuration);

    for (unsigned char i = 0; i < cube.numVertexIndicesInMesh; i++)
    {
      cube.vertexIndicesInMesh[i] = mesh.addVertex (cube.vertex, glm::vec3 (0.0f));
    }
#ifndef NDEBUG
    for (unsigned char i = cube.numVertexIndicesInMesh; i < cube.vertexIndicesInMesh.size (); i++)
    {
      cube.vertexIndicesInMesh[i] = Util::invalidIndex ();
    }
#endif
  }

  void addQuadToMesh (DynamicMesh& mesh, unsigned int i, unsigned int iu, unsigned int iv,
                      unsigned int iuv)
  {
    if (glm::distance2 (mesh.vertex (i), mesh.vertex (iuv)) <=
        glm::distance2 (mesh.vertex (iu), mesh.vertex (iv)))
    {
      mesh.addFace (i, iu, iuv);
      mesh.addFace (i, iuv, iv);
    }
    else
    {
      mesh.addFace (iu, iuv, iv);
      mesh.addFace (iu, iv, i);
    }
  }

  void makeFaces (DynamicMesh& mesh, unsigned char edge, unsigned int x, unsigned int y,
                  unsigned int z)
  {
    assert (edge == 0 || edge == 1 || edge == 2);

    const float s1 = this->samples[this->sampleIndex (x, y, z)];
    const float s2 = this->samples[this->sampleIndex (edge == 0 ? x + 1 : x, edge == 1 ? y + 1 : y,
                                                      edge == 2 ? z + 1 : z)];

    if (isIntersecting (s1, s2))
    {
      unsigned int i, iu, iv, iuv;

      if (edge == 0)
      {
        i = this->cubeVertexIndex (this->cubeIndex (x, y, z), 0);
        iu = this->cubeVertexIndex (this->cubeIndex (x, y - 1, z), 3);
        iuv = this->cubeVertexIndex (this->cubeIndex (x, y - 1, z - 1), 9);
        iv = this->cubeVertexIndex (this->cubeIndex (x, y, z - 1), 6);
      }
      else if (edge == 1)
      {
        i = this->cubeVertexIndex (this->cubeIndex (x, y, z), 1);
        iu = this->cubeVertexIndex (this->cubeIndex (x, y, z - 1), 7);
        iuv = this->cubeVertexIndex (this->cubeIndex (x - 1, y, z - 1), 10);
        iv = this->cubeVertexIndex (this->cubeIndex (x - 1, y, z), 4);
      }
      else if (edge == 2)
      {
        i = this->cubeVertexIndex (this->cubeIndex (x, y, z), 2);
        iu = this->cubeVertexIndex (this->cubeIndex (x - 1, y, z), 5);
        iuv = this->cubeVertexIndex (this->cubeIndex (x - 1, y - 1, z), 11);
        iv = this->cubeVertexIndex (this->cubeIndex (x, y - 1, z), 8);
      }
      else
      {
        DILAY_IMPOSSIBLE
      }

      if (s1 >= 0.0f)
      {
        std::swap (iu, iv);
      }

      this->addQuadToMesh (mesh, i, iu, iv, iuv);
    }
  }

  void makeFaces (DynamicMesh& mesh, unsigned int x, unsigned int y, unsigned int z)
  {
    if (y > 0 && z > 0)
    {
      this->makeFaces (mesh, 0, x, y, z);
    }
    if (x > 0 && z > 0)
    {
      this->makeFaces (mesh, 1, x, y, z);
    }
    if (x > 0 && y > 0)
    {
      this->makeFaces (mesh, 2, x, y, z);
    }
  }

  void makeMesh (DynamicMesh& mesh)
  {
    this->setCubeVertices ();
    this->resolveNonManifolds ();

    mesh.reset ();
    for (Cube& cube : this->cubes)
    {
      this->addCubeVerticesToMesh (cube, mesh);
    }

    for (unsigned int z = 0; z < this->numCubes.z; z++)
    {
      for (unsigned int y = 0; y < this->numCubes.y; y++)
      {
        for (unsigned int x = 0; x < this->numCubes.x; x++)
        {
          this->makeFaces (mesh, x, y, z);
        }
      }
    }
    mesh.setAllNormals ();

#ifndef NDEBUG
    std::vector<unsigned int> vertexIndexMap, faceIndexMap;
    assert (mesh.numFaces () == 0 || mesh.pruneAndCheckConsistency (&vertexIndexMap, nullptr));

    for (Cube& c : this->cubes)
    {
      if (vertexIndexMap.empty () == false)
      {
        for (unsigned char i = 0; i < c.numVertexIndicesInMesh; i++)
        {
          c.vertexIndicesInMesh[i] = vertexIndexMap[c.vertexIndicesInMesh[i]];
          assert (c.vertexIndicesInMesh[i] != Util::invalidIndex ());
        }
      }
    }
#endif
    mesh.bufferData ();
  }
};

DELEGATE2_BIG4_COPY (IsosurfaceExtractionGrid, const PrimAABox&, float)
GETTER_CONST (float, IsosurfaceExtractionGrid, resolution)
GETTER_CONST (const glm::uvec3&, IsosurfaceExtractionGrid, numSamples)
GETTER_CONST (const glm::uvec3&, IsosurfaceExtractionGrid, numCubes)
GETTER (std::vector<float>&, IsosurfaceExtractionGrid, samples)
DELEGATE3_CONST (glm::vec3, IsosurfaceExtractionGrid, samplePos, unsigned int, unsigned int,
                 unsigned int)
DELEGATE1_CONST (glm::vec3, IsosurfaceExtractionGrid, samplePos, unsigned int)
DELEGATE3_CONST (unsigned int, IsosurfaceExtractionGrid, sampleIndex, unsigned int, unsigned int,
                 unsigned int)
DELEGATE2_CONST (unsigned int, IsosurfaceExtractionGrid, sampleIndex, unsigned int, unsigned char)
DELEGATE3_CONST (unsigned int, IsosurfaceExtractionGrid, cubeIndex, unsigned int, unsigned int,
                 unsigned int)
DELEGATE1 (void, IsosurfaceExtractionGrid, makeMesh, DynamicMesh&)
