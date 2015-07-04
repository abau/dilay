/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "hash.hpp"
#include "intersection.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "util.hpp"

namespace {
  Mesh& finalized (Mesh& mesh) {
    for (unsigned int i = 0; i < mesh.numVertices (); i++) {
      mesh.setNormal (i, glm::normalize (mesh.vertex (i)));
    }
    return mesh;
  }

  unsigned int addVertex (Mesh& mesh, const glm::vec3& v) {
    return mesh.addVertex (v);
  }

  void addFace (Mesh& mesh, unsigned int i1, unsigned int i2, unsigned int i3) {
    mesh.addIndex (i1);
    mesh.addIndex (i2);
    mesh.addIndex (i3);
  }

  void addFace (Mesh& mesh, unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4) {
    mesh.addIndex (i1);
    mesh.addIndex (i2);
    mesh.addIndex (i3);
    mesh.addIndex (i4);
    mesh.addIndex (i1);
    mesh.addIndex (i3);
  }
}

Mesh MeshUtil :: cube () {
  Mesh mesh;
  const float d = 0.5f;

  addVertex (mesh, glm::vec3 (-d, -d, -d));
  addVertex (mesh, glm::vec3 (-d, -d, +d));
  addVertex (mesh, glm::vec3 (-d, +d, -d));
  addVertex (mesh, glm::vec3 (-d, +d, +d));
  addVertex (mesh, glm::vec3 (+d, -d, -d));
  addVertex (mesh, glm::vec3 (+d, -d, +d));
  addVertex (mesh, glm::vec3 (+d, +d, -d));
  addVertex (mesh, glm::vec3 (+d, +d, +d));

  addFace (mesh, 0, 1, 3, 2);
  addFace (mesh, 1, 5, 7, 3);
  addFace (mesh, 5, 4, 6, 7);
  addFace (mesh, 4, 0, 2, 6);
  addFace (mesh, 3, 7, 6, 2);
  addFace (mesh, 0, 4, 5, 1);

  return finalized (mesh);
}

Mesh MeshUtil :: sphere (unsigned int rings, unsigned int sectors) {
  assert (rings > 1 && sectors > 2);
  Mesh mesh;

  const float radius     = 1.0f;
  const float ringStep   =        glm::pi <float> () / float (rings);
  const float sectorStep = 2.0f * glm::pi <float> () / float (sectors);
        float phi        = ringStep;
        float theta      = 0.0f;

  // inner rings vertices
  for (unsigned int r = 0; r < rings - 1; r++) {
    for (unsigned int s = 0; s < sectors; s++) {
      const float x = radius * sin (theta) * sin (phi);
      const float y = radius * cos (phi);
      const float z = radius * cos (theta) * sin (phi);

      addVertex (mesh, glm::vec3 (x,y,z));

      theta += sectorStep;
    }
    phi += ringStep;
  }

  // caps vertices
  const unsigned int topCapIndex = addVertex (mesh, glm::vec3 (0.0f, radius, 0.0f));
  const unsigned int botCapIndex = addVertex (mesh, glm::vec3 (0.0f,-radius, 0.0f));

  // inner rings indices
  for (unsigned int r = 0; r < rings - 2; r++) {
    for (unsigned int s = 0; s < sectors; s++) {
      addFace ( mesh
              , (sectors * r) + s
              , (sectors * (r+1)) + s
              , (sectors * (r+1)) + ((s+1) % sectors) 
              , (sectors * r) + ((s+1) % sectors) );
    }
  }

  // caps indices
  for (unsigned int s = 0; s < sectors; s++) {
    addFace (mesh, topCapIndex, s, (s+1) % sectors);

    addFace ( mesh
            , botCapIndex
            , (sectors * (rings-2)) + ((s+1) % sectors)
            , (sectors * (rings-2)) + s);
  }
  return finalized (mesh);
}

