/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
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

  bool isIntersecting (float s1, float s2)
  {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }
}

int IsosurfaceExtractionGrid::vertexIndicesByEdge[12][2] = {
  {0, 1}, {0, 2}, {0, 4}, {2, 3}, {1, 3}, {1, 5}, {4, 5}, {4, 6}, {2, 6}, {6, 7}, {5, 7}, {3, 7}};

IsosurfaceExtractionGrid::Cube::Cube ()
  : configuration (Util::invalidIndex ())
  , vertex (invalidVec3)
  , nonManifold (false)
{
}

bool IsosurfaceExtractionGrid::Cube::nonManifoldConfig () const
{
  assert (this->configuration < 256);
  return ::nonManifoldConfig[this->configuration];
}

bool IsosurfaceExtractionGrid::Cube::collapseNonManifoldConfig () const
{
  return this->nonManifoldConfig () && this->nonManifold == false;
}

unsigned int IsosurfaceExtractionGrid::Cube::vertexIndex (unsigned int edge) const
{
  assert (edge < 12);
  assert (this->configuration < 256);
  assert (vertexIndicesByConfiguration[this->configuration][edge] >= 0);
  assert (this->nonManifold == false || this->nonManifoldConfig ());

  const unsigned int i = this->collapseNonManifoldConfig ()
                           ? 0
                           : (unsigned int) vertexIndicesByConfiguration[this->configuration][edge];
  assert (i < this->vertexIndicesInMesh.size ());

  return this->vertexIndicesInMesh.at (i);
}

unsigned int IsosurfaceExtractionGrid::Cube::getAmbiguousFaceOfNonManifoldConfig () const
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

IsosurfaceExtractionGrid::IsosurfaceExtractionGrid (const PrimAABox& bounds, float r)
  : _resolution (r)
{
  const glm::vec3 min = bounds.minimum () - glm::vec3 (Util::epsilon () + r);
  const glm::vec3 max = bounds.maximum () + glm::vec3 (Util::epsilon () + r);

  this->_sampleOrigin = min;
  this->_numSamples = glm::vec3 (1.0f) + glm::ceil ((max - min) / glm::vec3 (r));
  this->_numCubes = this->_numSamples - glm::uvec3 (1);

  const unsigned int totalNumSamples =
    this->_numSamples.x * this->_numSamples.y * this->_numSamples.z;
  const unsigned int totalNumCubes = this->_numCubes.x * this->_numCubes.y * this->_numCubes.z;

  this->_samples.resize (totalNumSamples, Util::maxFloat ());
  this->_cubes.resize (totalNumCubes);
}

glm::vec3 IsosurfaceExtractionGrid::samplePos (unsigned int x, unsigned int y, unsigned int z) const
{
  assert (x < (unsigned int) this->_numSamples.x);
  assert (y < (unsigned int) this->_numSamples.y);
  assert (z < (unsigned int) this->_numSamples.z);

  return this->_sampleOrigin +
         (glm::vec3 (this->_resolution) * glm::vec3 (float(x), float(y), float(z)));
}

glm::vec3 IsosurfaceExtractionGrid::samplePos (unsigned int i) const
{
  const std::div_t divZ = std::div (int(i), int(this->_numSamples.x * this->_numSamples.y));
  const std::div_t divY = std::div (divZ.rem, int(this->_numSamples.x));

  const unsigned int x = (unsigned int) (divY.rem);
  const unsigned int y = (unsigned int) (divY.quot);
  const unsigned int z = (unsigned int) (divZ.quot);

  assert (this->sampleIndex (x, y, z) == i);

  return this->samplePos (x, y, z);
}

unsigned int IsosurfaceExtractionGrid::sampleIndex (unsigned int x, unsigned int y,
                                                    unsigned int z) const
{
  return (z * this->_numSamples.x * this->_numSamples.y) + (y * this->_numSamples.x) + x;
}

unsigned int IsosurfaceExtractionGrid::sampleIndex (unsigned int cubeIndex,
                                                    unsigned int vertex) const
{
  assert (vertex < 8);

  const std::div_t   divZ = std::div (int(cubeIndex), int(this->_numCubes.x * this->_numCubes.y));
  const std::div_t   divY = std::div (divZ.rem, int(this->_numCubes.x));
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

unsigned int IsosurfaceExtractionGrid::cubeIndex (unsigned int x, unsigned int y,
                                                  unsigned int z) const
{
  return (z * this->_numCubes.x * this->_numCubes.y) + (y * this->_numCubes.x) + x;
}

void IsosurfaceExtractionGrid::setSample (unsigned int index, float sample)
{
  assert (index < this->_samples.size ());
  this->_samples[index] = sample;
}

void IsosurfaceExtractionGrid::setCubeVertex (unsigned int cubeIndex)
{
  glm::vec3    vertex = glm::vec3 (0.0f);
  unsigned int numCrossedEdges = 0;
  Cube&        cube = this->_cubes.at (cubeIndex);

  const unsigned int indices[] = {
    this->sampleIndex (cubeIndex, 0), this->sampleIndex (cubeIndex, 1),
    this->sampleIndex (cubeIndex, 2), this->sampleIndex (cubeIndex, 3),
    this->sampleIndex (cubeIndex, 4), this->sampleIndex (cubeIndex, 5),
    this->sampleIndex (cubeIndex, 6), this->sampleIndex (cubeIndex, 7)};

  const float samples[] = {this->_samples.at (indices[0]), this->_samples.at (indices[1]),
                           this->_samples.at (indices[2]), this->_samples.at (indices[3]),
                           this->_samples.at (indices[4]), this->_samples.at (indices[5]),
                           this->_samples.at (indices[6]), this->_samples.at (indices[7])};

  const glm::vec3 positions[] = {this->samplePos (indices[0]), this->samplePos (indices[1]),
                                 this->samplePos (indices[2]), this->samplePos (indices[3]),
                                 this->samplePos (indices[4]), this->samplePos (indices[5]),
                                 this->samplePos (indices[6]), this->samplePos (indices[7])};

  assert (cube.configuration == Util::invalidIndex ());

  cube.configuration = 0;
  for (unsigned int edge = 0; edge < 12; edge++)
  {
    const unsigned int vertex1 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][0];
    const unsigned int vertex2 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][1];

    if (samples[vertex1] < 0.0f)
    {
      cube.configuration |= (1 << vertex1);
    }

    if (samples[vertex2] < 0.0f)
    {
      cube.configuration |= (1 << vertex2);
    }

    if (isIntersecting (samples[vertex1], samples[vertex2]))
    {
      const float     factor = samples[vertex1] / (samples[vertex1] - samples[vertex2]);
      const glm::vec3 delta = positions[vertex2] - positions[vertex1];

      vertex += positions[vertex1] + (delta * factor);
      numCrossedEdges++;
    }
  }
  assert (cube.configuration < 256);

