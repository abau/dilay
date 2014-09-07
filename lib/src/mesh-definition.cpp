#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "mesh-definition.hpp"
#include "util.hpp"

struct MeshDefinition::Impl {
  typedef std::tuple <unsigned int, unsigned int, unsigned int> Indices3;

  std::vector <glm::vec3> vertices;
  std::vector <Indices3>  indices3;

  unsigned int addVertex (const glm::vec3& v) {
    this->vertices.push_back (v);
    return this->numVertices () - 1;
  }

  unsigned int addFace (unsigned int i1, unsigned int i2, unsigned int i3) {
    this->indices3.push_back (Indices3 (i1,i2,i3));
    return this->numFace3 () - 1;
  }

  unsigned int numVertices () const {
    return this->vertices.size ();
  }

  unsigned int numFace3 () const {
    return this->indices3.size ();
  }

  const glm::vec3& vertex (unsigned int i) const {
    assert (i < this->numVertices ());
    return this->vertices[i];
  }

  void face (unsigned int i, unsigned int& i1, unsigned int& i2, unsigned int& i3) const {
    assert (i < this->numFace3 ());
    i1 = std::get <0> (this->indices3[i]);
    i2 = std::get <1> (this->indices3[i]);
    i3 = std::get <2> (this->indices3[i]);
  }

  static MeshDefinition cube () {
    MeshDefinition m;
    const float d = 0.5f;

    m.addVertex ( glm::vec3 (-d, -d, -d) );
    m.addVertex ( glm::vec3 (-d, -d, +d) );
    m.addVertex ( glm::vec3 (-d, +d, -d) );
    m.addVertex ( glm::vec3 (-d, +d, +d) );
    m.addVertex ( glm::vec3 (+d, -d, -d) );
    m.addVertex ( glm::vec3 (+d, -d, +d) );
    m.addVertex ( glm::vec3 (+d, +d, -d) );
    m.addVertex ( glm::vec3 (+d, +d, +d) );

    m.addFace (0, 1, 2);
    m.addFace (3, 2, 1);

    m.addFace (1, 5, 3);
    m.addFace (7, 3, 5);

    m.addFace (5, 4, 7);
    m.addFace (6, 7, 4);

    m.addFace (4, 0, 6);
    m.addFace (2, 6, 0);

    m.addFace (3, 7, 2);
    m.addFace (6, 2, 7);

    m.addFace (0, 4, 1);
    m.addFace (5, 1, 4);
    return m;
  }

  static MeshDefinition sphere (unsigned int rings, unsigned int sectors) {
    assert (rings > 1 && sectors > 2);
    MeshDefinition m;

    const float radius     = 1.0f;
    const float ringStep   =        M_PI / float (rings);
    const float sectorStep = 2.0f * M_PI / float (sectors);
          float phi        = ringStep;
          float theta      = 0.0f;

    // inner rings vertices
    for (unsigned int r = 0; r < rings - 1; r++) {
      for (unsigned int s = 0; s < sectors; s++) {
        const float x = radius * sin (theta) * sin (phi);
        const float y = radius * cos (phi);
        const float z = radius * cos (theta) * sin (phi);

        m.addVertex (glm::vec3 (x,y,z));

        theta += sectorStep;
      }
      phi += ringStep;
    }

    // caps vertices
    const unsigned int topCapIndex = m.addVertex (glm::vec3 (0.0f, radius, 0.0f));
    const unsigned int botCapIndex = m.addVertex (glm::vec3 (0.0f,-radius, 0.0f));

    // inner rings indices
    for (unsigned int r = 0; r < rings - 2; r++) {
      for (unsigned int s = 0; s < sectors; s++) {
        m.addFace ( (sectors * r) + s
                  , (sectors * (r+1)) + s
                  , (sectors * r) + ((s+1) % sectors));

        m.addFace ( (sectors * (r+1)) + ((s+1) % sectors)
                  , (sectors * r) + ((s+1) % sectors)
                  , (sectors * (r+1)) + s);
      }
    }

    // caps indices
    for (unsigned int s = 0; s < sectors; s++) {
      m.addFace (topCapIndex, s, (s+1) % sectors);

      m.addFace ( botCapIndex
                , (sectors * (rings-2)) + ((s+1) % sectors)
                , (sectors * (rings-2)) + s);
    }
    return m;
  }

