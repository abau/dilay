/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "opengl-buffer-id.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "util.hpp"

namespace
{
  static_assert (sizeof (glm::vec3) == 3 * sizeof (float), "Unexpected memory layout");

  template <typename T> struct BufferedData
  {
    OpenGLBufferId id;
    std::vector<T> data;
    unsigned int   dataLowerBound;
    unsigned int   dataUpperBound;
    unsigned int   bufferSize;

    BufferedData ()
    {
      this->reset ();
    }

    void reset ()
    {
      this->id.reset ();
      this->data.clear ();
      this->resetBounds ();
      this->bufferSize = 0;
    }

    void resetBounds ()
    {
      this->dataLowerBound = Util::maxUnsignedInt ();
      this->dataUpperBound = 0;
    }

    unsigned int numElements () const
    {
      return this->data.size ();
    }

    void reserve (unsigned int size)
    {
      this->data.reserve (size);
    }

    void shrink (unsigned int n)
    {
      assert (n <= this->numElements ());
      this->data.resize (n);
      this->dataLowerBound = 0;
      this->dataUpperBound = n > 0 ? n - 1 : 0;
    }

    void updateBounds (unsigned int index)
    {
      this->dataLowerBound = glm::min (this->dataLowerBound, index);
      this->dataUpperBound = glm::max (this->dataUpperBound, index);
    }

    unsigned int add (const T& value)
    {
      this->data.push_back (value);
      this->updateBounds (this->numElements () - 1);
      return this->numElements () - 1;
    }

    void set (unsigned int index, const T& value)
    {
      assert (index < this->numElements ());
      this->data[index] = value;
      this->updateBounds (index);
    }

    const T& get (unsigned int index) const
    {
      assert (index < this->numElements ());
      return this->data[index];
    }

    void bufferData (unsigned int target)
    {
      if (this->id.isValid () == false)
      {
        this->id.allocate ();
        this->bufferSize = 0;
      }
      OpenGL::glBindBuffer (target, this->id.id ());

      const unsigned int dataSize = this->numElements () * sizeof (T);

      if (this->bufferSize == 0)
      {
        OpenGL::glBufferData (target, dataSize, this->data.data (), OpenGL::StaticDraw ());
        this->bufferSize = dataSize;
      }
      else if (this->bufferSize < dataSize)
      {
        const unsigned int newBufferSize = this->bufferSize + (100 * (dataSize - this->bufferSize));

        OpenGL::glBufferData (target, newBufferSize, nullptr, OpenGL::StaticDraw ());
        OpenGL::glBufferSubData (target, 0, dataSize, this->data.data ());
        this->bufferSize = newBufferSize;
      }
      else if (this->dataLowerBound <= this->dataUpperBound)
      {
        const unsigned int size = (this->dataUpperBound - this->dataLowerBound + 1) * sizeof (T);

        OpenGL::glBufferSubData (target, this->dataLowerBound * sizeof (T), size,
                                 &this->get (this->dataLowerBound));
      }
      this->resetBounds ();
    }
  };
}

struct Mesh::Impl
{
  // cf. copy-constructor, reset
  glm::mat4x4                scalingMatrix;
  glm::mat4x4                rotationMatrix;
  glm::mat4x4                translationMatrix;
  BufferedData<glm::vec3>    vertices;
  BufferedData<unsigned int> indices;
  BufferedData<glm::vec3>    normals;
  Color                      color;
  Color                      wireframeColor;

  RenderMode renderMode;

  Impl ()
    : scalingMatrix (glm::mat4x4 (1.0f))
    , rotationMatrix (glm::mat4x4 (1.0f))
    , translationMatrix (glm::mat4x4 (1.0f))
    , color (Color::White ())
    , wireframeColor (Color::Black ())
  {
    this->renderMode.smoothShading (true);
  }

  unsigned int numVertices () const
  {
    return this->vertices.numElements ();
  }

  unsigned int numIndices () const
  {
    return this->indices.numElements ();
  }

  const glm::vec3& vertex (unsigned int i) const
  {
    return this->vertices.get (i);
  }

  unsigned int index (unsigned int i) const
  {
    return this->indices.get (i);
  }

  const glm::vec3& normal (unsigned int i) const
  {
    return this->normals.get (i);
  }

  void copyNonGeometry (const Mesh& source)
  {
    this->scalingMatrix = source.impl->scalingMatrix;
    this->rotationMatrix = source.impl->rotationMatrix;
    this->translationMatrix = source.impl->translationMatrix;
    this->color = source.impl->color;
    this->wireframeColor = source.impl->wireframeColor;
    this->renderMode = source.impl->renderMode;
  }

  unsigned int addIndex (unsigned int i)
  {
    return this->indices.add (i);
  }

  void reserveIndices (unsigned int n)
  {
    this->indices.reserve (n);
  }

  void shrinkIndices (unsigned int n)
  {
    this->indices.shrink (n);
  }

  unsigned int addVertex (const glm::vec3& v)
  {
    return this->addVertex (v, glm::vec3 (0.0f));
  }