#ifndef NDEBUG
  for (unsigned int edge = 0; edge < 12; edge++)
  {
    const unsigned int vertex1 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][0];
    const unsigned int vertex2 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][1];

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

void IsosurfaceExtractionGrid::setCubeVertices ()
{
  for (unsigned int z = 0; z < this->_numCubes.z; z++)
  {
    for (unsigned int y = 0; y < this->_numCubes.y; y++)
    {
      for (unsigned int x = 0; x < this->_numCubes.x; x++)
      {
        this->setCubeVertex (this->cubeIndex (x, y, z));
      }
    }
  }

#ifndef NDEBUG
  for (unsigned int z = 0; z < this->_numCubes.z; z++)
  {
    for (unsigned int y = 0; y < this->_numCubes.y; y++)
    {
      for (unsigned int x = 0; x < this->_numCubes.x; x++)
      {
        unsigned int config = this->_cubes.at (this->cubeIndex (x, y, z)).configuration;

        if (x > 0)
        {
          unsigned int left = this->_cubes.at (this->cubeIndex (x - 1, y, z)).configuration;

          assert (((config & (1 << 0)) == 0) == ((left & (1 << 1)) == 0));
          assert (((config & (1 << 2)) == 0) == ((left & (1 << 3)) == 0));
          assert (((config & (1 << 4)) == 0) == ((left & (1 << 5)) == 0));
          assert (((config & (1 << 6)) == 0) == ((left & (1 << 7)) == 0));
        }
        if (y > 0)
        {
          unsigned int below = this->_cubes.at (this->cubeIndex (x, y - 1, z)).configuration;

          assert (((config & (1 << 0)) == 0) == ((below & (1 << 2)) == 0));
          assert (((config & (1 << 1)) == 0) == ((below & (1 << 3)) == 0));
          assert (((config & (1 << 4)) == 0) == ((below & (1 << 6)) == 0));
          assert (((config & (1 << 5)) == 0) == ((below & (1 << 7)) == 0));
        }
        if (z > 0)
        {
          unsigned int behind = this->_cubes.at (this->cubeIndex (x, y, z - 1)).configuration;

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

void IsosurfaceExtractionGrid::resolveNonManifolds ()
{
  auto check = [this](const Cube& cube, unsigned int x, unsigned int y, unsigned int z,
                      unsigned int ambiguousFace, int dim) -> bool {
    assert (cube.nonManifoldConfig ());
    assert (dim == -3 || dim == -2 || dim == -1 || dim == 1 || dim == 2 || dim == 3);
    unused (cube);

    Cube& other = this->_cubes.at (this->cubeIndex (dim == -1 ? x - 1 : (dim == 1 ? x + 1 : x),
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

  for (unsigned int z = 0; z < this->_numCubes.z; z++)
  {
    for (unsigned int y = 0; y < this->_numCubes.y; y++)
    {
      for (unsigned int x = 0; x < this->_numCubes.x; x++)
      {
        Cube& cube = this->_cubes.at (this->cubeIndex (x, y, z));

        if (cube.nonManifoldConfig ())
        {
          const unsigned int ambiguousFace = cube.getAmbiguousFaceOfNonManifoldConfig ();

          const bool nx = x > 0 && check (cube, x, y, z, ambiguousFace, -1);
          const bool px = x < this->_numCubes.x - 1 && check (cube, x, y, z, ambiguousFace, 1);
          const bool ny = y > 0 && check (cube, x, y, z, ambiguousFace, -2);
          const bool py = y < this->_numCubes.y - 1 && check (cube, x, y, z, ambiguousFace, 2);
          const bool nz = z > 0 && check (cube, x, y, z, ambiguousFace, -3);
          const bool pz = z < this->_numCubes.z - 1 && check (cube, x, y, z, ambiguousFace, 3);

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

void IsosurfaceExtractionGrid::addCubeVerticesToMesh (Mesh& mesh)
{
  for (Cube& cube : this->_cubes)
  {
    for (unsigned int i = 0; i < cube.vertexIndicesInMesh.size (); i++)
    {
      cube.vertexIndicesInMesh.at (i) = mesh.addVertex (cube.vertex);

      if (cube.collapseNonManifoldConfig ())
      {
        break;
      }
    }
  }
}
