/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "../mesh.hpp"
#include "config.hpp"
#include "dynamic/faces.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "dynamic/octree.hpp"
#include "intersection.hpp"
#include "mesh-util.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "tool/sculpt/util/action.hpp"
#include "util.hpp"

namespace
{
  struct VertexData
  {
    bool                      isFree;
    std::vector<unsigned int> adjacentFaces;

    VertexData ()
    {
      this->reset ();
    }
    void reset ()
    {
      this->isFree = true;
      this->adjacentFaces.clear ();
    }

    void addAdjacentFace (unsigned int face)
    {
      this->adjacentFaces.push_back (face);
    }
    void deleteAdjacentFace (unsigned int face)
    {
      for (auto it = this->adjacentFaces.begin (); it != this->adjacentFaces.end (); ++it)
      {
        if (*it == face)
        {
          this->adjacentFaces.erase (it);
          return;
        }
      }
      DILAY_IMPOSSIBLE
    }
  };

  struct FaceData
  {
    bool isFree;

    FaceData ()
    {
      this->reset ();
    }
    void reset ()
    {
      this->isFree = true;
    }
  };
}

struct DynamicMesh::Impl
{
  DynamicMesh*               self;
  Mesh                       mesh;
  std::vector<VertexData>    vertexData;
  std::vector<unsigned char> vertexVisited;
  std::vector<unsigned int>  freeVertexIndices;
  std::vector<FaceData>      faceData;
  std::vector<unsigned char> faceVisited;
  std::vector<unsigned int>  freeFaceIndices;
  DynamicOctree              octree;

  Impl (DynamicMesh* s, const Mesh& m)
    : self (s)
  {
    this->fromMesh (m);
  }

  unsigned int numVertices () const
  {
    assert (this->mesh.numVertices () >= this->freeVertexIndices.size ());
    return this->mesh.numVertices () - this->freeVertexIndices.size ();
  }

  unsigned int numFaces () const
  {
    assert (this->faceData.size () >= this->freeFaceIndices.size ());
    return this->faceData.size () - this->freeFaceIndices.size ();
  }

  bool isEmpty () const
  {
    return this->numFaces () == 0;
  }

  bool isFreeVertex (unsigned int i) const
  {
    assert (i < this->vertexData.size ());
    return this->vertexData[i].isFree;
  }

  bool isFreeFace (unsigned int i) const
  {
    assert (i < this->faceData.size ());
    return this->faceData[i].isFree;
  }

  bool isPruned () const
  {
    return this->freeFaceIndices.empty () && this->freeVertexIndices.empty ();
  }

  unsigned int valence (unsigned int i) const
  {
    assert (this->isFreeVertex (i) == false);
    return this->vertexData[i].adjacentFaces.size ();
  }

  void vertexIndices (unsigned int i, unsigned int& i1, unsigned int& i2, unsigned int& i3) const
  {
    assert (this->isFreeFace (i) == false);

    i1 = this->mesh.index ((3 * i) + 0);
    i2 = this->mesh.index ((3 * i) + 1);
    i3 = this->mesh.index ((3 * i) + 2);
  }

  PrimTriangle face (unsigned int i) const
  {
    assert (this->isFreeFace (i) == false);

    return PrimTriangle (this->mesh.vertex (this->mesh.index ((3 * i) + 0)),
                         this->mesh.vertex (this->mesh.index ((3 * i) + 1)),
                         this->mesh.vertex (this->mesh.index ((3 * i) + 2)));
  }

  const glm::vec3& vertexNormal (unsigned int i) const
  {
    return this->mesh.normal (i);
  }

  const std::vector<unsigned int>& adjacentFaces (unsigned int i) const
  {
    assert (this->isFreeVertex (i) == false);
    return this->vertexData[i].adjacentFaces;
  }

  void forEachVertex (const std::function<void(unsigned int)>& f)
  {
    for (unsigned int i = 0; i < this->vertexData.size (); i++)
    {
      if (this->isFreeVertex (i) == false)
      {
        f (i);
      }
    }
  }

