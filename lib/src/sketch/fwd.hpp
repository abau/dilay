/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_TREE_FWD
#define DILAY_SKETCH_TREE_FWD

#include "primitive/sphere.hpp"
#include "tree.hpp"

class SketchNodeIntersection;
class SketchBoneIntersection;
class SketchMeshIntersection;
using SketchNode = TreeNode <PrimSphere>;
using SketchTree = Tree <PrimSphere>;
class SketchMesh;

#endif
