#include <list>
#include "sphere-mesh.hpp"
#include "sphere-joint.hpp"
#include "macro.hpp"
#include "id.hpp"
#include "mesh.hpp"


struct SphereMesh::Impl {
  const IdObject          id;
  std::list <SphereJoint> joints;
  Mesh                    jointMesh;

  void bufferData  () { 
    this->jointMesh.bufferData   (); 
  }

  void render () {
    this->jointMesh.renderBegin ();

    for (SphereJoint& j : this->joints) {
      
      this->jointMesh.setPosition    (glm::vec3 (j.position ()));
      this->jointMesh.setScaling     (glm::vec3 (j.radius   ()));
      this->jointMesh.fixModelMatrix ();
    }
    this->jointMesh.renderEnd ();
  }

  void addJoint (const SphereJoint& j) {
    this->joints.push_back (j);
  }
};

DELEGATE_BIG4 (SphereMesh)
ID            (SphereMesh)

DELEGATE      (void, SphereMesh, bufferData)
DELEGATE      (void, SphereMesh, render)
DELEGATE1     (void, SphereMesh, addJoint, const SphereJoint&)