  void visitVertices (unsigned int i, const std::function<void(unsigned int)>& f)
  {
    assert (this->isFreeFace (i) == false);

    unsigned int i1, i2, i3;
    this->vertexIndices (i, i1, i2, i3);

    if (this->vertexVisited[i1] == 0)
    {
      this->vertexVisited[i1] = 1;
      f (i1);
    }
    if (this->vertexVisited[i2] == 0)
    {
      this->vertexVisited[i2] = 1;
      f (i2);
    }
    if (this->vertexVisited[i3] == 0)
    {
      this->vertexVisited[i3] = 1;
      f (i3);
    }
    this->faceVisited[i] = 1;
  }

  void unvisitVertices ()
  {
    std::memset (this->vertexVisited.data (), 0, this->vertexVisited.size ());
  }

  void unvisitFaces ()
  {
    std::memset (this->faceVisited.data (), 0, this->faceVisited.size ());
  }

  void forEachVertex (const DynamicFaces& faces, const std::function<void(unsigned int)>& f)
  {
    this->unvisitVertices ();

    for (unsigned int i : faces)
    {
      this->visitVertices (i, f);
    }
  }

  void forEachVertexExt (const DynamicFaces& faces, const std::function<void(unsigned int)>& f)
  {
    this->unvisitVertices ();
    this->unvisitFaces ();

    for (unsigned int i : faces)
    {
      this->visitVertices (i, [this, &f](unsigned int j) {
        f (j);

        for (unsigned int a : this->vertexData[j].adjacentFaces)
        {
          if (this->faceVisited[a] == 0)
          {
            this->visitVertices (a, f);
          }
        }
      });
    }
  }

  void forEachFace (const std::function<void(unsigned int)>& f)
  {
    for (unsigned int i = 0; i < this->faceData.size (); i++)
    {
      if (this->isFreeFace (i) == false)
      {
        f (i);
      }
    }
  }

  void forEachFaceExt (const DynamicFaces& faces, const std::function<void(unsigned int)>& f)
  {
    this->unvisitVertices ();
    this->unvisitFaces ();

    for (unsigned int i : faces)
    {
      if (this->faceVisited[i] == 0)
      {
        f (i);
        this->faceVisited[i] = 1;
      }
      this->visitVertices (i, [this, &f](unsigned int j) {
        for (unsigned int a : this->vertexData[j].adjacentFaces)
        {
          if (this->faceVisited[a] == 0)
          {
            f (a);
            this->faceVisited[a] = 1;
          }
        }
      });
    }
  }

  void average (const DynamicFaces& faces, glm::vec3& position, glm::vec3& normal) const
  {
    assert (faces.numElements () > 0);

    position = glm::vec3 (0.0f);
    normal = glm::vec3 (0.0f);

    for (unsigned int f : faces)
    {
      unsigned int i1, i2, i3;
      this->vertexIndices (f, i1, i2, i3);

      position += this->mesh.vertex (i1);
      position += this->mesh.vertex (i2);
      position += this->mesh.vertex (i3);
      normal += glm::cross (this->mesh.vertex (i2) - this->mesh.vertex (i1),
                            this->mesh.vertex (i3) - this->mesh.vertex (i1));
    }
    position /= float(faces.numElements () * 3);
    normal = glm::normalize (normal);
  }

  glm::vec3 averagePosition (const DynamicFaces& faces) const
  {
    assert (faces.numElements () > 0);

    glm::vec3 position = glm::vec3 (0.0f);

    for (unsigned int f : faces)
    {
      unsigned int i1, i2, i3;
      this->vertexIndices (f, i1, i2, i3);

      position += this->mesh.vertex (i1);
      position += this->mesh.vertex (i2);
      position += this->mesh.vertex (i3);
    }
    return position / float(faces.numElements () * 3);
  }

