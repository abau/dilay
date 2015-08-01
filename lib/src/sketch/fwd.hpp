/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_TREE_FWD
#define DILAY_SKETCH_TREE_FWD

#include "tree.hpp"

template <typename T> class Tree;
class SketchNodeData;
class SketchNodeIntersection;
using SketchNode = Tree <SketchNodeData>;
class SketchMesh;

#endif