Mesh MeshUtil :: icosphere (unsigned int numSubdivisions) {
  typedef std::unordered_map <ui_pair, unsigned int> VertexCache;

  Mesh        mesh;
  VertexCache vertexCache;

  // adds new vertex to ico-sphere
  auto addIcoVertex = [&mesh] (const glm::vec3& v) -> unsigned int {
    return addVertex (mesh, glm::normalize (v));
  };

  // looks up vertex in cache or computes a new one
  auto lookupVertex = [&vertexCache,&addIcoVertex,&mesh]
    (unsigned int i1, unsigned int i2) -> unsigned int 
    {
      const unsigned int lowerI = i1 < i2 ? i1 : i2;
      const unsigned int upperI = i1 < i2 ? i2 : i1;
      const ui_pair      key    = std::make_pair (lowerI, upperI);

      VertexCache::iterator it = vertexCache.find (key);
      if (it == vertexCache.end ()) {
        const unsigned int n = addIcoVertex (Util::between ( mesh.vertex (lowerI)
                                                           , mesh.vertex (upperI)));
        vertexCache.emplace (key, n);
        return n;
      }
      else {
        return it->second;
      }
    };

  // subdivides a face of the ico-sphere
  std::function <void (unsigned int,unsigned int,unsigned int,unsigned int)> subdivide =
    [&mesh,&subdivide,&lookupVertex] 
    (unsigned int s,unsigned int i1, unsigned int i2, unsigned int i3) -> void 
    {
      if (s == 0) {
        addFace (mesh, i1, i2, i3); 
      }
      else {
        const unsigned int i12 = lookupVertex (i1,i2);
        const unsigned int i23 = lookupVertex (i2,i3);
        const unsigned int i31 = lookupVertex (i3,i1);

        subdivide (s-1, i1 , i12, i31);
        subdivide (s-1, i2 , i23, i12);
        subdivide (s-1, i3 , i31, i23);
        subdivide (s-1, i12, i23, i31);
      }
    };

  const float t = (1.0f + glm::sqrt (5.0f)) * 0.5f;

  addIcoVertex (glm::vec3 (-1.0f, +t   ,  0.0f));
  addIcoVertex (glm::vec3 (+1.0f, +t   ,  0.0f));
  addIcoVertex (glm::vec3 (-1.0f, -t   ,  0.0f));
  addIcoVertex (glm::vec3 (+1.0f, -t   ,  0.0f));

  addIcoVertex (glm::vec3 ( 0.0f, -1.0f, +t   ));
  addIcoVertex (glm::vec3 ( 0.0f, +1.0f, +t   ));
  addIcoVertex (glm::vec3 ( 0.0f, -1.0f, -t   ));
  addIcoVertex (glm::vec3 ( 0.0f, +1.0f, -t   ));

  addIcoVertex (glm::vec3 (+t   ,  0.0f, -1.0f));
  addIcoVertex (glm::vec3 (+t   ,  0.0f, +1.0f));
  addIcoVertex (glm::vec3 (-t   ,  0.0f, -1.0f));
  addIcoVertex (glm::vec3 (-t   ,  0.0f, +1.0f));

  subdivide (numSubdivisions, 0 ,11,5 ); 
  subdivide (numSubdivisions, 0 ,5 ,1 ); 
  subdivide (numSubdivisions, 0 ,1 ,7 ); 
  subdivide (numSubdivisions, 0 ,7 ,10); 
  subdivide (numSubdivisions, 0 ,10,11); 

  subdivide (numSubdivisions, 1 ,5 ,9 ); 
  subdivide (numSubdivisions, 5 ,11,4 ); 
  subdivide (numSubdivisions, 11,10,2 ); 
  subdivide (numSubdivisions, 10,7 ,6 ); 
  subdivide (numSubdivisions, 7 ,1 ,8 ); 

  subdivide (numSubdivisions, 3 ,9 ,4 ); 
  subdivide (numSubdivisions, 3 ,4 ,2 ); 
  subdivide (numSubdivisions, 3 ,2 ,6 ); 
  subdivide (numSubdivisions, 3 ,6 ,8 ); 
  subdivide (numSubdivisions, 3 ,8 ,9 ); 

  subdivide (numSubdivisions, 4 ,9 ,5 ); 
  subdivide (numSubdivisions, 2 ,4 ,11); 
  subdivide (numSubdivisions, 6 ,2 ,10); 
  subdivide (numSubdivisions, 8 ,6 ,7 ); 
  subdivide (numSubdivisions, 9 ,8 ,1 ); 

  return finalized (mesh);
}

