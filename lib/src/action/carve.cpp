#define GLM_FORCE_RADIANS
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/carve.hpp"
#include "id.hpp"
#include "action/unit/on.hpp"
#include "action/subdivide.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "carve-brush.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"
#include "adjacent-iterator.hpp"
#include "intersection.hpp"
#include "octree.hpp"

namespace {
  class VertexData {
    public: 
      VertexData (const glm::vec3& p, const glm::vec3& n)
        : position (p)
        , normal   (n)
        , carved   (false)
        , delta    (0.0f)
        {}

      const glm::vec3 position;
      const glm::vec3 normal;

      bool  wasCarved () const { return this->carved; }
      float getDelta  () const { return this->delta;  }

      void  setDelta (float d) { 
        this->delta = d;
        this->carved = true;
      }

    private:
      bool  carved;
      float delta;
  };
};

struct CarveCache::Impl {
  std::unordered_map <unsigned int,VertexData> vertexCache;
  Octree                                       faceCache;                   
  WingedMesh*                                  meshCache;

  Impl () : faceCache (true) {}

  VertexData& cacheVertex (const WingedMesh& mesh, const WingedVertex& vertex) {
    auto it = this->vertexCache.find (vertex.index ());

    if (it == this->vertexCache.end () || it->second.wasCarved () == false) {
      VertexData data (vertex.vector (mesh), vertex.interpolatedNormal (mesh));
      return this->vertexCache.emplace (vertex.index (), data).first->second;
    }
    else {
      return it->second;
    }
  }

  void cacheFace (const WingedFace& face) {
    if (this->faceCache.hasFace (face.id ()) == false) {
      PrimTriangle triangle ( this->cachedVertex (face.firstVertex  ()).position
                            , this->cachedVertex (face.secondVertex ()).position
                            , this->cachedVertex (face.thirdVertex  ()).position
                            );
      this->faceCache.insertFace (face, triangle);
    }
  }

  VertexData& cachedVertex (const WingedVertex& vertex) {
    auto it = this->vertexCache.find (vertex.index ());

    if (it == this->vertexCache.end ()) {
      assert (false);
    }
    else {
      return it->second;
    }
  }

  bool intersects (const PrimRay& ray, Intersection& intersection) {
    return this->faceCache.intersects (ray, intersection);
  }

  void reset () {
    this->vertexCache.clear ();
    this->faceCache  .reset ();
    this->meshCache = nullptr;
  }
};

DELEGATE_BIG3 (CarveCache)
DELEGATE2     (bool, CarveCache, intersects, const PrimRay&, Intersection&)
GETTER        (WingedMesh*, CarveCache, meshCache)
SETTER        (WingedMesh*, CarveCache, meshCache)
DELEGATE      (void, CarveCache, reset)

struct ActionCarve::Impl {
  ActionCarve*              self;
  ActionUnitOn <WingedMesh> actions;