  unsigned int addVertex (const glm::vec3& v, const glm::vec3& n)
  {
    assert (Util::isNaN (v) == false);
    assert (Util::isNaN (n) == false);
    assert (this->vertices.numElements () == this->normals.numElements ());

    this->vertices.add (v);
    return this->normals.add (n);
  }

  void reserveVertices (unsigned int n)
  {
    this->vertices.reserve (n);
    this->normals.reserve (n);
  }

  void shrinkVertices (unsigned int n)
  {
    this->vertices.shrink (n);
    this->normals.shrink (n);
  }

  void index (unsigned int i, unsigned int index)
  {
    this->indices.set (i, index);
  }

  void vertex (unsigned int i, const glm::vec3& v)
  {
    assert (Util::isNaN (v) == false);
    this->vertices.set (i, v);
  }

  void normal (unsigned int i, const glm::vec3& n)
  {
    assert (Util::isNaN (n) == false);
    this->normals.set (i, n);
  }

  void bufferData ()
  {
    this->vertices.bufferData (OpenGL::ArrayBuffer ());
    this->indices.bufferData (OpenGL::ElementArrayBuffer ());
    this->normals.bufferData (OpenGL::ArrayBuffer ());

    OpenGL::glBindBuffer (OpenGL::ElementArrayBuffer (), 0);
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);
  }

  glm::mat4x4 modelMatrix () const
  {
    return this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
  }

  glm::mat3x3 modelNormalMatrix () const
  {
    return glm::inverseTranspose (glm::mat3x3 (this->modelMatrix ()));
  }

  void setModelMatrix (Camera& camera, bool noZoom) const
  {
    camera.setModelViewProjection (this->modelMatrix (), this->modelNormalMatrix (), noZoom);
  }

  void renderBegin (Camera& camera) const
  {
    if (this->renderMode.renderWireframe () && OpenGL::supportsGeometryShader () == false)
    {
      RenderMode nonWireframeRenderMode (this->renderMode);
      nonWireframeRenderMode.renderWireframe (false);

      camera.renderer ().setProgram (nonWireframeRenderMode);
    }
    else
    {
      camera.renderer ().setProgram (this->renderMode);
    }
    camera.renderer ().setColor (this->color);
    camera.renderer ().setWireframeColor (this->wireframeColor);

    this->setModelMatrix (camera, this->renderMode.cameraRotationOnly ());

    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), this->vertices.id.id ());
    OpenGL::glEnableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glVertexAttribPointer (OpenGL::PositionIndex, 3, OpenGL::Float (), false, 0, 0);

    OpenGL::glBindBuffer (OpenGL::ElementArrayBuffer (), this->indices.id.id ());

    if (this->renderMode.smoothShading ())
    {
      OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), this->normals.id.id ());
      OpenGL::glEnableVertexAttribArray (OpenGL::NormalIndex);
      OpenGL::glVertexAttribPointer (OpenGL::NormalIndex, 3, OpenGL::Float (), false, 0, 0);
    }
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);

    if (this->renderMode.noDepthTest ())
    {
      OpenGL::glDisable (OpenGL::DepthTest ());
    }
  }

  void renderEnd () const
  {
    OpenGL::glDisableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glDisableVertexAttribArray (OpenGL::NormalIndex);
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);
    OpenGL::glBindBuffer (OpenGL::ElementArrayBuffer (), 0);
    OpenGL::glEnable (OpenGL::DepthTest ());
  }

  void render (Camera& camera) const
  {
    this->renderBegin (camera);

    OpenGL::glDrawElements (OpenGL::Triangles (), this->numIndices (), OpenGL::UnsignedInt (),
                            nullptr);

    this->renderEnd ();
  }

  void renderLines (Camera& camera) const
  {
    this->renderBegin (camera);
    OpenGL::glDrawElements (OpenGL::Lines (), this->numIndices (), OpenGL::UnsignedInt (), nullptr);
    this->renderEnd ();
  }

  void reset ()
  {
    this->scalingMatrix = glm::mat4x4 (1.0f);
    this->rotationMatrix = glm::mat4x4 (1.0f);
    this->translationMatrix = glm::mat4x4 (1.0f);
    this->resetGeometry ();
  }

  void resetGeometry ()
  {
    this->vertices.reset ();
    this->indices.reset ();
    this->normals.reset ();
  }

  void scale (const glm::vec3& v)
  {
    this->scalingMatrix = glm::scale (this->scalingMatrix, v);
  }

  void scaling (const glm::vec3& v)
  {
    this->scalingMatrix = glm::scale (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 scaling () const
  {
    return glm::vec3 (this->scalingMatrix[0][0], this->scalingMatrix[1][1],
                      this->scalingMatrix[2][2]);
  }

  void translate (const glm::vec3& v)
  {
    this->translationMatrix = glm::translate (this->translationMatrix, v);
  }

  void position (const glm::vec3& v)
  {
    this->translationMatrix = glm::translate (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 position () const
  {
    return glm::vec3 (this->translationMatrix[3][0], this->translationMatrix[3][1],
                      this->translationMatrix[3][2]);
  }

  void rotationX (float angle)
  {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (1.0f, 0.0f, 0.0f));
  }

  void rotationY (float angle)
  {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (0.0f, 1.0f, 0.0f));
  }

  void rotationZ (float angle)
  {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (0.0f, 0.0f, 1.0f));
  }

  void rotateX (float angle)
  {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (1.0f, 0.0f, 0.0f));
  }

  void rotateY (float angle)
  {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (0.0f, 1.0f, 0.0f));
  }

  void rotateZ (float angle)
  {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (0.0f, 0.0f, 1.0f));
  }

  void normalize ()
  {
    const glm::mat4x4 model = this->modelMatrix ();
    const glm::mat3x3 modelNormal = this->modelNormalMatrix ();

    for (unsigned int i = 0; i < this->numVertices (); i++)
    {
      this->vertex (i, Util::transformPosition (model, this->vertex (i)));
      if (Util::isNotNull (this->normal (i)))
      {
        this->normal (i, glm::normalize (modelNormal * this->normal (i)));
      }
    }
    this->position (glm::vec3 (0.0f));
    this->scaling (glm::vec3 (1.0f));
    this->rotationMatrix = glm::mat4x4 (1.0f);
  }

  glm::vec3 center () const
  {
    glm::vec3 min, max;

    this->minMax (min, max);

    return (min + max) * 0.5f;
  }

  void minMax (glm::vec3& min, glm::vec3& max) const
  {
    min = glm::vec3 (Util::maxFloat ());
    max = glm::vec3 (Util::minFloat ());

    for (unsigned int i = 0; i < this->numVertices (); i++)
    {
      min = glm::min (min, this->vertices.get (i));
      max = glm::max (max, this->vertices.get (i));
    }
  }
};