  static MeshDefinition icosphere (unsigned int numSubdivisions) {
    MeshDefinition m;
    typedef unsigned long                          Key;
    typedef std::unordered_map <Key, unsigned int> VertexCache;

    VertexCache vertexCache;

    // adds new vertex to ico-sphere
    auto addIcoVertex = [&m] (const glm::vec3& v) -> unsigned int {
      return m.addVertex (glm::normalize (v));
    };

    // computes key for vertex cache
    auto getKey = [] (unsigned int i1, unsigned i2) -> Key {
      return (Key (i1) << 8 * sizeof (int)) + Key (i2);
    };

    // looks up vertex in cache or computes a new one
    auto lookupVertex = [&vertexCache,&addIcoVertex,&getKey,&m] 
      (unsigned int i1, unsigned int i2) -> unsigned int 
      {
        const unsigned int lowerI = i1 < i2 ? i1 : i2;
        const unsigned int upperI = i1 < i2 ? i2 : i1;
        const Key          key    = getKey (lowerI, upperI);

        VertexCache::iterator it = vertexCache.find (key);
        if (it == vertexCache.end ()) {
          const unsigned int n = addIcoVertex (Util::between ( m.vertex (lowerI)
                                                             , m.vertex (upperI)));
          vertexCache.emplace (key, n);
          return n;
        }
        else {
          return it->second;
        }
      };

    // subdivides a face of the ico-sphere
    std::function <void (unsigned int,unsigned int,unsigned int,unsigned int)> subdivide =
      [&m,&subdivide,&lookupVertex] 
      (unsigned int s,unsigned int i1, unsigned int i2, unsigned int i3) -> void 
      {
        if (s == 0) {
          m.addFace (i1, i2, i3); 
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

    return m;
  }

  static MeshDefinition cone (unsigned int numBaseVertices) {
    assert (numBaseVertices >= 3);

    MeshDefinition m;
    const float    c = 2.0f * glm::pi <float> () / float (numBaseVertices);

    for (unsigned int i = 0; i < numBaseVertices; i++) {
      m.addVertex (glm::vec3 ( glm::sin <float> (float (i) * c)
                             , -0.5f
                             , glm::cos <float> (float (i) * c)));
    }
    m.addVertex (glm::vec3 (0.0f, -0.5f, 0.0f));
    m.addVertex (glm::vec3 (0.0f,  0.5f, 0.0f));

    for (unsigned int i = 0; i < numBaseVertices - 1; i++) {
      m.addFace (i, i + 1, numBaseVertices + 1);
      m.addFace (i + 1, i, numBaseVertices);
    }
    m.addFace (numBaseVertices - 1, 0, numBaseVertices + 1);
    m.addFace (0, numBaseVertices - 1, numBaseVertices);
    return m;
  }

  static MeshDefinition cylinder (unsigned int numVertices) {
    assert (numVertices >= 3);

    MeshDefinition m;
    const float    c = 2.0f * glm::pi <float> () / float (numVertices);

    for (unsigned int i = 0; i < numVertices; i++) {
      m.addVertex (glm::vec3 ( glm::sin <float> (float (i) * c)
                             , -0.5f
                             , glm::cos <float> (float (i) * c)));
    }
    for (unsigned int i = 0; i < numVertices; i++) {
      m.addVertex (glm::vec3 ( glm::sin <float> (float (i) * c)
                             , 0.5f
                             , glm::cos <float> (float (i) * c)));
    }
    m.addVertex (glm::vec3 (0.0f, -0.5f, 0.0f));
    m.addVertex (glm::vec3 (0.0f,  0.5f, 0.0f));

    for (unsigned int i = 0; i < numVertices - 1; i++) {
      m.addFace (i, i + 1, i + numVertices); 
      m.addFace (i + numVertices + 1, i + numVertices, i + 1);

      m.addFace (i + 1, i, 2 * numVertices);
      m.addFace (i + numVertices, i + numVertices + 1, (2 * numVertices) + 1);
    }
    m.addFace (numVertices - 1, 0, (2 * numVertices) - 1);
    m.addFace (numVertices, (2 * numVertices) - 1, 0);

    m.addFace (0, numVertices - 1, 2 * numVertices);
    m.addFace ((2 * numVertices) - 1, numVertices, (2 * numVertices) + 1);
    return m;
  }
};


DELEGATE_BIG6   (MeshDefinition)
DELEGATE1       (unsigned int, MeshDefinition, addVertex, const glm::vec3&)
DELEGATE3       (unsigned int, MeshDefinition, addFace, unsigned int, unsigned int, unsigned int)
DELEGATE_CONST  (unsigned int, MeshDefinition, numVertices)
DELEGATE_CONST  (unsigned int, MeshDefinition, numFace3)
DELEGATE1_CONST (const glm::vec3&, MeshDefinition, vertex, unsigned int)
DELEGATE4_CONST (void, MeshDefinition, face, unsigned int, unsigned int&, unsigned int&, unsigned int&)

DELEGATE_STATIC  (MeshDefinition, MeshDefinition, cube)
DELEGATE2_STATIC (MeshDefinition, MeshDefinition, sphere, unsigned int, unsigned int)
DELEGATE1_STATIC (MeshDefinition, MeshDefinition, icosphere, unsigned int)
DELEGATE1_STATIC (MeshDefinition, MeshDefinition, cone, unsigned int)
DELEGATE1_STATIC (MeshDefinition, MeshDefinition, cylinder, unsigned int)
