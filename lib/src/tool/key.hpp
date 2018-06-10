/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_KEY
#define DILAY_TOOL_KEY

#include <functional>

enum class ToolKey
{
  TransformMesh,
  DeleteMesh,
  NewMesh,
  SculptDraw,
  SculptGrab,
  SculptSmooth,
  SculptFlatten,
  SculptCrease,
  SculptPinch,
  SculptReduce,
  EditSketch,
  DeleteSketch,
  ConvertSketch,
  SketchSpheres,
  TrimMesh,
  Remesh,
  MoveCamera
};

namespace std
{
  template <> struct hash<ToolKey>
  {
    size_t operator() (const ToolKey& key) const { return std::hash<int> () (int(key)); }
  };
}

#endif
