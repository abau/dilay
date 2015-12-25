/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
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
namespace {
  static glm::vec3 invalidVec3 = glm::vec3 (std::numeric_limits <float>::lowest ());

  static int edgeVertexIndices[256][12] = {
      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
    , {0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1}
    , {0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1}
    , {-1,0,0,-1,0,0,-1,-1,-1,-1,-1,-1}
    , {-1,0,-1,0,-1,-1,-1,-1,0,-1,-1,-1}
    , {0,-1,0,0,-1,-1,-1,-1,0,-1,-1,-1}
    , {0,1,-1,1,0,0,-1,-1,1,-1,-1,-1}
    , {-1,-1,0,0,0,0,-1,-1,0,-1,-1,-1}
    , {-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,0}
    , {0,0,0,1,1,-1,-1,-1,-1,-1,-1,1}
    , {0,-1,-1,0,-1,0,-1,-1,-1,-1,-1,0}
    , {-1,0,0,0,-1,0,-1,-1,-1,-1,-1,0}
    , {-1,0,-1,-1,0,-1,-1,-1,0,-1,-1,0}
    , {0,-1,0,-1,0,-1,-1,-1,0,-1,-1,0}
    , {0,0,-1,-1,-1,0,-1,-1,0,-1,-1,0}
    , {-1,-1,0,-1,-1,0,-1,-1,0,-1,-1,0}
    , {-1,-1,0,-1,-1,-1,0,0,-1,-1,-1,-1}
    , {0,0,-1,-1,-1,-1,0,0,-1,-1,-1,-1}
    , {0,-1,1,-1,0,0,1,1,-1,-1,-1,-1}
    , {-1,0,-1,-1,0,0,0,0,-1,-1,-1,-1}
    , {-1,0,1,0,-1,-1,1,1,0,-1,-1,-1}
    , {0,-1,-1,0,-1,-1,0,0,0,-1,-1,-1}
    , {0,1,2,1,0,0,2,2,1,-1,-1,-1}
    , {-1,-1,-1,0,0,0,0,0,0,-1,-1,-1}
    , {-1,-1,1,0,0,-1,1,1,-1,-1,-1,0}
    , {0,0,-1,1,1,-1,0,0,-1,-1,-1,1}
    , {0,-1,1,0,-1,0,1,1,-1,-1,-1,0}
    , {-1,0,-1,0,-1,0,0,0,-1,-1,-1,0}
    , {-1,0,1,-1,0,-1,1,1,0,-1,-1,0}
    , {0,-1,-1,-1,0,-1,0,0,0,-1,-1,0}
    , {0,0,1,-1,-1,0,1,1,0,-1,-1,0}
    , {-1,-1,-1,-1,-1,0,0,0,0,-1,-1,0}
    , {-1,-1,-1,-1,-1,0,0,-1,-1,-1,0,-1}
    , {0,0,0,-1,-1,1,1,-1,-1,-1,1,-1}
    , {0,-1,-1,-1,0,-1,0,-1,-1,-1,0,-1}
    , {-1,0,0,-1,0,-1,0,-1,-1,-1,0,-1}
    , {-1,0,-1,0,-1,1,1,-1,0,-1,1,-1}
    , {0,-1,0,0,-1,1,1,-1,0,-1,1,-1}
    , {0,1,-1,1,0,-1,0,-1,1,-1,0,-1}
    , {-1,-1,0,0,0,-1,0,-1,0,-1,0,-1}
    , {-1,-1,-1,0,0,1,1,-1,-1,-1,1,0}
    , {0,0,0,1,1,2,2,-1,-1,-1,2,1}
    , {0,-1,-1,0,-1,-1,0,-1,-1,-1,0,0}
    , {-1,0,0,0,-1,-1,0,-1,-1,-1,0,0}
    , {-1,0,-1,-1,0,1,1,-1,0,-1,1,0}
    , {0,-1,0,-1,0,1,1,-1,0,-1,1,0}
    , {0,0,-1,-1,-1,-1,0,-1,0,-1,0,0}
    , {-1,-1,0,-1,-1,-1,0,-1,0,-1,0,0}
    , {-1,-1,0,-1,-1,0,-1,0,-1,-1,0,-1}
    , {0,0,-1,-1,-1,0,-1,0,-1,-1,0,-1}
    , {0,-1,0,-1,0,-1,-1,0,-1,-1,0,-1}
    , {-1,0,-1,-1,0,-1,-1,0,-1,-1,0,-1}
    , {-1,0,1,0,-1,1,-1,1,0,-1,1,-1}
    , {0,-1,-1,0,-1,0,-1,0,0,-1,0,-1}
    , {0,1,0,1,0,-1,-1,0,1,-1,0,-1}
    , {-1,-1,-1,0,0,-1,-1,0,0,-1,0,-1}
    , {-1,-1,1,0,0,1,-1,1,-1,-1,1,0}
    , {0,0,-1,1,1,0,-1,0,-1,-1,0,1}
    , {0,-1,0,0,-1,-1,-1,0,-1,-1,0,0}
    , {-1,0,-1,0,-1,-1,-1,0,-1,-1,0,0}
    , {-1,0,1,-1,0,1,-1,1,0,-1,1,0}
    , {0,-1,-1,-1,0,0,-1,1,1,-1,1,1}
    , {0,0,0,-1,-1,-1,-1,1,1,-1,1,1}
    , {-1,-1,-1,-1,-1,-1,-1,0,0,-1,0,0}
    , {-1,-1,-1,-1,-1,-1,-1,0,0,0,-1,-1}
    , {0,0,0,-1,-1,-1,-1,1,1,1,-1,-1}
    , {0,-1,-1,-1,0,0,-1,1,1,1,-1,-1}
    , {-1,0,0,-1,0,0,-1,1,1,1,-1,-1}
    , {-1,0,-1,0,-1,-1,-1,0,-1,0,-1,-1}
    , {0,-1,0,0,-1,-1,-1,0,-1,0,-1,-1}
    , {0,1,-1,1,0,0,-1,1,-1,1,-1,-1}
    , {-1,-1,0,0,0,0,-1,0,-1,0,-1,-1}
    , {-1,-1,-1,0,0,-1,-1,1,1,1,-1,0}
    , {0,0,0,1,1,-1,-1,2,2,2,-1,1}
    , {0,-1,-1,0,-1,0,-1,1,1,1,-1,0}
    , {-1,0,0,0,-1,0,-1,1,1,1,-1,0}
    , {-1,0,-1,-1,0,-1,-1,0,-1,0,-1,0}
    , {0,-1,0,-1,0,-1,-1,0,-1,0,-1,0}
    , {0,0,-1,-1,-1,0,-1,0,-1,0,-1,0}
    , {-1,-1,0,-1,-1,0,-1,0,-1,0,-1,0}
    , {-1,-1,0,-1,-1,-1,0,-1,0,0,-1,-1}
    , {0,0,-1,-1,-1,-1,0,-1,0,0,-1,-1}
    , {0,-1,1,-1,0,0,1,-1,1,1,-1,-1}
    , {-1,0,-1,-1,0,0,0,-1,0,0,-1,-1}
    , {-1,0,0,0,-1,-1,0,-1,-1,0,-1,-1}
    , {0,-1,-1,0,-1,-1,0,-1,-1,0,-1,-1}
    , {0,1,1,1,0,0,1,-1,-1,1,-1,-1}
    , {-1,-1,-1,0,0,0,0,-1,-1,0,-1,-1}
    , {-1,-1,1,0,0,-1,1,-1,1,1,-1,0}
    , {0,0,-1,1,1,-1,0,-1,0,0,-1,1}
    , {0,-1,1,0,-1,0,1,-1,1,1,-1,0}
    , {-1,0,-1,0,-1,1,1,-1,0,1,-1,1}
    , {-1,0,0,-1,0,-1,0,-1,-1,0,-1,0}
    , {0,-1,-1,-1,0,-1,0,-1,-1,0,-1,0}
    , {0,0,0,-1,-1,1,1,-1,-1,1,-1,1}
    , {-1,-1,-1,-1,-1,0,0,-1,-1,0,-1,0}
    , {-1,-1,-1,-1,-1,0,0,1,1,1,0,-1}
    , {0,0,0,-1,-1,1,1,2,2,2,1,-1}
    , {0,-1,-1,-1,0,-1,0,1,1,1,0,-1}
    , {-1,0,0,-1,0,-1,0,1,1,1,0,-1}
    , {-1,0,-1,0,-1,1,1,0,-1,0,1,-1}
    , {0,-1,0,0,-1,1,1,0,-1,0,1,-1}
    , {0,1,-1,1,0,-1,0,1,-1,1,0,-1}
    , {-1,-1,1,0,0,-1,1,1,-1,0,0,-1}
    , {-1,-1,-1,0,0,1,1,2,2,2,1,0}
    , {0,0,0,1,1,2,2,3,3,3,2,1}
    , {0,-1,-1,0,-1,-1,0,1,1,1,0,0}
    , {-1,0,0,0,-1,-1,0,1,1,1,0,0}
    , {-1,0,-1,-1,0,1,1,0,-1,0,1,0}
    , {0,-1,0,-1,0,1,1,0,-1,0,1,0}
    , {0,0,-1,-1,-1,-1,0,0,-1,1,1,1}
    , {-1,-1,0,-1,-1,-1,0,0,-1,1,1,1}
    , {-1,-1,0,-1,-1,0,-1,-1,0,0,0,-1}
    , {0,0,-1,-1,-1,0,-1,-1,0,0,0,-1}
    , {0,-1,0,-1,0,-1,-1,-1,0,0,0,-1}
    , {-1,0,-1,-1,0,-1,-1,-1,0,0,0,-1}
    , {-1,0,0,0,-1,0,-1,-1,-1,0,0,-1}
    , {0,-1,-1,0,-1,0,-1,-1,-1,0,0,-1}
    , {0,0,0,1,1,-1,-1,-1,-1,1,1,-1}
    , {-1,-1,-1,0,0,-1,-1,-1,-1,0,0,-1}
    , {-1,-1,1,0,0,1,-1,-1,1,1,1,0}
    , {0,0,-1,1,1,0,-1,-1,0,0,0,1}
    , {0,-1,0,0,-1,-1,-1,-1,0,1,1,1}
    , {-1,0,-1,0,-1,-1,-1,-1,0,1,1,1}
    , {-1,0,0,-1,0,0,-1,-1,-1,1,1,1}
    , {0,-1,-1,-1,0,0,-1,-1,-1,1,1,1}
    , {0,0,0,-1,-1,-1,-1,-1,-1,1,1,1}
    , {-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0}
    , {-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0}
    , {0,0,0,-1,-1,-1,-1,-1,-1,1,1,1}
    , {0,-1,-1,-1,0,0,-1,-1,-1,1,1,1}
    , {-1,0,0,-1,0,0,-1,-1,-1,1,1,1}
    , {-1,0,-1,0,-1,-1,-1,-1,0,1,1,1}
    , {0,-1,0,0,-1,-1,-1,-1,0,1,1,1}
    , {0,1,-1,1,0,0,-1,-1,1,2,2,2}
    , {-1,-1,0,0,0,0,-1,-1,0,1,1,1}
    , {-1,-1,-1,0,0,-1,-1,-1,-1,0,0,-1}
    , {0,0,0,1,1,-1,-1,-1,-1,1,1,-1}
    , {0,-1,-1,0,-1,0,-1,-1,-1,0,0,-1}
    , {-1,0,0,0,-1,0,-1,-1,-1,0,0,-1}
    , {-1,0,-1,-1,0,-1,-1,-1,0,0,0,-1}
    , {0,-1,0,-1,0,-1,-1,-1,0,0,0,-1}
    , {0,0,-1,-1,-1,0,-1,-1,0,0,0,-1}
    , {-1,-1,0,-1,-1,0,-1,-1,0,0,0,-1}
    , {-1,-1,0,-1,-1,-1,0,0,-1,1,1,1}
    , {0,0,-1,-1,-1,-1,0,0,-1,1,1,1}
    , {0,-1,1,-1,0,0,1,1,-1,2,2,2}
    , {-1,0,-1,-1,0,0,0,0,-1,1,1,1}
    , {-1,0,1,0,-1,-1,1,1,0,2,2,2}
    , {0,-1,-1,0,-1,-1,0,0,0,1,1,1}
    , {0,1,2,1,0,0,2,2,1,3,3,3}
    , {-1,-1,-1,0,0,0,0,0,0,1,1,1}
    , {-1,-1,1,0,0,-1,1,1,-1,0,0,-1}
    , {0,0,-1,1,1,-1,0,0,-1,1,1,-1}
    , {0,-1,1,0,-1,0,1,1,-1,0,0,-1}
    , {-1,0,-1,0,-1,1,1,0,-1,0,1,-1}
    , {-1,0,1,-1,0,-1,1,1,0,0,0,-1}
    , {0,-1,-1,-1,0,-1,0,1,1,1,0,-1}
    , {0,0,1,-1,-1,0,1,1,0,0,0,-1}
    , {-1,-1,-1,-1,-1,0,0,1,1,1,0,-1}
    , {-1,-1,-1,-1,-1,0,0,-1,-1,0,-1,0}
    , {0,0,0,-1,-1,1,1,-1,-1,1,-1,1}
    , {0,-1,-1,-1,0,-1,0,-1,-1,0,-1,0}
    , {-1,0,0,-1,0,-1,0,-1,-1,0,-1,0}
    , {-1,0,-1,0,-1,1,1,-1,0,1,-1,1}
    , {0,-1,0,0,-1,1,1,-1,0,1,-1,1}
    , {0,1,-1,1,0,-1,0,-1,1,0,-1,0}
    , {-1,-1,1,0,0,-1,1,-1,1,1,-1,0}
    , {-1,-1,-1,0,0,0,0,-1,-1,0,-1,-1}
    , {0,0,0,1,1,1,1,-1,-1,1,-1,-1}
    , {0,-1,-1,0,-1,-1,0,-1,-1,0,-1,-1}
    , {-1,0,0,0,-1,-1,0,-1,-1,0,-1,-1}
    , {-1,0,-1,-1,0,0,0,-1,0,0,-1,-1}
    , {0,-1,1,-1,0,0,1,-1,1,1,-1,-1}
    , {0,0,-1,-1,-1,-1,0,-1,0,0,-1,-1}
    , {-1,-1,0,-1,-1,-1,0,-1,0,0,-1,-1}
    , {-1,-1,0,-1,-1,0,-1,0,-1,0,-1,0}
    , {0,0,-1,-1,-1,0,-1,0,-1,0,-1,0}
    , {0,-1,0,-1,0,-1,-1,0,-1,0,-1,0}
    , {-1,0,-1,-1,0,-1,-1,0,-1,0,-1,0}
    , {-1,0,1,0,-1,1,-1,1,0,1,-1,1}
    , {0,-1,-1,0,-1,0,-1,1,1,1,-1,0}
    , {0,1,0,1,0,-1,-1,0,1,0,-1,0}
    , {-1,-1,-1,0,0,-1,-1,1,1,1,-1,0}
    , {-1,-1,0,0,0,0,-1,0,-1,0,-1,-1}
    , {0,1,-1,1,0,0,-1,1,-1,1,-1,-1}
    , {0,-1,0,0,-1,-1,-1,0,-1,0,-1,-1}
    , {-1,0,-1,0,-1,-1,-1,0,-1,0,-1,-1}
    , {-1,0,0,-1,0,0,-1,1,1,1,-1,-1}
    , {0,-1,-1,-1,0,0,-1,1,1,1,-1,-1}
    , {0,0,0,-1,-1,-1,-1,1,1,1,-1,-1}
    , {-1,-1,-1,-1,-1,-1,-1,0,0,0,-1,-1}
    , {-1,-1,-1,-1,-1,-1,-1,0,0,-1,0,0}
    , {0,0,0,-1,-1,-1,-1,1,1,-1,1,1}
    , {0,-1,-1,-1,0,0,-1,1,1,-1,1,1}
    , {-1,0,0,-1,0,0,-1,1,1,-1,1,1}
    , {-1,0,-1,0,-1,-1,-1,0,-1,-1,0,0}
    , {0,-1,0,0,-1,-1,-1,0,-1,-1,0,0}
    , {0,1,-1,1,0,0,-1,1,-1,-1,1,1}
    , {-1,-1,1,0,0,1,-1,1,-1,-1,1,0}
    , {-1,-1,-1,0,0,-1,-1,0,0,-1,0,-1}
    , {0,0,0,1,1,-1,-1,1,1,-1,1,-1}
    , {0,-1,-1,0,-1,0,-1,0,0,-1,0,-1}
    , {-1,0,1,0,-1,1,-1,1,0,-1,1,-1}
    , {-1,0,-1,-1,0,-1,-1,0,-1,-1,0,-1}
    , {0,-1,0,-1,0,-1,-1,0,-1,-1,0,-1}
    , {0,0,-1,-1,-1,0,-1,0,-1,-1,0,-1}
    , {-1,-1,0,-1,-1,0,-1,0,-1,-1,0,-1}
    , {-1,-1,0,-1,-1,-1,0,-1,0,-1,0,0}
    , {0,0,-1,-1,-1,-1,0,-1,0,-1,0,0}
    , {0,-1,1,-1,0,0,1,-1,1,-1,1,1}
    , {-1,0,-1,-1,0,1,1,-1,0,-1,1,0}
    , {-1,0,0,0,-1,-1,0,-1,-1,-1,0,0}
    , {0,-1,-1,0,-1,-1,0,-1,-1,-1,0,0}
    , {0,1,1,1,0,0,1,-1,-1,-1,1,1}
    , {-1,-1,-1,0,0,1,1,-1,-1,-1,1,0}
    , {-1,-1,0,0,0,-1,0,-1,0,-1,0,-1}
    , {0,1,-1,1,0,-1,0,-1,1,-1,0,-1}
    , {0,-1,0,0,-1,1,1,-1,0,-1,1,-1}
    , {-1,0,-1,0,-1,1,1,-1,0,-1,1,-1}
    , {-1,0,0,-1,0,-1,0,-1,-1,-1,0,-1}
    , {0,-1,-1,-1,0,-1,0,-1,-1,-1,0,-1}
    , {0,0,0,-1,-1,1,1,-1,-1,-1,1,-1}
    , {-1,-1,-1,-1,-1,0,0,-1,-1,-1,0,-1}
    , {-1,-1,-1,-1,-1,0,0,0,0,-1,-1,0}
    , {0,0,0,-1,-1,1,1,1,1,-1,-1,1}
    , {0,-1,-1,-1,0,-1,0,0,0,-1,-1,0}
    , {-1,0,1,-1,0,-1,1,1,0,-1,-1,0}
    , {-1,0,-1,0,-1,0,0,0,-1,-1,-1,0}
    , {0,-1,1,0,-1,0,1,1,-1,-1,-1,0}
    , {0,0,-1,1,1,-1,0,0,-1,-1,-1,1}
    , {-1,-1,1,0,0,-1,1,1,-1,-1,-1,0}
    , {-1,-1,-1,0,0,0,0,0,0,-1,-1,-1}
    , {0,0,0,1,1,1,1,1,1,-1,-1,-1}
    , {0,-1,-1,0,-1,-1,0,0,0,-1,-1,-1}
    , {-1,0,1,0,-1,-1,1,1,0,-1,-1,-1}
    , {-1,0,-1,-1,0,0,0,0,-1,-1,-1,-1}
    , {0,-1,1,-1,0,0,1,1,-1,-1,-1,-1}
    , {0,0,-1,-1,-1,-1,0,0,-1,-1,-1,-1}
    , {-1,-1,0,-1,-1,-1,0,0,-1,-1,-1,-1}
    , {-1,-1,0,-1,-1,0,-1,-1,0,-1,-1,0}
    , {0,0,-1,-1,-1,0,-1,-1,0,-1,-1,0}
    , {0,-1,0,-1,0,-1,-1,-1,0,-1,-1,0}
    , {-1,0,-1,-1,0,-1,-1,-1,0,-1,-1,0}
    , {-1,0,0,0,-1,0,-1,-1,-1,-1,-1,0}
    , {0,-1,-1,0,-1,0,-1,-1,-1,-1,-1,0}
    , {0,0,0,1,1,-1,-1,-1,-1,-1,-1,1}
    , {-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,0}
    , {-1,-1,0,0,0,0,-1,-1,0,-1,-1,-1}
    , {0,1,-1,1,0,0,-1,-1,1,-1,-1,-1}
    , {0,-1,0,0,-1,-1,-1,-1,0,-1,-1,-1}
    , {-1,0,-1,0,-1,-1,-1,-1,0,-1,-1,-1}
    , {-1,0,0,-1,0,0,-1,-1,-1,-1,-1,-1}
    , {0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1}
    , {0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1}
    , {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
  };

  void vertexIndices (unsigned int edge, unsigned int& vertex1, unsigned int& vertex2) {
    switch (edge) {
      case 0 : vertex1 = 0; vertex2 = 1; return;
      case 1 : vertex1 = 0; vertex2 = 2; return;
      case 2 : vertex1 = 0; vertex2 = 4; return;
      case 3 : vertex1 = 2; vertex2 = 3; return;
      case 4 : vertex1 = 1; vertex2 = 3; return;
      case 5 : vertex1 = 1; vertex2 = 5; return;
      case 6 : vertex1 = 4; vertex2 = 5; return;
      case 7 : vertex1 = 4; vertex2 = 6; return;
      case 8 : vertex1 = 2; vertex2 = 6; return;
      case 9 : vertex1 = 6; vertex2 = 7; return;
      case 10: vertex1 = 5; vertex2 = 7; return;
      case 11: vertex1 = 3; vertex2 = 7; return;
      default: DILAY_IMPOSSIBLE;
    }
  }

  void edgeIndices ( unsigned int face, unsigned& edge1, unsigned int& edge2
                   , unsigned& edge3, unsigned int& edge4 )
  {
    switch (face) {
      case 0: edge1 = 0; edge2 = 2; edge3 =  5; edge4 =  6; return;
      case 1: edge1 = 3; edge2 = 8; edge3 =  9; edge4 = 11; return;
      case 2: edge1 = 1; edge2 = 2; edge3 =  7; edge4 =  8; return;
      case 3: edge1 = 4; edge2 = 5; edge3 = 10; edge4 = 11; return;
      case 4: edge1 = 0; edge2 = 1; edge3 =  3; edge4 =  4; return;
      case 5: edge1 = 6; edge2 = 7; edge3 =  9; edge4 = 10; return;
      default: DILAY_IMPOSSIBLE;
    }
  }

  struct Cube {
    unsigned int               configuration;
    glm::vec3                  vertex;
    std::vector <unsigned int> vertexInstanceIndices;
    bool                       collapseWhenAmbiguous;

    Cube ()
      : configuration         (Util::invalidIndex ())  
      , vertex                (invalidVec3)
      , collapseWhenAmbiguous (false)
    {}

    void initializeVertexInstanceIndices () {
      assert (this->configuration <= 255);

      int n = -1;
      for (unsigned int i = 0; i < 12; i++) {
        n = glm::max (n, edgeVertexIndices[this->configuration][i]);
      }
      this->vertexInstanceIndices.resize (n+1, Util::invalidIndex ());
    }

    unsigned int vertexInstanceIndex (unsigned int edge) const {
      assert (edge <= 11);
      assert (this->configuration <= 255);
      assert (edgeVertexIndices[this->configuration][edge] >= 0);
      assert (this->collapseWhenAmbiguous == false || this->isAmbiguous ());

      const unsigned int i = this->collapseWhenAmbiguous
                           ? 0
                           : (unsigned int) edgeVertexIndices[this->configuration][edge];

      assert (i < this->vertexInstanceIndices.size ());

      return this->vertexInstanceIndices.at (i);
    }

    bool hasAmbiguousFace ( unsigned int edge1, unsigned int edge2, unsigned int edge3
                          , unsigned int edge4 ) const
    {
      assert (this->configuration <= 255);
      assert (edge1 <= 11);
      assert (edge2 <= 11);
      assert (edge3 <= 11);
      assert (edge4 <= 11);

      const int i1 = edgeVertexIndices[this->configuration][edge1];
      const int i2 = edgeVertexIndices[this->configuration][edge2];
      const int i3 = edgeVertexIndices[this->configuration][edge3];
      const int i4 = edgeVertexIndices[this->configuration][edge4];

      if (i1 == -1 || i2 == -1 || i3 == -1 || i4 == -1) {
        return false;
      }
      else {
        assert (i1 == i2 || i1 == i3 || i1 == i4);
        assert (i2 == i1 || i2 == i3 || i2 == i4);
        assert (i3 == i1 || i3 == i2 || i3 == i4);
        assert (i4 == i1 || i4 == i2 || i4 == i3);

        assert ((i1 != i2) || (i3 == i4 && i1 != i3));
        assert ((i1 != i3) || (i2 == i4 && i1 != i2));
        assert ((i1 != i4) || (i2 == i3 && i1 != i2));
        assert ((i2 != i3) || (i1 == i4 && i1 != i2));
        assert ((i2 != i4) || (i1 == i3 && i1 != i2));
        assert ((i3 != i4) || (i1 == i2 && i1 != i3));

        return true;
      }
    }

    bool hasAmbiguousFaces (unsigned int* face = nullptr) const {
      for (unsigned int i = 0; i < 6; i++) {
        unsigned int edge1, edge2, edge3, edge4;
        edgeIndices (i, edge1, edge2, edge3, edge4);

        if (this->hasAmbiguousFace (edge1, edge2, edge3, edge4)) {
          if (face) {
            *face = i;
          }
          return true;
        }
      }
      return false;
    }

    bool isAmbiguous () const {
      assert (this->configuration <= 255);

      switch (Util::countOnes (this->configuration)) {
        case  5: return this->hasAmbiguousFaces ();
        case  6: return this->hasAmbiguousFaces ();
        default: return false;
      }
    }
  };

  struct Parameters {
    float               resolution;
    glm::vec3           sampleOrigin;
    glm::uvec3          numSamples;
    std::vector <float> samples;
    glm::uvec3          numCubes;
    std::vector <Cube>  grid;

    Parameters ()
      : resolution   (0.0f)
      , sampleOrigin (glm::vec3 (0.0f))
      , numSamples   (glm::uvec3 (0))
    {}

    glm::vec3 samplePos (unsigned int x, unsigned int y, unsigned int z) const {
      assert (x < (unsigned int) this->numSamples.x);
      assert (y < (unsigned int) this->numSamples.y);
      assert (z < (unsigned int) this->numSamples.z);

      return sampleOrigin + ( glm::vec3 (this->resolution) 
                            * glm::vec3 (float (x), float (y), float (z)) );
    }

    glm::vec3 samplePos (unsigned int i) const {
      const std::div_t divZ = std::div (int (i), int (this->numSamples.x * this->numSamples.y));
      const std::div_t divY = std::div (divZ.rem, int (this->numSamples.x));

      return this->samplePos ( (unsigned int) (divY.rem)
                             , (unsigned int) (divY.quot)
                             , (unsigned int) (divZ.quot) );
    }

    unsigned int sampleIndex (unsigned int x, unsigned int y, unsigned int z) const {
      return (z * this->numSamples.x * this->numSamples.y) + (y * this->numSamples.x) + x;
    }

    unsigned int sampleIndex (unsigned int cubeIndex, unsigned int vertex) const {
      assert (vertex < 8);

      const std::div_t   divZ = std::div ( int (cubeIndex)
                                         , int (this->numCubes.x * this->numCubes.y));
      const std::div_t   divY = std::div (divZ.rem, int (this->numCubes.x));
      const unsigned int x    = (unsigned int) (divY.rem);
      const unsigned int y    = (unsigned int) (divY.quot);
      const unsigned int z    = (unsigned int) (divZ.quot);

      switch (vertex) {
        case 0 : return this->sampleIndex (x  , y  , z  );
        case 1 : return this->sampleIndex (x+1, y  , z  );
        case 2 : return this->sampleIndex (x  , y+1, z  );
        case 3 : return this->sampleIndex (x+1, y+1, z  );
        case 4 : return this->sampleIndex (x  , y  , z+1);
        case 5 : return this->sampleIndex (x+1, y  , z+1);
        case 6 : return this->sampleIndex (x  , y+1, z+1);
        case 7 : return this->sampleIndex (x+1, y+1, z+1);
        default: DILAY_IMPOSSIBLE
      }
    }

    unsigned int cubeIndex (unsigned int x, unsigned int y, unsigned int z) const {
      return (z * this->numCubes.x * this->numCubes.y) + (y * this->numCubes.x) + x;
    }
  };

  void setupSampling (const SketchMesh& mesh, Parameters& params) {
    glm::vec3 min, max;
    mesh.minMax (min, max);

    min = min - glm::vec3 (Util::epsilon ());
    max = max + glm::vec3 (Util::epsilon ());

    params.sampleOrigin = min;
    params.numSamples   = glm::vec3 (1.0f) + glm::ceil ((max - min) / glm::vec3 (params.resolution));
  }

  float sampleAt (const SketchMesh& mesh, const glm::vec3& pos) {
    float distance = std::numeric_limits <float>::max ();
      
    if (mesh.tree ().hasRoot ()) {
      mesh.tree ().root ().forEachConstNode ([&pos, &distance] (const SketchNode& node) {
        const float d = node.parent ()
                      ? Distance::distance ( PrimConeSphere ( node.data ()
                                                            , node.parent ()->data () )
                                           , pos )
                      : Distance::distance (node.data (), pos);

        distance = glm::min (distance, d);
      });
    }
    for (const SketchPath& p : mesh.paths ()) {
      for (const PrimSphere& s : p.spheres ()) {
        distance = glm::min (distance, Distance::distance (s, pos));
      }
    }
    return distance;
  }

  void sampleThread ( const SketchMesh& mesh, Parameters& params
                    , unsigned int numThreads, unsigned int threadId )
  {
    for (unsigned int z = 0; z < params.numSamples.z; z++) {
      for (unsigned int y = 0; y < params.numSamples.y; y++) {
        for (unsigned int x = 0; x < params.numSamples.x; x++) {
          const unsigned int index = params.sampleIndex (x,y,z);

          if (index % numThreads == threadId) {
            const glm::vec3    pos   = params.samplePos   (x,y,z);

            params.samples.at (index) = sampleAt (mesh, pos);

            assert ((x > 0 && x < params.numSamples.x-1) || params.samples.at (index) > 0.0f);
            assert ((y > 0 && y < params.numSamples.y-1) || params.samples.at (index) > 0.0f);
            assert ((z > 0 && z < params.numSamples.z-1) || params.samples.at (index) > 0.0f);
          }
        }
      }
    }
  }

  void sample (const SketchMesh& mesh, Parameters& params) {
    params.samples.resize (params.numSamples.x * params.numSamples.y * params.numSamples.z, 0.0f);

    const unsigned int        numThreads = std::thread::hardware_concurrency();
    std::vector <std::thread> threads;

    for (unsigned int i = 0; i < numThreads; i++) {
      threads.emplace_back (sampleThread, std::ref (mesh), std::ref (params), numThreads, i);
    }
    for (unsigned int i = 0; i < numThreads; i++) {
      threads.at (i).join ();
    }
  }

  bool isIntersecting (float s1, float s2) {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }

  void setCubeVertex (Parameters& params, unsigned int cubeIndex) {
    glm::vec3    vertex          = glm::vec3 (0.0f);
    unsigned int numCrossedEdges = 0;
    Cube&        cube            = params.grid.at (cubeIndex);

    const unsigned int indices[] = { params.sampleIndex (cubeIndex, 0)
                                   , params.sampleIndex (cubeIndex, 1)
                                   , params.sampleIndex (cubeIndex, 2)
                                   , params.sampleIndex (cubeIndex, 3)
                                   , params.sampleIndex (cubeIndex, 4)
                                   , params.sampleIndex (cubeIndex, 5)
                                   , params.sampleIndex (cubeIndex, 6)
                                   , params.sampleIndex (cubeIndex, 7)
                                   };

    const float samples[] = { params.samples.at (indices [0]), params.samples.at (indices [1])
                            , params.samples.at (indices [2]), params.samples.at (indices [3])
                            , params.samples.at (indices [4]), params.samples.at (indices [5])
                            , params.samples.at (indices [6]), params.samples.at (indices [7])
                            };

    const glm::vec3 positions[] = { params.samplePos (indices [0]), params.samplePos (indices [1])
                                  , params.samplePos (indices [2]), params.samplePos (indices [3])
                                  , params.samplePos (indices [4]), params.samplePos (indices [5])
                                  , params.samplePos (indices [6]), params.samplePos (indices [7])
                                  };

    auto checkEdge = [&numCrossedEdges, &vertex, &samples, &positions]
                     (unsigned short vertex1, unsigned short vertex2)
    {
      if (isIntersecting (samples[vertex1], samples[vertex2])) {
        const float     factor = samples[vertex1] / (samples[vertex1] - samples[vertex2]);
        const glm::vec3 delta  = positions[vertex2] - positions[vertex1];

        vertex += positions[vertex1] + (delta * factor);
        numCrossedEdges++;
      }
    };

    cube.configuration = 0;
    for (unsigned int edge = 0; edge < 12; edge++) {
      unsigned int vertex1, vertex2;
      vertexIndices (edge, vertex1, vertex2);

      cube.configuration |= (int (samples[vertex1] < 0.0f)) << vertex1;
      cube.configuration |= (int (samples[vertex2] < 0.0f)) << vertex2;

      checkEdge (vertex1, vertex2);
    }
    assert (cube.configuration <= 255);

#ifndef NDEBUG
    for (unsigned int edge = 0; edge < 12; edge++) {
      unsigned int vertex1, vertex2;
      vertexIndices (edge, vertex1, vertex2);

      if (isIntersecting (samples[vertex1], samples[vertex2])) {
        assert (edgeVertexIndices [cube.configuration][edge] != -1);
      }
    }
#endif

    if (numCrossedEdges > 0) {
      cube.vertex = vertex / glm::vec3 (float (numCrossedEdges));
      cube.initializeVertexInstanceIndices ();
    }
  }

  void makeGrid (Parameters& params) {
    params.numCubes = params.numSamples - glm::uvec3 (1);
    params.grid.resize (params.numCubes.x * params.numCubes.y * params.numCubes.z);

    for (unsigned int z = 0; z < params.numCubes.z; z++) {
      for (unsigned int y = 0; y < params.numCubes.y; y++) {
        for (unsigned int x = 0; x < params.numCubes.x; x++) {
          setCubeVertex (params, params.cubeIndex (x,y,z));
        }
      }
    }
  }

  void resolveAmbiguities (Parameters& params) {
    auto check = [&params] ( const Cube& cube, unsigned int x, unsigned int y, unsigned int z
                           , unsigned int ambiguousFace, int dim ) -> bool
    {
      assert (cube.isAmbiguous ());
      assert (dim == -3 || dim == -2 || dim == -1 || dim == 1 || dim == 2 || dim == 3);

      Cube& other = params.grid.at ( params.cubeIndex 
                                   ( dim == -1 ? x-1 : (dim == 1 ? x+1 : x)
                                   , dim == -2 ? y-1 : (dim == 2 ? y+1 : y)
                                   , dim == -3 ? z-1 : (dim == 3 ? z+1 : z) ) );
      if (other.isAmbiguous ()) {
        unsigned int otherAmbiguousFace = Util::invalidIndex ();
        const bool   hasOtherAmbiguousFace = other.hasAmbiguousFaces (&otherAmbiguousFace);

        assert (hasOtherAmbiguousFace);

        return (dim == -1 && ambiguousFace == 2 && otherAmbiguousFace == 3)
            || (dim ==  1 && ambiguousFace == 3 && otherAmbiguousFace == 2)
            || (dim == -2 && ambiguousFace == 0 && otherAmbiguousFace == 1)
            || (dim ==  2 && ambiguousFace == 1 && otherAmbiguousFace == 0)
            || (dim == -3 && ambiguousFace == 4 && otherAmbiguousFace == 5)
            || (dim ==  3 && ambiguousFace == 5 && otherAmbiguousFace == 4);
      }
      else {
        return false;
      }
    };

    for (unsigned int z = 0; z < params.numCubes.z; z++) {
      for (unsigned int y = 0; y < params.numCubes.y; y++) {
        for (unsigned int x = 0; x < params.numCubes.x; x++) {
          Cube& cube = params.grid.at (params.cubeIndex (x,y,z));

          if (cube.isAmbiguous ()) {
            unsigned int ambiguousFace = Util::invalidIndex ();
            const bool   hasAmbiguousFace = cube.hasAmbiguousFaces (&ambiguousFace);

            assert (hasAmbiguousFace);

            if ( (x > 0                   && check (cube, x, y, z, ambiguousFace, -1))
              || (x < params.numCubes.x-1 && check (cube, x, y, z, ambiguousFace,  1))
              || (y > 0                   && check (cube, x, y, z, ambiguousFace, -2))
              || (y < params.numCubes.y-1 && check (cube, x, y, z, ambiguousFace,  2))
              || (z > 0                   && check (cube, x, y, z, ambiguousFace, -3))
              || (z < params.numCubes.z-1 && check (cube, x, y, z, ambiguousFace,  3)) )
            {
              cube.collapseWhenAmbiguous = false;
            }
            else {
              cube.collapseWhenAmbiguous = true;
            }
          }
          else {
            cube.collapseWhenAmbiguous = false;
          }
        }
      }
    }
  }

  Mesh makeMesh (Parameters& params) {
    Mesh mesh;

    for (Cube& cube : params.grid) {
      for (unsigned int i = 0; i < cube.vertexInstanceIndices.size (); i++) {
        assert (cube.vertex != invalidVec3);
        cube.vertexInstanceIndices.at (i) = mesh.addVertex (cube.vertex);

        if (cube.collapseWhenAmbiguous) {
          break;
        }
      }
    }

    auto makeQuad = [&params, &mesh] ( unsigned int dim, bool swap
                                     , unsigned int i, unsigned int iu
                                     , unsigned int iv, unsigned int iuv )
    {
      unsigned int v1, v2, v3, v4;

      if (dim == 0) {
        v1 = params.grid.at (i)  .vertexInstanceIndex (0);
        v2 = params.grid.at (iu) .vertexInstanceIndex (3);
        v3 = params.grid.at (iuv).vertexInstanceIndex (9);
        v4 = params.grid.at (iv) .vertexInstanceIndex (6);
      }
      else if (dim == 1) {
        v1 = params.grid.at (i)  .vertexInstanceIndex (1);
        v2 = params.grid.at (iu) .vertexInstanceIndex (7);
        v3 = params.grid.at (iuv).vertexInstanceIndex (10);
        v4 = params.grid.at (iv) .vertexInstanceIndex (4);
      }
      else if (dim == 2) {
        v1 = params.grid.at (i)  .vertexInstanceIndex (2);
        v2 = params.grid.at (iu) .vertexInstanceIndex (5);
        v3 = params.grid.at (iuv).vertexInstanceIndex (11);
        v4 = params.grid.at (iv) .vertexInstanceIndex (8);
      }
      else {
        DILAY_IMPOSSIBLE
      }

      if (swap) {
        std::swap (v2, v4);
      }
      if ( glm::distance2 (mesh.vertex (v1), mesh.vertex (v3))
        <= glm::distance2 (mesh.vertex (v2), mesh.vertex (v4)) ) 
      {
        mesh.addIndex (v1); mesh.addIndex (v2); mesh.addIndex (v3);
        mesh.addIndex (v1); mesh.addIndex (v3); mesh.addIndex (v4);
      }
      else {
        mesh.addIndex (v2); mesh.addIndex (v3); mesh.addIndex (v4);
        mesh.addIndex (v2); mesh.addIndex (v4); mesh.addIndex (v1);
      }
    };

    auto makeFaces = [&params, &makeQuad]
                     (unsigned int dim, unsigned int x, unsigned int y, unsigned int z)
    {
      assert (dim == 0 || dim == 1 || dim == 2);

      const float s1 = params.samples.at (params.sampleIndex (x,y,z));
      const float s2 = params.samples.at (params.sampleIndex ( dim == 0 ? x+1 : x
                                                             , dim == 1 ? y+1 : y
                                                             , dim == 2 ? z+1 : z ));
      if (isIntersecting (s1, s2)) {
        const unsigned int i   = params.cubeIndex (x,y,z);
        const unsigned int u   = (dim + 1) % 3;
        const unsigned int v   = (dim + 2) % 3;
        const unsigned int iu  = params.cubeIndex ( u == 0 ? x-1 : x
                                                  , u == 1 ? y-1 : y
                                                  , u == 2 ? z-1 : z );
        const unsigned int iv  = params.cubeIndex ( v == 0 ? x-1 : x
                                                  , v == 1 ? y-1 : y
                                                  , v == 2 ? z-1 : z );

        const unsigned int iuv = dim == 0 ? params.cubeIndex (x  , y-1, z-1)
                             : ( dim == 1 ? params.cubeIndex (x-1, y  , z-1)
                             : (            params.cubeIndex (x-1, y-1, z  ) ));

        makeQuad (dim, s1 >= 0.0f, i, iu, iv, iuv);
      }
    };

    for (unsigned int z = 0; z < params.numCubes.z; z++) {
      for (unsigned int y = 0; y < params.numCubes.y; y++) {
        for (unsigned int x = 0; x < params.numCubes.x; x++) {
          if (y > 0 && z > 0) { makeFaces (0,x,y,z); }
          if (x > 0 && z > 0) { makeFaces (1,x,y,z); }
          if (x > 0 && y > 0) { makeFaces (2,x,y,z); }
        }
      }
    }

    assert (MeshUtil::checkConsistency (mesh));
    return mesh;
  }
}

Mesh SketchConversion :: convert (const SketchMesh& mesh, float resolution) {
  assert (mesh.isEmpty () == false);

  Parameters params;
  params.resolution = resolution;

  setupSampling (mesh, params);

  if (params.numSamples.x > 0 && params.numSamples.y > 0 && params.numSamples.z > 0) {
    sample             (mesh, params);
    makeGrid           (params);
    resolveAmbiguities (params);
    return makeMesh    (params);
  }
  else {
    return Mesh ();
  }
}
