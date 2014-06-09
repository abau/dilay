#define GLM_FORCE_RADIANS
#include <vector>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "mesh.hpp"
#include "macro.hpp"
#include "opengl-util.hpp"
#include "renderer.hpp"
#include "render-mode.hpp"
#include "color.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "util.hpp"

struct Mesh::Impl {
  // cf. copy-constructor, reset
  glm::mat4x4                 scalingMatrix;
  glm::mat4x4                 rotationMatrix;
  glm::mat4x4                 translationMatrix;
  std::vector<   GLfloat   >  vertices;
  std::vector< unsigned int>  indices;
  std::vector<   GLfloat   >  normals;
  Color                       color;
  Color                       wireframeColor;

  GLuint                      arrayObjectId; 
  GLuint                      vertexBufferId;
  GLuint                      indexBufferId;
  GLuint                      normalBufferId;

  RenderMode                  renderMode;

  Impl () { 
    this->scalingMatrix       = glm::mat4x4 (1.0f);
    this->rotationMatrix      = glm::mat4x4 (1.0f);
    this->translationMatrix   = glm::mat4x4 (1.0f);
    this->arrayObjectId       = 0;
    this->vertexBufferId      = 0;
    this->indexBufferId       = 0;
    this->normalBufferId      = 0;
    this->renderMode          = RenderMode::Flat;

    this->color               = Config::get <Color> ("/editor/color/mesh");
    this->wireframeColor      = Config::get <Color> ("/editor/color/mesh-wireframe");
  }

  Impl (const Impl& source)
              : scalingMatrix       (source.scalingMatrix)
              , rotationMatrix      (source.rotationMatrix)
              , translationMatrix   (source.translationMatrix)
              , vertices            (source.vertices)
              , indices             (source.indices)
              , normals             (source.normals)
              , color               (source.color)
              , wireframeColor      (source.wireframeColor)
              , arrayObjectId       (0)
              , vertexBufferId      (0)
              , indexBufferId       (0)
              , normalBufferId      (0)
              , renderMode          (source.renderMode) 
              {}

  ~Impl () { this->reset (); }

  unsigned int numVertices () const { return this->vertices.size () / 3; }
  unsigned int numIndices  () const { return this->indices.size  (); }
  unsigned int numNormals  () const { return this->normals.size () / 3; }

  unsigned int sizeOfVertices () const { 
    return this->vertices.size () * sizeof (GLfloat);
  }

  unsigned int sizeOfIndices () const { 
    return this->indices.size () * sizeof (unsigned int);
  }

  unsigned int sizeOfNormals () const { 
    return this->normals.size () * sizeof (GLfloat);
  }

  glm::vec3 vertex (unsigned int i) const {
    return glm::vec3 ( this->vertices [(3 * i) + 0]
                     , this->vertices [(3 * i) + 1]
                     , this->vertices [(3 * i) + 2]
        );
  }

  glm::vec3 worldVertex (unsigned int i) const {
    return Util::transformPosition (this->modelMatrix (), this->vertex (i));
  }

  unsigned int index (unsigned int i) const { return this->indices [i]; }

  glm::vec3 normal (unsigned int i) const {
    return glm::vec3 ( this->normals [(3 * i) + 0]
                     , this->normals [(3 * i) + 1]
                     , this->normals [(3 * i) + 2]
        );
  }

  unsigned int addIndex (unsigned int i) { 
    this->indices.push_back (i); 
    return this->indices.size () - 1;
  }

  unsigned int addVertex (const glm::vec3& v) { 
    this->vertices.push_back (v.x);
    this->vertices.push_back (v.y);
    this->vertices.push_back (v.z);

    this->normals.push_back (0.0f);
    this->normals.push_back (0.0f);
    this->normals.push_back (0.0f);

    return this->numVertices () - 1;
  }

  void setIndex (unsigned int indexNumber, unsigned int index) {
    assert (indexNumber < this->indices.size ());
    this->indices[indexNumber] = index;
  }

