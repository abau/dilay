/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <algorithm>
#include <fstream>
#include <sstream>
#include "dynamic/mesh.hpp"
#include "import-export.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "sketch/fwd.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "util.hpp"

namespace
{
  void toDlyFile (std::ostream& stream, const Mesh& mesh)
  {
    stream << "o\n";
    for (unsigned int i = 0; i < mesh.numVertices (); i++)
    {
      stream << "v " << mesh.vertex (i) << std::endl;
    }
    for (unsigned int i = 0; i < mesh.numIndices (); i += 3)
    {
      stream << "f " << mesh.index (i + 0) + 1 << " " << mesh.index (i + 1) + 1 << " "
             << mesh.index (i + 2) + 1 << std::endl;
    }
  }

  unsigned int toDlyFile (std::ostream& stream, const SketchNode& node, unsigned int parentIndex,
                          unsigned nodeIndex)
  {
    stream << "dly_sketch_node " << nodeIndex << " " << parentIndex << " " << node.data ().center ()
           << " " << node.data ().radius () << std::endl;

    unsigned int childIndex = nodeIndex;

    node.forEachConstChild (
      [&stream, parentIndex, nodeIndex, &childIndex](const SketchNode& child) {
        childIndex = toDlyFile (stream, child, nodeIndex, childIndex + 1);
      });
    return childIndex;
  }

  void toDlyFile (std::ostream& stream, const SketchPath& path)
  {
    if (path.isEmpty () == false)
    {
      stream << "dly_sketch_path"
             << " " << path.intersectionFirst () << " " << path.intersectionLast () << std::endl;

      for (const PrimSphere& s : path.spheres ())
      {
        stream << "dly_sketch_sphere"
               << " " << s.center () << " " << s.radius () << std::endl;
      }
    }
  }

  void toDlyFile (std::ostream& stream, const SketchMesh& mesh)
  {
    if (mesh.isEmpty () == false)
    {
      stream << "dly_sketch_mesh\n";

      if (mesh.tree ().hasRoot ())
      {
        toDlyFile (stream, mesh.tree ().root (), Util::invalidIndex (), 0);
      }

      for (const SketchPath& p : mesh.paths ())
      {
        toDlyFile (stream, p);
      }
    }
  }
};

namespace ImportExport
{
  void toDlyFile (std::ostream& stream, Scene& scene, bool isObjFile)
  {
    scene.forEachMesh ([&stream](DynamicMesh& mesh) {
      mesh.prune ();
      ::toDlyFile (stream, mesh.mesh ());
    });

    if (isObjFile == false)
    {
      scene.forEachConstMesh ([&stream](const SketchMesh& mesh) { ::toDlyFile (stream, mesh); });
    }
  }

  bool toDlyFile (const std::string& fileName, Scene& scene, bool isObjFile)
  {
    std::ofstream file (fileName);

    if (file.is_open ())
    {
      ImportExport::toDlyFile (file, scene, isObjFile);
      file.close ();
      return true;
    }
    else
    {
      return false;
    }
  }