Mesh MeshUtil :: cone (unsigned int numBaseVertices) {
  assert (numBaseVertices >= 3);

  Mesh mesh;
  const float c = 2.0f * glm::pi <float> () / float (numBaseVertices);

  for (unsigned int i = 0; i < numBaseVertices; i++) {
    addVertex (mesh, glm::vec3 ( glm::sin (float (i) * c)
                               , -0.5f
                               , glm::cos (float (i) * c)));
  }
  addVertex (mesh, glm::vec3 (0.0f, -0.5f, 0.0f));
  addVertex (mesh, glm::vec3 (0.0f,  0.5f, 0.0f));

  for (unsigned int i = 0; i < numBaseVertices - 1; i++) {
    addFace (mesh, i, i + 1, numBaseVertices + 1);
    addFace (mesh, i + 1, i, numBaseVertices);
  }
  addFace (mesh, numBaseVertices - 1, 0, numBaseVertices + 1);
  addFace (mesh, 0, numBaseVertices - 1, numBaseVertices);

  return finalized (mesh);
}

Mesh MeshUtil :: cylinder (unsigned int numVertices) {
  assert (numVertices >= 3);

  Mesh mesh;
  const float c = 2.0f * glm::pi <float> () / float (numVertices);

  for (unsigned int i = 0; i < numVertices; i++) {
    addVertex (mesh, glm::vec3 ( glm::sin (float (i) * c)
                               , -0.5f
                               , glm::cos (float (i) * c)));
  }
  for (unsigned int i = 0; i < numVertices; i++) {
    addVertex (mesh, glm::vec3 ( glm::sin (float (i) * c)
                               , 0.5f
                               , glm::cos (float (i) * c)));
  }
  addVertex (mesh, glm::vec3 (0.0f, -0.5f, 0.0f));
  addVertex (mesh, glm::vec3 (0.0f,  0.5f, 0.0f));

  for (unsigned int i = 0; i < numVertices - 1; i++) {
    addFace (mesh, i, i + 1, i + numVertices + 1, i + numVertices);

    addFace (mesh, i + 1, i, 2 * numVertices);
    addFace (mesh, i + numVertices, i + numVertices + 1, (2 * numVertices) + 1);
  }
  addFace (mesh, numVertices - 1, 0, numVertices, (2 * numVertices) - 1);

  addFace (mesh, 0, numVertices - 1, 2 * numVertices);
  addFace (mesh, (2 * numVertices) - 1, numVertices, (2 * numVertices) + 1);

  return finalized (mesh);
}