  glm::vec3 averagePosition (unsigned int i) const
  {
    assert (this->isFreeVertex (i) == false);
    assert (this->vertexData[i].adjacentFaces.size () > 0);

    glm::vec3 position = glm::vec3 (0.0f);

    for (unsigned int a : this->vertexData[i].adjacentFaces)
    {
      unsigned int i1, i2, i3;
      this->vertexIndices (a, i1, i2, i3);

      if (i != i1)
      {
        position += this->mesh.vertex (i1);
      }
      if (i != i2)
      {
        position += this->mesh.vertex (i2);
      }
      if (i != i3)
      {
        position += this->mesh.vertex (i3);
      }
    }
    return position / float(this->vertexData[i].adjacentFaces.size () * 2);
  }

  glm::vec3 averageNormal (const DynamicFaces& faces) const
  {
    assert (faces.numElements () > 0);

    glm::vec3 normal = glm::vec3 (0.0f);

    for (unsigned int f : faces)
    {
      unsigned int i1, i2, i3;
      this->vertexIndices (f, i1, i2, i3);

      normal += glm::cross (this->mesh.vertex (i2) - this->mesh.vertex (i1),
                            this->mesh.vertex (i3) - this->mesh.vertex (i1));
    }
    return glm::normalize (normal);
  }

  glm::vec3 averageNormal (unsigned int i)
  {
    assert (this->isFreeVertex (i) == false);
    assert (this->vertexData[i].adjacentFaces.size () > 0);

    glm::vec3 normal = glm::vec3 (0.0f);

    for (unsigned int f : this->vertexData[i].adjacentFaces)
    {
      unsigned int i1, i2, i3;
      this->vertexIndices (f, i1, i2, i3);

      if (i != i1)
      {
        normal += glm::cross (this->mesh.vertex (i2) - this->mesh.vertex (i1),
                              this->mesh.vertex (i3) - this->mesh.vertex (i1));
      }
      else if (i != i2)
      {
        normal += glm::cross (this->mesh.vertex (i3) - this->mesh.vertex (i2),
                              this->mesh.vertex (i1) - this->mesh.vertex (i2));
      }
      else if (i != i3)
      {
        normal += glm::cross (this->mesh.vertex (i1) - this->mesh.vertex (i3),
                              this->mesh.vertex (i2) - this->mesh.vertex (i3));
      }
      else
      {
        DILAY_IMPOSSIBLE
      }
    }
    return glm::normalize (normal);
  }

  float averageEdgeLengthSqr (const DynamicFaces& faces) const
  {
    assert (faces.numElements () > 0);

    float length = 0.0f;
    for (unsigned int i : faces)
    {
      length += this->averageEdgeLengthSqr (i);
    }
    return length / float(faces.numElements ());
  }

  float averageEdgeLengthSqr (unsigned int i) const
  {
    assert (this->isFreeFace (i) == false);

    unsigned int i1, i2, i3;
    this->vertexIndices (i, i1, i2, i3);

    const glm::vec3& v1 = this->mesh.vertex (i1);
    const glm::vec3& v2 = this->mesh.vertex (i2);
    const glm::vec3& v3 = this->mesh.vertex (i3);

    return (glm::distance2 (v1, v2) + glm::distance2 (v1, v3) + glm::distance2 (v2, v3)) / 3.0f;
  }

  unsigned int addVertex (const glm::vec3& vertex, const glm::vec3& normal)
  {
    assert (this->vertexData.size () == this->mesh.numVertices ());
    assert (this->vertexVisited.size () == this->mesh.numVertices ());

    if (this->freeVertexIndices.empty ())
    {
      this->vertexData.emplace_back ();
      this->vertexData.back ().isFree = false;
      this->vertexVisited.push_back (0);
      return this->mesh.addVertex (vertex, normal);
    }
    else
    {
      const unsigned int index = this->freeVertexIndices.back ();
      this->mesh.vertex (index, vertex);
      this->mesh.normal (index, normal);
      this->vertexData[index].reset ();
      this->vertexData[index].isFree = false;
      this->vertexVisited[index] = 0;
      this->freeVertexIndices.pop_back ();
      return index;
    }
  }

