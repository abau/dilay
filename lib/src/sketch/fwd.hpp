/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_FWD
#define DILAY_SKETCH_FWD

#include "primitive/sphere.hpp"
#include "tree.hpp"

class SketchPath;
using SketchPaths = std::vector<SketchPath>;
class SketchNodeIntersection;
class SketchBoneIntersection;
class SketchMeshIntersection;
class SketchPathIntersection;
using SketchNode = TreeNode<PrimSphere>;
using SketchTree = Tree<PrimSphere>;
class SketchMesh;

#endif