Mesh MeshUtil :: mirror (const Mesh& mesh, const PrimPlane& plane) {
  assert (MeshUtil::checkConsistency (mesh));

  enum class Side       { Negative, Border, Positive };
  enum class BorderFlag { NoBorder, ConnectsNegative, ConnectsPositive, ConnectsBoth };

  auto side = [&plane] (const glm::vec3& v) -> Side {
    const float eps = Util::epsilon () * 0.5f;
    const float d   = plane.distance (v);

    return d < -eps ? Side::Negative
                   : ( d > eps ? Side::Positive 
                               : Side::Border );
  };

  Mesh                                       m (mesh, false);
  std::vector        <glm::vec3>             positions;
  std::vector        <Side>                  sides;
  std::vector        <BorderFlag>            borderFlags;
  std::vector        <ui_pair>               newIndices;
  std::unordered_map <ui_pair, unsigned int> newBorderVertices;

  auto updateBorderFlag = [&borderFlags] (unsigned int i, Side side) {
    BorderFlag& current = borderFlags [i];

    if (side == Side::Border) {}
    else if (side == Side::Negative) {
      if (current == BorderFlag::NoBorder) {
        current = BorderFlag::ConnectsNegative;
      }
      else if (current == BorderFlag::ConnectsPositive) {
        current = BorderFlag::ConnectsBoth;
      }
    }
    else if (side == Side::Positive) {
      if (current == BorderFlag::NoBorder) {
        current = BorderFlag::ConnectsPositive;
      }
      else if (current == BorderFlag::ConnectsNegative) {
        current = BorderFlag::ConnectsBoth;
      }
    }
  };

  auto newBorderVertex = [&mesh, &plane, &newBorderVertices, &m] 
                         (unsigned int i1, unsigned int i2) -> unsigned int 
  {
    const ui_pair key = std::make_pair ( glm::min (i1, i2)
                                       , glm::max (i1, i2) );
    auto it = newBorderVertices.find (key);
    if (it != newBorderVertices.end ()) {
      return it->second;
    }
    else {
      const glm::vec3 v1 (mesh.vertex (i1));
      const glm::vec3 v2 (mesh.vertex (i2));
      const PrimRay ray  (true, v1, v2 - v1);

      float     t;
      glm::vec3 position;

      if (IntersectionUtil::intersects (ray, plane, &t)) {
        position = ray.pointAt (t);
      }
      else {
        position = (v1 + v2) * 0.5f; 
      }
      const unsigned int newIndex = m.addVertex (position);

      newBorderVertices.emplace (key, newIndex);
      return newIndex;
    }
  };

  positions  .reserve (mesh.numVertices ());
  sides      .reserve (mesh.numVertices ());
  borderFlags.reserve (mesh.numVertices ());

  newIndices.resize ( mesh.numVertices ()
                    , std::make_pair (Util::invalidIndex (), Util::invalidIndex ()) );

  // cache data
  for (unsigned int i = 0; i < mesh.numVertices (); i++) {
    positions  .push_back (mesh.vertex (i));
    sides      .push_back (side (positions.back ()));
    borderFlags.push_back (BorderFlag::NoBorder);
  }

  // update border flags
  for (unsigned int i = 0; i < mesh.numIndices (); i += 3) {
    const unsigned int i1 = mesh.index (i + 0);
    const unsigned int i2 = mesh.index (i + 1);
    const unsigned int i3 = mesh.index (i + 2);

    assert (sides[i1] != Side::Border || sides[i2] != Side::Border || sides[i3] != Side::Border);

    updateBorderFlag (i1, sides [i2]);
    updateBorderFlag (i1, sides [i3]);
    updateBorderFlag (i2, sides [i1]);
    updateBorderFlag (i2, sides [i3]);
    updateBorderFlag (i3, sides [i1]);
    updateBorderFlag (i3, sides [i2]);
  }

  // mirror vertices
  for (unsigned int i = 0; i < mesh.numVertices (); i++) {
    const glm::vec3& v = positions [i];

    switch (sides [i]) {
      case Side::Negative: break;

      case Side::Border: {
        switch (borderFlags [i]) {
          case BorderFlag::NoBorder:
            DILAY_IMPOSSIBLE
            break;
          case BorderFlag::ConnectsNegative:
            break;
          case BorderFlag::ConnectsPositive: {
            const unsigned int index1 = m.addVertex (v);
            const unsigned int index2 = m.addVertex (v);

            newIndices [i] = std::make_pair (index1, index2);
            break;
          }
          case BorderFlag::ConnectsBoth: {
            const unsigned int index = m.addVertex (v);

            newIndices [i] = std::make_pair (index, index);
            break;
          }
        }
        break;
      }
      case Side::Positive: {
        const unsigned int index1 = m.addVertex (v);
        const unsigned int index2 = m.addVertex (plane.mirror (v));

        newIndices [i] = std::make_pair (index1, index2);
      }
    }
  }

  // mirror faces
  for (unsigned int i = 0; i < mesh.numIndices (); i += 3) {
    const unsigned int oldIndex1 = mesh.index (i + 0);
    const unsigned int oldIndex2 = mesh.index (i + 1);
    const unsigned int oldIndex3 = mesh.index (i + 2);

    const Side s1 = sides [oldIndex1];
    const Side s2 = sides [oldIndex2];
    const Side s3 = sides [oldIndex3];

    if (s1 == Side::Positive || s2 == Side::Positive || s3 == Side::Positive) {
      const ui_pair& new1 = newIndices [oldIndex1];
      const ui_pair& new2 = newIndices [oldIndex2];
      const ui_pair& new3 = newIndices [oldIndex3];

      // 3 non-negative
      if (s1 != Side::Negative && s2 != Side::Negative && s3 != Side::Negative) {
        addFace (m, new1.first, new2.first, new3.first);
        addFace (m, new3.second, new2.second, new1.second);
      }
      // 1 negative - 2 positive
      else if (s1 == Side::Positive && s2 == Side::Positive && s3 == Side::Negative) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex3);
        unsigned int b2 = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new2.first, b2, new1.first);
        addFace (m, new1.second, b2, new2.second);

        addFace (m, new1.first, b2, b1);
        addFace (m, b1, b2, new1.second);
      }
      else if (s1 == Side::Positive && s2 == Side::Negative && s3 == Side::Positive) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex2);
        unsigned int b2 = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new1.first, b1, new3.first);
        addFace (m, new3.second, b1, new1.second);

        addFace (m, new3.first, b1, b2);
        addFace (m, b2, b1, new3.second);
      }
      else if (s1 == Side::Negative && s2 == Side::Positive && s3 == Side::Positive) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex2);
        unsigned int b2 = newBorderVertex (oldIndex1, oldIndex3);

        addFace (m, new3.first, b2, new2.first);
        addFace (m, new2.second, b2, new3.second);

        addFace (m, new2.first, b2, b1);
        addFace (m, b1, b2, new2.second);
      }
      // 1 positive - 2 negative
      else if (s1 == Side::Positive && s2 == Side::Negative && s3 == Side::Negative) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex2);
        unsigned int b2 = newBorderVertex (oldIndex1, oldIndex3);

        addFace (m, new1.first, b1, b2);
        addFace (m, b2, b1, new1.second);
      }
      else if (s1 == Side::Negative && s2 == Side::Positive && s3 == Side::Negative) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex2);
        unsigned int b2 = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new2.first, b2, b1);
        addFace (m, b1, b2, new2.second);
      }
      else if (s1 == Side::Negative && s2 == Side::Negative && s3 == Side::Positive) {
        unsigned int b1 = newBorderVertex (oldIndex1, oldIndex3);
        unsigned int b2 = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new3.first, b1, b2);
        addFace (m, b2, b1, new3.second);
      }
      // 1 positive - 1 border - 1 negative
      else if (s1 == Side::Positive && s2 == Side::Border && s3 == Side::Negative) {
        assert (borderFlags [oldIndex2] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex1, oldIndex3);

        addFace (m, new1.first, new2.first, b);
        addFace (m, b, new2.second, new1.second);
      }
      else if (s1 == Side::Border && s2 == Side::Positive && s3 == Side::Negative) {
        assert (borderFlags [oldIndex1] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new1.first, new2.first, b);
        addFace (m, b, new2.second, new1.second);
      }
      else if (s1 == Side::Positive && s2 == Side::Negative && s3 == Side::Border) {
        assert (borderFlags [oldIndex3] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex1, oldIndex2);

        addFace (m, new1.first, b, new3.first);
        addFace (m, new3.second, b, new1.second);
      }
      else if (s1 == Side::Border && s2 == Side::Negative && s3 == Side::Positive) {
        assert (borderFlags [oldIndex1] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex2, oldIndex3);

        addFace (m, new1.first, b, new3.first);
        addFace (m, new3.second, b, new1.second);
      }
      else if (s1 == Side::Negative && s2 == Side::Positive && s3 == Side::Border) {
        assert (borderFlags [oldIndex3] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex1, oldIndex2);

        addFace (m, new2.first, new3.first, b);
        addFace (m, b, new3.second, new2.second);
      }
      else if (s1 == Side::Negative && s2 == Side::Border && s3 == Side::Positive) {
        assert (borderFlags [oldIndex2] == BorderFlag::ConnectsBoth);

        unsigned int b = newBorderVertex (oldIndex1, oldIndex3);

        addFace (m, new2.first, new3.first, b);
        addFace (m, b, new3.second, new2.second);
      }
      else {
        DILAY_IMPOSSIBLE
      }
    }
  }
  assert (MeshUtil::checkConsistency (m));
  return m;
}