  unsigned int addFace (unsigned int i1, unsigned int i2, unsigned int i3)
  {
    assert (i1 < this->mesh.numVertices ());
    assert (i2 < this->mesh.numVertices ());
    assert (i3 < this->mesh.numVertices ());
    assert (3 * this->faceData.size () == this->mesh.numIndices ());
    assert (this->faceData.size () == this->faceVisited.size ());

    unsigned int index = Util::invalidIndex ();

    if (this->freeFaceIndices.empty ())
    {
      index = this->numFaces ();
      this->faceData.emplace_back ();
      this->faceVisited.push_back (0);

      this->mesh.addIndex (i1);
      this->mesh.addIndex (i2);
      this->mesh.addIndex (i3);
    }
    else
    {
      index = this->freeFaceIndices.back ();
      this->faceData[index].reset ();
      this->faceVisited[index] = 0;
      this->freeFaceIndices.pop_back ();

      this->mesh.index ((3 * index) + 0, i1);
      this->mesh.index ((3 * index) + 1, i2);
      this->mesh.index ((3 * index) + 2, i3);
    }
    this->faceData[index].isFree = false;

    this->vertexData[i1].addAdjacentFace (index);
    this->vertexData[i2].addAdjacentFace (index);
    this->vertexData[i3].addAdjacentFace (index);

    this->addFaceToOctree (index);

    return index;
  }

  void addFaceToOctree (unsigned int i)
  {
    const PrimTriangle tri = this->face (i);

    this->octree.addElement (i, tri.center (), tri.maxDimExtent ());
  }

  void deleteVertex (unsigned int i)
  {
    assert (i < this->vertexData.size ());
    assert (i < this->vertexVisited.size ());

    std::vector<unsigned int> adjacentFaces = this->vertexData[i].adjacentFaces;
    for (unsigned int f : adjacentFaces)
    {
      this->deleteFace (f);
    }
    this->vertexData[i].reset ();
    this->vertexVisited[i] = 0;
    this->freeVertexIndices.push_back (i);
  }

  void deleteFace (unsigned int i)
  {
    assert (i < this->faceData.size ());
    assert (i < this->faceVisited.size ());

    this->vertexData[this->mesh.index ((3 * i) + 0)].deleteAdjacentFace (i);
    this->vertexData[this->mesh.index ((3 * i) + 1)].deleteAdjacentFace (i);
    this->vertexData[this->mesh.index ((3 * i) + 2)].deleteAdjacentFace (i);

    this->faceData[i].reset ();
    this->faceVisited[i] = 0;
    this->freeFaceIndices.push_back (i);
    this->octree.deleteElement (i);
  }

  void vertexNormal (unsigned int i, const glm::vec3& n)
  {
    assert (this->isFreeVertex (i) == false);
    assert (this->mesh.numVertices () == this->vertexData.size ());

    this->mesh.normal (i, n);
  }

  void setVertexNormal (unsigned int i)
  {
    const glm::vec3 avg = this->averageNormal (i);

    if (Util::isNaN (avg))
    {
      this->mesh.normal (i, glm::vec3 (0.0f));
    }
    else
    {
      this->mesh.normal (i, this->averageNormal (i));
    }
  }

  void setAllNormals ()
  {
    this->forEachVertex ([this](unsigned int i) { this->setVertexNormal (i); });
  }

  void setupOctreeRoot (const Mesh& mesh)
  {
    assert (this->octree.hasRoot () == false);

    glm::vec3 minVertex, maxVertex;
    mesh.minMax (minVertex, maxVertex);

    const glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
    const glm::vec3 delta = maxVertex - minVertex;
    const float     width = glm::max (glm::max (delta.x, delta.y), delta.z);

    this->octree.setupRoot (center, width);
  }