DELEGATE_BIG6 (Mesh)
DELEGATE_CONST (unsigned int, Mesh, numVertices)
DELEGATE_CONST (unsigned int, Mesh, numIndices)
DELEGATE1_CONST (const glm::vec3&, Mesh, vertex, unsigned int)
DELEGATE1_CONST (unsigned int, Mesh, index, unsigned int)
DELEGATE1_CONST (const glm::vec3&, Mesh, normal, unsigned int)

DELEGATE1 (void, Mesh, copyNonGeometry, const Mesh&)
DELEGATE1 (unsigned int, Mesh, addIndex, unsigned int)
DELEGATE1 (void, Mesh, reserveIndices, unsigned int)
DELEGATE1 (void, Mesh, shrinkIndices, unsigned int)
DELEGATE1 (unsigned int, Mesh, addVertex, const glm::vec3&)
DELEGATE2 (unsigned int, Mesh, addVertex, const glm::vec3&, const glm::vec3&)
DELEGATE1 (void, Mesh, reserveVertices, unsigned int)
DELEGATE1 (void, Mesh, shrinkVertices, unsigned int)
DELEGATE2 (void, Mesh, index, unsigned int, unsigned int)
DELEGATE2 (void, Mesh, vertex, unsigned int, const glm::vec3&)
DELEGATE2 (void, Mesh, normal, unsigned int, const glm::vec3&)

DELEGATE (void, Mesh, bufferData)
DELEGATE_CONST (glm::mat4x4, Mesh, modelMatrix)
DELEGATE_CONST (glm::mat3x3, Mesh, modelNormalMatrix)
DELEGATE1_CONST (void, Mesh, renderBegin, Camera&)
DELEGATE_CONST (void, Mesh, renderEnd)
DELEGATE1_CONST (void, Mesh, render, Camera&)
DELEGATE1_CONST (void, Mesh, renderLines, Camera&)
DELEGATE (void, Mesh, reset)
DELEGATE (void, Mesh, resetGeometry)
GETTER_CONST (const RenderMode&, Mesh, renderMode)
GETTER (RenderMode&, Mesh, renderMode)

DELEGATE1 (void, Mesh, scale, const glm::vec3&)
DELEGATE1 (void, Mesh, scaling, const glm::vec3&)
DELEGATE_CONST (glm::vec3, Mesh, scaling)
DELEGATE1 (void, Mesh, translate, const glm::vec3&)
DELEGATE1 (void, Mesh, position, const glm::vec3&)
DELEGATE_CONST (glm::vec3, Mesh, position)
SETTER (const glm::mat4x4&, Mesh, rotationMatrix)
GETTER_CONST (const glm::mat4x4&, Mesh, rotationMatrix)
DELEGATE1 (void, Mesh, rotationX, float)
DELEGATE1 (void, Mesh, rotationY, float)
DELEGATE1 (void, Mesh, rotationZ, float)
DELEGATE1 (void, Mesh, rotateX, float)
DELEGATE1 (void, Mesh, rotateY, float)
DELEGATE1 (void, Mesh, rotateZ, float)
DELEGATE (void, Mesh, normalize)
DELEGATE_CONST (glm::vec3, Mesh, center)
DELEGATE2_CONST (void, Mesh, minMax, glm::vec3&, glm::vec3&)
GETTER_CONST (const Color&, Mesh, color)
SETTER (const Color&, Mesh, color)
GETTER_CONST (const Color&, Mesh, wireframeColor)
SETTER (const Color&, Mesh, wireframeColor)