bool MeshUtil :: checkConsistency (const Mesh& mesh) {
  if (mesh.numVertices () == 0) {
    DILAY_WARN ("empty mesh");
    return false;
  }
  std::unordered_map <ui_pair, unsigned int> numEdgeAdjacentFaces;
  std::vector        <unsigned int>          numVertexAdjacentFaces;

  numVertexAdjacentFaces.resize (mesh.numVertices (), 0);

  auto addEdgeAdjFace = [&numEdgeAdjacentFaces] (unsigned int i, unsigned int j) {
    ui_pair key (glm::min (i,j), glm::max (i,j));

    auto it = numEdgeAdjacentFaces.find (key);
    if (it == numEdgeAdjacentFaces.end ()) {
      numEdgeAdjacentFaces.emplace (key, 1);
    }
    else {
      it->second += 1;
    }
  };

  auto addVertexAdjFace = [&numVertexAdjacentFaces] (unsigned int i) {
    numVertexAdjacentFaces [i] += 1;
  };

  for (unsigned int i = 0; i < mesh.numIndices (); i+=3) {
    const unsigned int i1 = mesh.index (i+0);
    const unsigned int i2 = mesh.index (i+1);
    const unsigned int i3 = mesh.index (i+2);

    addVertexAdjFace (i1);
    addVertexAdjFace (i2);
    addVertexAdjFace (i3);

    addEdgeAdjFace (i1,i2);
    addEdgeAdjFace (i2,i3);
    addEdgeAdjFace (i1,i3);
  }

  for (unsigned int v = 0; v < numVertexAdjacentFaces.size (); v++) {
    if (numVertexAdjacentFaces [v] < 3) {
      DILAY_WARN ("inconsistent vertex %u with %u adjacent faces", v, numVertexAdjacentFaces [v]);
      return false;
    }
  }
  for (auto it : numEdgeAdjacentFaces) {
    if (it.second != 2) {
      const unsigned int first  = it.first.first;
      const unsigned int second = it.first.second;

      DILAY_WARN ("inconsistent edge (%u,%u) with %u adjacent faces", first, second, it.second);
      return false;
    }
  }
  return true;
}