  Impl (ActionCarve* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run (WingedMesh& mesh, const glm::vec3& position, const CarveBrush& brush, CarveCache& cache) { 
    std::vector <WingedFace*> faces;
    PrimSphere                sphere (position, brush.width ());

    mesh.intersects (sphere, faces);

    this->subdivideFaces   (mesh, sphere, faces, brush, *cache.impl);
    this->cacheFaces       (faces, *cache.impl);
    this->carveFaces       (mesh, position, faces, brush, *cache.impl);
    this->subdivideFaces   (mesh, sphere, faces, brush, *cache.impl);
    this->self->bufferData (mesh);
  }

  bool isSubdividable ( const WingedMesh& mesh, const WingedFace& face
                      , const CarveBrush& brush, CarveCache::Impl& cache) 
  {
    const glm::vec3& v1 = cache.cacheVertex (mesh, face.firstVertex  ()).position;
    const glm::vec3& v2 = cache.cacheVertex (mesh, face.secondVertex ()).position;
    const glm::vec3& v3 = cache.cacheVertex (mesh, face.thirdVertex  ()).position;

    const float maxEdgeLength = glm::max ( glm::distance2 (v1, v2)
                                         , glm::max ( glm::distance2 (v1, v3)
                                                    , glm::distance2 (v2, v3)));

    return maxEdgeLength > brush.detail () * brush.detail ();
  }

  void subdivideFaces ( WingedMesh& mesh, const PrimSphere& sphere
                      , std::vector <WingedFace*>& faces
                      , const CarveBrush& brush, CarveCache::Impl& cache)
  {
    std::unordered_set <Id> thisIteration;
    std::unordered_set <Id> nextIteration;
    std::unordered_set <Id> affectedFaces;

    auto checkNextIteration = [&] (const WingedFace& face) -> void {
      if ( nextIteration.count (face.id ()) == 0 
        && this->isSubdividable (mesh, face, brush, cache)
        && IntersectionUtil::intersects (sphere,mesh,face)) 
      {
        nextIteration.insert (face.id ());
      }
    };

    // initialize first iteration
    for (WingedFace* face : faces) {
      assert (face);
      thisIteration.insert (face->id ());
      affectedFaces.insert (face->id ());
    }

    // iterate
    while (thisIteration.size () > 0) {
      for (const Id& id : thisIteration) {
        WingedFace* f = mesh.face (id);
        if (f && this->isSubdividable (mesh, *f, brush, cache)) {
          std::vector <Id> tmpAffected;
          this->actions.add <ActionSubdivide> ().run (mesh, *f, &tmpAffected);

          for (const Id& id : tmpAffected) {
            WingedFace* affected = mesh.face (id);
            if (affected) {
              checkNextIteration   (*affected);
              affectedFaces.insert (id);
            }
          }
        }
      }
      thisIteration = nextIteration;
      nextIteration.clear ();
    }

    // collect affected faces
    faces.clear ();
    for (const Id& id : affectedFaces) {
      WingedFace* affected = mesh.face (id);
      if (affected) {
        faces.push_back (affected);
      }
    }
  }

  void cacheFaces (const std::vector <WingedFace*>& faces, CarveCache::Impl& cache) {
    for (WingedFace* face : faces) {
      assert (face);
      cache.cacheFace (*face);
    }
  }

  glm::vec3 carveVertex (const glm::vec3& poa, const CarveBrush& brush, VertexData& vd) const {
    const float delta = brush.y (glm::distance <float> (vd.position, poa));

    vd.setDelta (glm::max (vd.getDelta (), delta));
    return vd.position + (vd.normal * vd.getDelta ());
  }

  void carveFaces ( WingedMesh& mesh, const glm::vec3& poa, std::vector <WingedFace*>& faces
                  , const CarveBrush& brush, CarveCache::Impl& cache) 
  {
    // compute set of vertices
    std::set <WingedVertex*> vertices;
    for (const WingedFace* face : faces) {
      assert (face);
      vertices.insert (&face->firstVertex  ());
      vertices.insert (&face->secondVertex ());
      vertices.insert (&face->thirdVertex  ());
    }

    // write new positions
    for (WingedVertex* v : vertices) {
      const glm::vec3 newPos = this->carveVertex (poa, brush, cache.cachedVertex (*v));

      this->actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }

    // write normals
    for (WingedVertex* v : vertices) {
      this->actions.add <PAModifyWVertex> ().writeNormal (mesh,*v, v->interpolatedNormal (mesh));
    }

    // realign faces
    for (auto it = faces.begin (); it != faces.end (); ++it) {
      WingedFace* face = *it;
      assert (face);
      *it = &this->self->realignFace (mesh, *face);
    }
  }
};

DELEGATE_BIG3_SELF (ActionCarve)
DELEGATE4          (void, ActionCarve, run , WingedMesh&, const glm::vec3&, const CarveBrush&, CarveCache&)
DELEGATE1          (void, ActionCarve, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1          (void, ActionCarve, runRedoBeforePostProcessing, WingedMesh&)