  void reset ()
  {
    this->mesh.reset ();
    this->vertexData.clear ();
    this->vertexVisited.clear ();
    this->freeVertexIndices.clear ();
    this->faceData.clear ();
    this->faceVisited.clear ();
    this->freeFaceIndices.clear ();
    this->octree.reset ();
  }

  void fromMesh (const Mesh& mesh)
  {
    this->reset ();
    this->setupOctreeRoot (mesh);
    this->mesh.reserveVertices (mesh.numVertices ());

    for (unsigned int i = 0; i < mesh.numVertices (); i++)
    {
      this->addVertex (mesh.vertex (i), mesh.normal (i));
    }

    assert (mesh.numIndices () % 3 == 0);
    this->mesh.reserveIndices (mesh.numIndices ());

    for (unsigned int i = 0; i < mesh.numIndices (); i += 3)
    {
      this->addFace (mesh.index (i), mesh.index (i + 1), mesh.index (i + 2));
    }
    this->setAllNormals ();
    this->mesh.bufferData ();
  }

  void realignFace (unsigned int i)
  {
    assert (this->isFreeFace (i) == false);

    const PrimTriangle tri = this->face (i);

    this->octree.realignElement (i, tri.center (), tri.maxDimExtent ());
  }

  void realignAllFaces ()
  {
    this->forEachFace ([this](unsigned int i) { this->realignFace (i); });
  }

  void sanitize ()
  {
    this->octree.deleteEmptyChildren ();
    this->octree.shrinkRoot ();
  }

  void prune ()
  {
    if (this->isPruned () == false)
    {
      std::vector<unsigned int> vertexIndexMap;
      std::vector<unsigned int> faceIndexMap;

      Util::prune<VertexData> (this->vertexData, [](const VertexData& d) { return d.isFree; },
                               &vertexIndexMap);
      Util::prune<FaceData> (this->faceData, [](const FaceData& d) { return d.isFree; },
                             &faceIndexMap);

      const unsigned int newNumVertices = this->vertexData.size ();
      const unsigned int newNumFaces = this->faceData.size ();

      for (VertexData& d : this->vertexData)
      {
        for (unsigned int& f : d.adjacentFaces)
        {
          assert (faceIndexMap[f] != Util::invalidIndex ());

          f = faceIndexMap[f];
        }
      }

      for (unsigned int i = 0; i < vertexIndexMap.size (); i++)
      {
        const unsigned int newV = vertexIndexMap[i];
        if (newV != Util::invalidIndex ())
        {
          this->mesh.vertex (newV, this->mesh.vertex (i));
        }
        else
        {
          assert (std::find (this->freeVertexIndices.begin (), this->freeVertexIndices.end (), i) !=
                  this->freeVertexIndices.end ());
        }
      }
      this->freeVertexIndices.clear ();
      this->mesh.shrinkVertices (newNumVertices);
      this->vertexVisited.resize (newNumVertices);
      assert (this->numVertices () == newNumVertices);

      for (unsigned int i = 0; i < faceIndexMap.size (); i++)
      {
        const unsigned int newF = faceIndexMap[i];
        if (newF != Util::invalidIndex ())
        {
          const unsigned int oldI1 = this->mesh.index ((3 * i) + 0);
          const unsigned int oldI2 = this->mesh.index ((3 * i) + 1);
          const unsigned int oldI3 = this->mesh.index ((3 * i) + 2);

          assert (vertexIndexMap[oldI1] != Util::invalidIndex ());
          assert (vertexIndexMap[oldI2] != Util::invalidIndex ());
          assert (vertexIndexMap[oldI3] != Util::invalidIndex ());

          this->mesh.index ((3 * newF) + 0, vertexIndexMap[oldI1]);
          this->mesh.index ((3 * newF) + 1, vertexIndexMap[oldI2]);
          this->mesh.index ((3 * newF) + 2, vertexIndexMap[oldI3]);
        }
        else
        {
          assert (std::find (this->freeFaceIndices.begin (), this->freeFaceIndices.end (), i) !=
                  this->freeFaceIndices.end ());
        }
      }
      this->freeFaceIndices.clear ();
      this->mesh.shrinkIndices (3 * newNumFaces);
      this->faceVisited.resize (newNumFaces);
      assert (this->numFaces () == newNumFaces);

      this->octree.updateIndices (faceIndexMap);
    }
  }