  void setVertex (unsigned int i, const glm::vec3& v) {
    assert (i < this->numVertices ());
    this->vertices [(3*i) + 0] = v.x;
    this->vertices [(3*i) + 1] = v.y;
    this->vertices [(3*i) + 2] = v.z;
  }

  void setNormal (unsigned int i, const glm::vec3& n) {
    assert (i < this->numNormals ());
    this->normals [(3*i) + 0] = n.x;
    this->normals [(3*i) + 1] = n.y;
    this->normals [(3*i) + 2] = n.z;
  }

  void popVertex () {
    this->vertices.pop_back ();
    this->vertices.pop_back ();
    this->vertices.pop_back ();
    this->normals .pop_back ();
    this->normals .pop_back ();
    this->normals .pop_back ();
  }

  void popIndices (unsigned int i) {
    this->resizeIndices (this->indices.size () - i);
  }

  void allocateIndices (unsigned int i) { 
    this->resizeIndices (this->indices.size () + i);
  }

  void resizeIndices (unsigned int i) {
    this->indices.resize (i);
  }

  void bufferData () {
    if (glIsVertexArray (this->arrayObjectId) == GL_FALSE)
      glGenVertexArrays (1, &this->arrayObjectId);
    if (glIsBuffer (this->vertexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->vertexBufferId);
    if (glIsBuffer (this->indexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->indexBufferId);
    if (glIsBuffer (this->normalBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->normalBufferId);

    glBindVertexArray          (this->arrayObjectId);

    glBindBuffer               ( GL_ARRAY_BUFFER, this->vertexBufferId );
    glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfVertices ()
                               , &this->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray  ( OpenGLUtil :: PositionIndex);
    glVertexAttribPointer      ( OpenGLUtil :: PositionIndex
                               , 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer               ( GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId );
    glBufferData               ( GL_ELEMENT_ARRAY_BUFFER, this->sizeOfIndices ()
                               , &this->indices[0], GL_STATIC_DRAW);

    glBindBuffer               ( GL_ARRAY_BUFFER, this->normalBufferId );
    glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfNormals ()
                               , &this->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray  ( OpenGLUtil :: NormalIndex);
    glVertexAttribPointer      ( OpenGLUtil :: NormalIndex
                               , 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer               ( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer               ( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray          ( 0 );
  }

  glm::mat4x4 modelMatrix () const {
    return this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
  }

  void fixModelMatrix () {
    State::camera ().modelViewProjection (this->modelMatrix ());
  }

  void renderBegin () {
    Renderer :: setProgram (this->renderMode);
    this->fixModelMatrix   ();
    glBindVertexArray      (this->arrayObjectId);
    glBindBuffer           (GL_ARRAY_BUFFER, this->vertexBufferId);
    glBindBuffer           (GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);
  }

  void renderEnd () { 
    glBindBuffer      (GL_ARRAY_BUFFER, 0);
    glBindBuffer      (GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray (0); 
  }

  void render () {
    if (this->renderMode == RenderMode::Smooth || this->renderMode == RenderMode::Flat)
      return this->renderSolid ();
    else if (this->renderMode == RenderMode::Wireframe)
      return this->renderWireframe ();
    else
      assert (false);
  }

  void renderSolid () {
    this->renderBegin     ();
    Renderer :: setColor3 (this->color);
    glDrawElements        (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);
    this->renderEnd       ();
  }

  void renderWireframe () {
    this->renderBegin  ();
    Renderer :: setColor3 (this->color);
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glClear(GL_DEPTH_BUFFER_BIT);

    Renderer :: setColor3 (this->wireframeColor);
    glPolygonMode      (GL_FRONT, GL_LINE);
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glPolygonMode      (GL_FRONT, GL_FILL);
    this->renderEnd    ();
  }

  void reset () {
    this->scalingMatrix     = glm::mat4x4 (1.0f);
    this->rotationMatrix    = glm::mat4x4 (1.0f);
    this->translationMatrix = glm::mat4x4 (1.0f);
    this->vertices.clear ();
    this->indices.clear  ();
    this->normals.clear  ();
    OpenGLUtil :: safeDeleteArray  (this->arrayObjectId);
    OpenGLUtil :: safeDeleteBuffer (this->vertexBufferId);
    OpenGLUtil :: safeDeleteBuffer (this->indexBufferId);
    OpenGLUtil :: safeDeleteBuffer (this->normalBufferId);
  }

  void toggleRenderMode () {
    this->renderMode = RenderModeUtil :: toggle (this->renderMode);
  }

  void scale (const glm::vec3& v) {
    this->scalingMatrix = glm::scale (this->scalingMatrix, v);
  }

  void scaling (const glm::vec3& v) {
    this->scalingMatrix = glm::scale (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 scaling () const {
    return glm::vec3 (this->scalingMatrix * glm::vec4 (1.0f));
  }

  void translate (const glm::vec3& v) {
    this->translationMatrix = glm::translate (this->translationMatrix, v);
  }

  void position (const glm::vec3& v) {
    this->translationMatrix = glm::translate (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 position () const {
    return Util::transformPosition (this->modelMatrix (), glm::vec3 (0.0f));
  }

  static Mesh cube () {
    Mesh m;
    float d = 0.5f;
    m.addVertex ( glm::vec3 (-d, -d, -d) );
    m.addVertex ( glm::vec3 (-d, -d, +d) );
    m.addVertex ( glm::vec3 (-d, +d, -d) );
    m.addVertex ( glm::vec3 (-d, +d, +d) );
    m.addVertex ( glm::vec3 (+d, -d, -d) );
    m.addVertex ( glm::vec3 (+d, -d, +d) );
    m.addVertex ( glm::vec3 (+d, +d, -d) );
    m.addVertex ( glm::vec3 (+d, +d, +d) );

    for (unsigned int i = 0; i < m.numVertices (); i++) {
      m.setNormal (i, glm::normalize (m.vertex (i)));
    }

    m.addIndex (0); m.addIndex (1); m.addIndex (2);
    m.addIndex (3); m.addIndex (2); m.addIndex (1);

    m.addIndex (1); m.addIndex (5); m.addIndex (3);
    m.addIndex (7); m.addIndex (3); m.addIndex (5);

    m.addIndex (5); m.addIndex (4); m.addIndex (7);
    m.addIndex (6); m.addIndex (7); m.addIndex (4);

    m.addIndex (4); m.addIndex (0); m.addIndex (6);
    m.addIndex (2); m.addIndex (6); m.addIndex (0);

    m.addIndex (3); m.addIndex (7); m.addIndex (2);
    m.addIndex (6); m.addIndex (2); m.addIndex (7);

    m.addIndex (0); m.addIndex (4); m.addIndex (1);
    m.addIndex (5); m.addIndex (1); m.addIndex (4);
    return m;
  }

  static Mesh sphere (unsigned int rings, unsigned int sectors) {
    assert (rings > 1 && sectors > 2);
    Mesh m;

    const float radius     = 1.0f;
    const float ringStep   =        M_PI / float (rings);
    const float sectorStep = 2.0f * M_PI / float (sectors);
          float phi        = ringStep;
          float theta      = 0.0f;

    // inner rings vertices
    for (unsigned int r = 0; r < rings - 1; r++) {
      for (unsigned int s = 0; s < sectors; s++) {
        float x = radius * sin (theta) * sin (phi);
        float y = radius * cos (phi);
        float z = radius * cos (theta) * sin (phi);

        m.addVertex (glm::vec3 (x,y,z));

        theta += sectorStep;
      }
      phi += ringStep;
    }

    // caps vertices
    unsigned int topCapIndex = m.addVertex (glm::vec3 (0.0f, radius, 0.0f));
    unsigned int botCapIndex = m.addVertex (glm::vec3 (0.0f,-radius, 0.0f));

    // set normals
    for (unsigned int i = 0; i < m.numVertices (); i++) {
      m.setNormal (i, glm::normalize (m.vertex (i)));
    }

    // inner rings indices
    for (unsigned int r = 0; r < rings - 2; r++) {
      for (unsigned int s = 0; s < sectors; s++) {
        m.addIndex ((sectors * r) + s);
        m.addIndex ((sectors * (r+1)) + s);
        m.addIndex ((sectors * r) + ((s+1) % sectors));

        m.addIndex ((sectors * (r+1)) + ((s+1) % sectors));
        m.addIndex ((sectors * r) + ((s+1) % sectors));
        m.addIndex ((sectors * (r+1)) + s);
      }
    }

    // caps indices
    for (unsigned int s = 0; s < sectors; s++) {
      m.addIndex (topCapIndex);
      m.addIndex (s);
      m.addIndex ((s+1) % sectors);

      m.addIndex (botCapIndex);
      m.addIndex ((sectors * (rings-2)) + ((s+1) % sectors));
      m.addIndex ((sectors * (rings-2)) + s);
    }
    return m;
  }

  static Mesh icosphere (unsigned int numSubdivisions) {
    Mesh m;
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
      (unsigned int i1, unsigned int i2) -> unsigned int {
        unsigned int lowerI = i1 < i2 ? i1 : i2;
        unsigned int upperI = i1 < i2 ? i2 : i1;
        Key          key    = getKey (lowerI, upperI);

        VertexCache::iterator it = vertexCache.find (key);
        if (it == vertexCache.end ()) {
          unsigned int n = addIcoVertex (Util::between ( m.vertex (lowerI)
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
      (unsigned int s,unsigned int i1, unsigned int i2, unsigned int i3) -> void {
        if (s == 0) {
          m.addIndex (i1); m.addIndex (i2); m.addIndex (i3); 
        }
        else {
          unsigned int i12 = lookupVertex (i1,i2);
          unsigned int i23 = lookupVertex (i2,i3);
          unsigned int i31 = lookupVertex (i3,i1);

          subdivide (s-1, i1 , i12, i31);
          subdivide (s-1, i2 , i23, i12);
          subdivide (s-1, i3 , i31, i23);
          subdivide (s-1, i12, i23, i31);
        }
      };

    float t = (1.0f + glm::sqrt (5.0f)) * 0.5f;

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

    // set normals
    for (unsigned int i = 0; i < m.numVertices (); i++) {
      m.setNormal (i, glm::normalize (m.vertex (i)));
    }
    return m;
  }

  static Mesh cone (unsigned int numBaseVertices) {
    assert (numBaseVertices >= 3);

          Mesh  m;
    const float c = 2.0f * glm::pi <float> () / float (numBaseVertices);

    for (unsigned int i = 0; i < numBaseVertices; i++) {
      m.addVertex (glm::vec3 ( glm::sin <float> (float (i) * c)
                             , -0.5f
                             , glm::cos <float> (float (i) * c)));
    }
    m.addVertex (glm::vec3 (0.0f, -0.5f, 0.0f));
    m.addVertex (glm::vec3 (0.0f,  0.5f, 0.0f));

    for (unsigned int i = 0; i < m.numVertices (); i++) {
      m.setNormal (i, glm::normalize (m.vertex (i)));
    }

    for (unsigned int i = 0; i < numBaseVertices - 1; i++) {
      m.addIndex (i); m.addIndex (i + 1); m.addIndex (numBaseVertices + 1);
      m.addIndex (i + 1); m.addIndex (i); m.addIndex (numBaseVertices);
    }
    m.addIndex (numBaseVertices - 1); m.addIndex (0); m.addIndex (numBaseVertices + 1);
    m.addIndex (0); m.addIndex (numBaseVertices - 1); m.addIndex (numBaseVertices);
    return m;
  }

  static Mesh cylinder (unsigned int numVertices) {
    assert (numVertices >= 3);

          Mesh  m;
    const float c = 2.0f * glm::pi <float> () / float (numVertices);

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

    for (unsigned int i = 0; i < m.numVertices (); i++) {
      m.setNormal (i, glm::normalize (m.vertex (i)));
    }

    for (unsigned int i = 0; i < numVertices - 1; i++) {
      m.addIndex (i); m.addIndex (i + 1); m.addIndex (i + numVertices); 
      m.addIndex (i + numVertices + 1); m.addIndex (i + numVertices); m.addIndex (i + 1);

      m.addIndex (i + 1); m.addIndex (i); m.addIndex (2 * numVertices);
      m.addIndex (i + numVertices); m.addIndex (i + numVertices + 1); m.addIndex ((2 * numVertices) + 1);
    }
    m.addIndex (numVertices - 1); m.addIndex (0); m.addIndex ((2 * numVertices) - 1);
    m.addIndex (numVertices); m.addIndex ((2 * numVertices) - 1); m.addIndex (0);

    m.addIndex (0); m.addIndex (numVertices - 1); m.addIndex (2 * numVertices);
    m.addIndex ((2 * numVertices) - 1); m.addIndex (numVertices); m.addIndex ((2 * numVertices) + 1);
    return m;
  }
};

DELEGATE_BIG6    (Mesh)
DELEGATE_CONST   (unsigned int      , Mesh, numVertices)
DELEGATE_CONST   (unsigned int      , Mesh, numIndices)
DELEGATE_CONST   (unsigned int      , Mesh, numNormals)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfVertices)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfIndices)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfNormals)
DELEGATE1_CONST  (glm::vec3         , Mesh, vertex, unsigned int)
DELEGATE1_CONST  (glm::vec3         , Mesh, worldVertex, unsigned int)
DELEGATE1_CONST  (unsigned int      , Mesh, index, unsigned int)
DELEGATE1_CONST  (glm::vec3         , Mesh, normal, unsigned int)

DELEGATE1        (unsigned int      , Mesh, addIndex, unsigned int)
DELEGATE1        (unsigned int      , Mesh, addVertex, const glm::vec3&)
DELEGATE2        (void              , Mesh, setIndex, unsigned int, unsigned int)
DELEGATE2        (void              , Mesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2        (void              , Mesh, setNormal, unsigned int, const glm::vec3&)
DELEGATE         (void              , Mesh, popVertex)
DELEGATE1        (void              , Mesh, popIndices, unsigned int)
DELEGATE1        (void              , Mesh, allocateIndices, unsigned int)
DELEGATE1        (void              , Mesh, resizeIndices, unsigned int)

DELEGATE         (void              , Mesh, bufferData)
DELEGATE_CONST   (glm::mat4x4       , Mesh, modelMatrix)
DELEGATE         (void              , Mesh, fixModelMatrix)
DELEGATE         (void              , Mesh, renderBegin)
DELEGATE         (void              , Mesh, renderEnd)
DELEGATE         (void              , Mesh, render)
DELEGATE         (void              , Mesh, renderSolid)
DELEGATE         (void              , Mesh, renderWireframe)
DELEGATE         (void              , Mesh, reset)
SETTER           (RenderMode        , Mesh, renderMode)
DELEGATE         (void              , Mesh, toggleRenderMode)

DELEGATE1        (void              , Mesh, scale      , const glm::vec3&)
DELEGATE1        (void              , Mesh, scaling    , const glm::vec3&)
DELEGATE_CONST   (glm::vec3         , Mesh, scaling)
DELEGATE1        (void              , Mesh, translate  , const glm::vec3&)
DELEGATE1        (void              , Mesh, position   , const glm::vec3&)
DELEGATE_CONST   (glm::vec3         , Mesh, position)
SETTER           (const glm::mat4x4&, Mesh, rotationMatrix)
GETTER_CONST     (const Color&      , Mesh, color)
SETTER           (const Color&      , Mesh, color)

DELEGATE_STATIC  (Mesh, Mesh, cube)
DELEGATE2_STATIC (Mesh, Mesh, sphere, unsigned int, unsigned int)
DELEGATE1_STATIC (Mesh, Mesh, icosphere, unsigned int)
DELEGATE1_STATIC (Mesh, Mesh, cone, unsigned int)
DELEGATE1_STATIC (Mesh, Mesh, cylinder, unsigned int)