void MeshUtil :: toObjFile (std::ostream& stream, const Mesh& mesh) {
  for (unsigned int i = 0; i < mesh.numVertices (); i++) {
    const glm::vec3 v = mesh.vertex (i);

    stream << "v " << v.x << " " << v.y << " " << v.z << std::endl;
  }
  for (unsigned int i = 0; i < mesh.numIndices (); i += 3) {
    stream << "f " << mesh.index (i + 0) + 1 << " " 
                   << mesh.index (i + 1) + 1 << " " 
                   << mesh.index (i + 2) + 1 << std::endl;
  }
}

bool MeshUtil :: fromObjFile (std::istream& stream, std::vector <Mesh>& meshes) {
  unsigned int lineNumber = 1;

  meshes.clear ();

  auto extractFirst = [] (const std::string& string, unsigned int& value) -> bool {
    auto slashPos = string.find ('/');
    return Util::fromString (string.substr (0, slashPos), value);
  };

  for (std::string line; std::getline (stream, line); ) {
    std::istringstream lineStream (line);
    std::string        keyword;

    lineStream >> keyword;

    if (keyword.empty () == false) {
      if (keyword == "o") {
        meshes.push_back (Mesh ());
      }
      else {
        if (meshes.empty ()) {
          meshes.push_back (Mesh ());
        }
        if (keyword == "v") {
          float x, y, z;
          lineStream >> x >> y >> z;

          if (lineStream.fail ()) {
            DILAY_WARN ("could not parse vertex at line %u", lineNumber)
            return false;
          }
          else {
            addVertex (meshes.back (), glm::vec3 (x, y, z));
          }
        }
        else if (keyword == "f") {
          std::string v1String, v2String, v3String;
          unsigned int v1, v2, v3;

          lineStream >> v1String >> v2String >> v3String;

          if ( lineStream.fail () || extractFirst (v1String, v1) == false
                                  || extractFirst (v2String, v2) == false
                                  || extractFirst (v3String, v3) == false )
          {
            DILAY_WARN ("could not parse face at line %u", lineNumber)
            return false;
          }
          else {
            std::string  v4String;
            unsigned int v4;
            lineStream >> v4String;

            if (lineStream.fail ()) {
              lineStream.clear ();
              addFace (meshes.back (), v1-1, v2-1, v3-1);
            }
            else if (extractFirst (v4String, v4) == false) {
              DILAY_WARN ("could not parse face at line %u", lineNumber)
              return false;
            }
            else {
              addFace (meshes.back (), v1-1, v2-1, v3-1, v4-1);
            }
          }
        }
      }
    }
    lineNumber++;
  }
  meshes.erase ( std::remove_if ( meshes.begin ()
                                , meshes.end   ()
                                , [] (Mesh& m) { return m.numVertices () == 0; } )
               , meshes.end () );

  if (std::all_of ( meshes.begin (), meshes.end ()
                  , [] (Mesh& m) { return checkConsistency (m); } ))
  {
    return true;
  }
  else {
    meshes.clear ();
    return false;
  }
}