  bool checkConsistency ()
  {
    this->prune ();
    this->bufferData ();

    if (MeshUtil::checkConsistency (this->mesh))
    {
      for (unsigned int i = 0; i < this->vertexData.size (); i++)
      {
        if (this->vertexData[i].isFree == false)
        {
          if (this->vertexData[i].adjacentFaces.empty ())
          {
            DILAY_WARN ("vertex %u is not free but has no adjacent faces", i);
            return false;
          }
        }
      }
      return true;
    }
    else
    {
      return false;
    }
  }

  void mirror (const PrimPlane& plane)
  {
    assert (this->checkConsistency ());

    ToolSculptAction::deleteDegeneratedFaces (*this->self);
    this->prune ();
    this->fromMesh (MeshUtil::mirror (this->mesh, plane));

    assert (this->checkConsistency ());
  }

  void bufferData ()
  {
    const auto findNonFreeFaceIndex = [this]() -> unsigned int {
      assert (this->numFaces () > 0);

      for (unsigned int i = 0; i < this->faceData.size (); i++)
      {
        if (this->isFreeFace (i) == false)
        {
          return i;
        }
      }
      DILAY_IMPOSSIBLE
    };

    if (this->numFaces () > 0 && this->freeFaceIndices.empty () == false)
    {
      const unsigned int nonFree = findNonFreeFaceIndex ();

      for (unsigned int i : this->freeFaceIndices)
      {
        this->mesh.index ((3 * i) + 0, this->mesh.index ((3 * nonFree) + 0));
        this->mesh.index ((3 * i) + 1, this->mesh.index ((3 * nonFree) + 1));
        this->mesh.index ((3 * i) + 2, this->mesh.index ((3 * nonFree) + 2));
      }
    }
    this->mesh.bufferData ();
  }

  void render (Camera& camera)
  {
    this->mesh.render (camera);
#ifdef DILAY_RENDER_OCTREE
    this->octree.render (camera);
#endif
  }

  bool intersects (const PrimRay& ray, Intersection& intersection) const
  {
    this->octree.intersects (ray, [this, &ray, &intersection](unsigned int i) {
      const PrimTriangle tri = this->face (i);
      float              t;

      if (IntersectionUtil::intersects (ray, tri, &t))
      {
        intersection.update (t, ray.pointAt (t), tri.normal ());
      }
    });
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, DynamicMeshIntersection& intersection)
  {
    this->octree.intersects (ray, [this, &ray, &intersection](unsigned int i) {
      const PrimTriangle tri = this->face (i);
      float              t;

      if (IntersectionUtil::intersects (ray, tri, &t))
      {
        intersection.update (t, ray.pointAt (t), tri.normal (), i, *this->self);
      }
    });
    return intersection.isIntersection ();
  }

  template <typename T, typename... Ts>
  bool intersectsT (const T& t, DynamicFaces& faces, const Ts&... args) const
  {
    this->octree.intersects (t, [this, &t, &faces, &args...](unsigned int i) {
      if (IntersectionUtil::intersects (t, this->face (i), args...))
      {
        faces.insert (i);
      }
    });
    faces.commit ();
    return faces.isEmpty () == false;
  }

  template <typename T, typename... Ts>
  bool containsOrIntersectsT (const T& t, DynamicFaces& faces, const Ts&... args) const
  {
    this->octree.intersects (t, [this, &t, &faces, &args...](bool contains, unsigned int i) {
      if (contains || IntersectionUtil::intersects (t, this->face (i), args...))
      {
        faces.insert (i);
        faces.commit ();
      }
    });
    return faces.isEmpty () == false;
  }

  bool intersects (const PrimRay& ray, DynamicFaces& faces) const
  {
    return this->intersectsT<PrimRay> (ray, faces, nullptr);
  }