  bool fromDlyFile (std::istream& stream, const Config& config, Scene& scene)
  {
    unsigned int       lineNumber = 0;
    std::istringstream lineStream;

    std::vector<Mesh>        meshes;
    std::vector<SketchNode*> nodes;
    SketchMesh*              sketch;
    SketchPath*              sketchPath;
    glm::vec3                intersectionFirst, intersectionLast;

    auto extractFirst = [](const std::string& string, unsigned int& value) -> bool {
      auto slashPos = string.find ('/');
      return Util::fromString (string.substr (0, slashPos), value);
    };

    auto nextLine = [&stream, &lineNumber, &lineStream]() -> bool {
      std::string line;
      std::getline (stream, line);

      if (stream)
      {
        lineNumber++;
        lineStream.clear ();
        lineStream.str (line);
        return true;
      }
      else
      {
        return false;
      }
    };

    while (nextLine ())
    {
      std::string keyword;
      lineStream >> keyword;

      if (lineStream.fail () == false)
      {
        if (keyword == "o")
        {
          meshes.push_back (Mesh ());
        }
        else if (keyword == "v")
        {
          glm::vec3 vertex;
          lineStream >> vertex;

          if (lineStream.fail ())
          {
            DILAY_WARN ("could not parse vertex at line %u", lineNumber)
            return false;
          }
          else
          {
            if (meshes.empty ())
            {
              meshes.push_back (Mesh ());
            }
            meshes.back ().addVertex (vertex);
          }
        }
        else if (keyword == "f")
        {
          std::string  v1String, v2String, v3String;
          unsigned int v1, v2, v3;

          lineStream >> v1String >> v2String >> v3String;

          if (lineStream.fail () || extractFirst (v1String, v1) == false ||
              extractFirst (v2String, v2) == false || extractFirst (v3String, v3) == false)
          {
            DILAY_WARN ("could not parse face at line %u", lineNumber)
            return false;
          }
          else
          {
            std::string  v4String;
            unsigned int v4;
            lineStream >> v4String;

            if (lineStream.fail ())
            {
              lineStream.clear ();

              if (meshes.empty ())
              {
                meshes.push_back (Mesh ());
              }
              MeshUtil::addFace (meshes.back (), v1 - 1, v2 - 1, v3 - 1);
            }
            else if (extractFirst (v4String, v4) == false)
            {
              DILAY_WARN ("could not parse face at line %u", lineNumber)
              return false;
            }
            else
            {
              if (meshes.empty ())
              {
                meshes.push_back (Mesh ());
              }
              MeshUtil::addFace (meshes.back (), v1 - 1, v2 - 1, v3 - 1, v4 - 1);
            }
          }
        }
        else if (keyword == "dly_sketch_mesh")
        {
          nodes.clear ();
          sketch = &scene.newSketchMesh (config, SketchTree ());
        }
        else if (keyword == "dly_sketch_node")
        {
          if (sketch == nullptr)
          {
            DILAY_WARN ("could not parse sketch node: no sketch found at line %u", lineNumber)
            return false;
          }
          unsigned int nodeIndex;
          unsigned int parentIndex;
          glm::vec3    center;
          float        radius;

          lineStream >> nodeIndex >> parentIndex >> center >> radius;

          if (lineStream.fail ())
          {
            DILAY_WARN ("could not parse sketch node at line %u", lineNumber)
            return false;
          }

          if (nodeIndex == nodes.size ())
          {
            if (nodeIndex == 0)
            {
              nodes.push_back (&sketch->tree ().emplaceRoot (PrimSphere (center, radius)));
            }
            else if (parentIndex < nodes.size ())
            {
              nodes.push_back (&nodes.at (parentIndex)->emplaceChild (PrimSphere (center, radius)));
            }
            else
            {
              DILAY_WARN ("invalid parent index at line %u", lineNumber)
              return false;
            }
          }
          else
          {
            DILAY_WARN ("invalid node index at line %u", lineNumber)
            return false;
          }
        }
        else if (keyword == "dly_sketch_path")
        {
          lineStream >> intersectionFirst >> intersectionLast;

          if (lineStream.fail ())
          {
            DILAY_WARN ("could not parse sketch path at line %u", lineNumber)
            return false;
          }
          if (sketch)
          {
            sketchPath = &sketch->addPath (SketchPath ());
          }
          else
          {
            DILAY_WARN ("could not parse sketch path: no sketch found at line %u", lineNumber)
            return false;
          }
        }
        else if (keyword == "dly_sketch_sphere")
        {
          glm::vec3 center;
          float     radius;

          lineStream >> center >> radius;

          if (lineStream.fail ())
          {
            DILAY_WARN ("could not parse sketch sphere at line %u", lineNumber)
            return false;
          }

          if (sketchPath)
          {
            if (sketchPath->isEmpty ())
            {
              sketchPath->addSphere (intersectionFirst, center, radius);
            }
            else
            {
              sketchPath->addSphere (intersectionLast, center, radius);
            }
          }
          else
          {
            DILAY_WARN ("could not parse sketch sphere: no sketch path found at line %u",
                        lineNumber)
            return false;
          }
        }
      }
    }
    meshes.erase (std::remove_if (meshes.begin (), meshes.end (),
                                  [](Mesh& m) { return m.numVertices () == 0; }),
                  meshes.end ());

    if (std::all_of (meshes.begin (), meshes.end (),
                     [](Mesh& m) { return MeshUtil::checkConsistency (m); }))
    {
      for (Mesh& m : meshes)
      {
        scene.newDynamicMesh (config, m);
      }
      return true;
    }
    else
    {
      return false;
    }
  }

  bool fromDlyFile (const std::string& fileName, const Config& config, Scene& scene)
  {
    std::ifstream file (fileName);

    if (file.is_open ())
    {
      const bool success = ImportExport::fromDlyFile (file, config, scene);
      file.close ();
      return success;
    }
    else
    {
      return false;
    }
  }
};