  bool intersects (const PrimPlane& plane, DynamicFaces& faces) const
  {
    return this->intersectsT<PrimPlane> (plane, faces);
  }

  bool intersects (const PrimSphere& sphere, DynamicFaces& faces) const
  {
    return this->containsOrIntersectsT<PrimSphere> (sphere, faces);
  }

  bool intersects (const PrimAABox& box, DynamicFaces& faces) const
  {
    return this->containsOrIntersectsT<PrimAABox> (box, faces);
  }

  void normalize ()
  {
    this->mesh.normalize ();
    this->octree.reset ();
    this->setupOctreeRoot (this->mesh);

    this->forEachFace ([this](unsigned int i) { this->addFaceToOctree (i); });
  }

  void printStatistics () const
  {
    this->octree.printStatistics ();
  }

  void runFromConfig (const Config& config)
  {
    this->mesh.color (config.get<Color> ("editor/mesh/color/normal"));
    this->mesh.wireframeColor (config.get<Color> ("editor/mesh/color/wireframe"));
  }
};

DELEGATE1_BIG4_COPY_SELF (DynamicMesh, const Mesh&)
DELEGATE_CONST (unsigned int, DynamicMesh, numVertices)
DELEGATE_CONST (unsigned int, DynamicMesh, numFaces)
DELEGATE_CONST (bool, DynamicMesh, isEmpty)
DELEGATE1_CONST (bool, DynamicMesh, isFreeVertex, unsigned int)
DELEGATE1_CONST (bool, DynamicMesh, isFreeFace, unsigned int)
DELEGATE1_MEMBER_CONST (const glm::vec3&, DynamicMesh, vertex, mesh, unsigned int)
DELEGATE1_CONST (unsigned int, DynamicMesh, valence, unsigned int)
DELEGATE4_CONST (void, DynamicMesh, vertexIndices, unsigned int, unsigned int&, unsigned int&,
                 unsigned int&)
DELEGATE1_CONST (PrimTriangle, DynamicMesh, face, unsigned int)
DELEGATE1_CONST (const glm::vec3&, DynamicMesh, vertexNormal, unsigned int)
DELEGATE1_CONST (const std::vector<unsigned int>&, DynamicMesh, adjacentFaces, unsigned int)
GETTER_CONST (const Mesh&, DynamicMesh, mesh)
DELEGATE1 (void, DynamicMesh, forEachVertex, const std::function<void(unsigned int)>&)
DELEGATE2 (void, DynamicMesh, forEachVertex, const DynamicFaces&,
           const std::function<void(unsigned int)>&)
DELEGATE2 (void, DynamicMesh, forEachVertexExt, const DynamicFaces&,
           const std::function<void(unsigned int)>&)
DELEGATE1 (void, DynamicMesh, forEachFace, const std::function<void(unsigned int)>&)
DELEGATE2 (void, DynamicMesh, forEachFaceExt, const DynamicFaces&,
           const std::function<void(unsigned int)>&)
DELEGATE3_CONST (void, DynamicMesh, average, const DynamicFaces&, glm::vec3&, glm::vec3&)
DELEGATE1_CONST (glm::vec3, DynamicMesh, averagePosition, const DynamicFaces&)
DELEGATE1_CONST (glm::vec3, DynamicMesh, averagePosition, unsigned int)
DELEGATE1_CONST (glm::vec3, DynamicMesh, averageNormal, const DynamicFaces&)
DELEGATE1_CONST (glm::vec3, DynamicMesh, averageNormal, unsigned int)
DELEGATE1_CONST (float, DynamicMesh, averageEdgeLengthSqr, const DynamicFaces&)
DELEGATE1_CONST (float, DynamicMesh, averageEdgeLengthSqr, unsigned int)
DELEGATE2 (unsigned int, DynamicMesh, addVertex, const glm::vec3&, const glm::vec3&)
DELEGATE3 (unsigned int, DynamicMesh, addFace, unsigned int, unsigned int, unsigned int)
DELEGATE1 (void, DynamicMesh, deleteVertex, unsigned int)
DELEGATE1 (void, DynamicMesh, deleteFace, unsigned int)
DELEGATE2_MEMBER (void, DynamicMesh, vertex, mesh, unsigned int, const glm::vec3&)
DELEGATE2 (void, DynamicMesh, vertexNormal, unsigned int, const glm::vec3&)
DELEGATE1 (void, DynamicMesh, setVertexNormal, unsigned int)
DELEGATE (void, DynamicMesh, setAllNormals)
DELEGATE (void, DynamicMesh, reset)
DELEGATE1 (void, DynamicMesh, fromMesh, const Mesh&)
DELEGATE1 (void, DynamicMesh, realignFace, unsigned int)
DELEGATE (void, DynamicMesh, realignAllFaces)
DELEGATE (void, DynamicMesh, sanitize)
DELEGATE (void, DynamicMesh, prune)
DELEGATE (bool, DynamicMesh, checkConsistency)
DELEGATE1 (void, DynamicMesh, mirror, const PrimPlane&)
DELEGATE (void, DynamicMesh, bufferData)
DELEGATE1 (void, DynamicMesh, render, Camera&)
DELEGATE_MEMBER_CONST (const RenderMode&, DynamicMesh, renderMode, mesh)
DELEGATE_MEMBER (RenderMode&, DynamicMesh, renderMode, mesh)

DELEGATE2_CONST (bool, DynamicMesh, intersects, const PrimRay&, Intersection&)
DELEGATE2 (bool, DynamicMesh, intersects, const PrimRay&, DynamicMeshIntersection&)
DELEGATE2_CONST (bool, DynamicMesh, intersects, const PrimRay&, DynamicFaces&)
DELEGATE2_CONST (bool, DynamicMesh, intersects, const PrimPlane&, DynamicFaces&)
DELEGATE2_CONST (bool, DynamicMesh, intersects, const PrimSphere&, DynamicFaces&)
DELEGATE2_CONST (bool, DynamicMesh, intersects, const PrimAABox&, DynamicFaces&)

DELEGATE (void, DynamicMesh, normalize)
DELEGATE1_MEMBER (void, DynamicMesh, scale, mesh, const glm::vec3&)
DELEGATE1_MEMBER (void, DynamicMesh, scaling, mesh, const glm::vec3&)
DELEGATE_MEMBER_CONST (glm::vec3, DynamicMesh, scaling, mesh)
DELEGATE1_MEMBER (void, DynamicMesh, translate, mesh, const glm::vec3&)
DELEGATE1_MEMBER (void, DynamicMesh, position, mesh, const glm::vec3&)
DELEGATE_MEMBER_CONST (glm::vec3, DynamicMesh, position, mesh)
DELEGATE1_MEMBER (void, DynamicMesh, rotationMatrix, mesh, const glm::mat4x4&)
DELEGATE_MEMBER_CONST (const glm::mat4x4&, DynamicMesh, rotationMatrix, mesh)
DELEGATE1_MEMBER (void, DynamicMesh, rotationX, mesh, float)
DELEGATE1_MEMBER (void, DynamicMesh, rotationY, mesh, float)
DELEGATE1_MEMBER (void, DynamicMesh, rotationZ, mesh, float)
DELEGATE_MEMBER_CONST (glm::vec3, DynamicMesh, center, mesh)
DELEGATE_MEMBER_CONST (const Color&, DynamicMesh, color, mesh)
DELEGATE1_MEMBER (void, DynamicMesh, color, mesh, const Color&)
DELEGATE_MEMBER_CONST (const Color&, DynamicMesh, wireframeColor, mesh)
DELEGATE1_MEMBER (void, DynamicMesh, wireframeColor, mesh, const Color&)

DELEGATE_CONST (void, DynamicMesh, printStatistics)
DELEGATE1 (void, DynamicMesh, runFromConfig, const Config&)
