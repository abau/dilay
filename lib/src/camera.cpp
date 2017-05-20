/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "opengl.hpp"
#include "primitive/ray.hpp"
#include "renderer.hpp"
#include "util.hpp"

struct Camera::Impl
{
  Camera*     self;
  Renderer    renderer;
  glm::vec3   gazePoint;
  glm::vec3   toEyePoint;
  glm::vec3   up;
  glm::vec3   right;
  glm::mat4x4 projection;
  glm::mat4x4 view;
  glm::mat4x4 viewRotation;
  glm::uvec2  resolution;
  float       nearClipping;
  float       farClipping;
  float       fieldOfView;

  Impl (Camera* s, const Config& config)
    : self (s)
    , renderer (config)
    , resolution (config.get<int> ("window/initial-width"),
                  config.get<int> ("window/initial-height"))
  {
    this->set (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 0.0f, 6.0f),
               glm::vec3 (0.0f, 1.0f, 0.0f));
    this->runFromConfig (config);
  }

  glm::vec3 position () const { return this->gazePoint + this->toEyePoint; }

  glm::mat4x4 world () const
  {
    const glm::vec3 x = this->right;
    const glm::vec3 z = glm::normalize (this->toEyePoint);
    const glm::vec3 y = glm::cross (z, x);
    const glm::vec3 p = this->position ();

    return glm::colMajor4 (glm::vec4 (x, 0.0f), glm::vec4 (y, 0.0f), glm::vec4 (z, 0.0f),
                           glm::vec4 (p, 1.0f));
  }

  void updateResolution (const glm::uvec2& dimension)
  {
    this->resolution = dimension;
    this->updateProjection ();
  }

  void setModelViewProjection (const glm::mat4x4& model, const glm::mat3x3& modelNormal,
                               bool onlyRotation)
  {
    this->renderer.setModel (&model[0][0], &modelNormal[0][0]);
    this->renderer.setProjection (&this->projection[0][0]);

    if (onlyRotation)
    {
      this->renderer.setView (&this->viewRotation[0][0]);
    }
    else
    {
      this->renderer.setView (&this->view[0][0]);
    }
  }

  void set (const glm::vec3& g, const glm::vec3& e, const glm::vec3& u)
  {
    this->gazePoint = g;
    this->toEyePoint = e;
    this->up = glm::normalize (u);
    this->right = Util::colinear (e, u) ? glm::vec3 (1.0f, 0.0f, 0.0f)
                                        : glm::normalize (glm::cross (this->up, this->toEyePoint));
    this->updateView ();
  }

  void setGaze (const glm::vec3& g)
  {
    this->gazePoint = g;
    this->updateView ();
  }

  void stepAlongGaze (float factor)
  {
    const float d = glm::length (this->toEyePoint);

    if (factor < 1.0f && d > 0.01f)
    {
      this->toEyePoint *= glm::vec3 (factor);
    }
    else if (factor > 1.0f)
    {
      this->toEyePoint *= glm::vec3 (factor);
    }
    this->updateView ();
  }

  void verticalRotation (float angle)
  {
    const glm::quat q = glm::angleAxis (angle, this->up);
    this->right = glm::rotate (q, this->right);
    this->toEyePoint = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  void horizontalRotation (float angle)
  {
    const glm::quat q = glm::angleAxis (angle, this->right);
    this->toEyePoint = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  glm::vec4 viewport () const { return glm::vec4 (0, 0, this->resolution.x, this->resolution.y); }

  glm::vec2 fromWorld (const glm::vec3& p, const glm::mat4x4& model, bool onlyRotation) const
  {
    const glm::mat4x4 mv = onlyRotation ? this->viewRotation * model : this->view * model;
    const glm::vec3   w = glm::project (p, mv, this->projection, this->viewport ());

    return glm::vec2 (w.x, float(resolution.y) - w.y);
  }

  glm::vec3 toWorld (const glm::ivec2& p, float z = 0.0f) const
  {
    const float invY = this->resolution.y - float(p.y);
    const float normZ = z / this->farClipping;
    return glm::unProject (glm::vec3 (float(p.x), invY, normZ), this->view, this->projection,
                           this->viewport ());
  }

  float toWorld (float length, float z)
  {
    const float onNearPlane = 2.0f * this->nearClipping * glm::tan (this->fieldOfView * 0.5f) *
                              length / float(this->resolution.x);
    return onNearPlane * (this->nearClipping + z) / this->nearClipping;
  }

  PrimRay ray (const glm::ivec2& p) const
  {
    const glm::vec3 w = this->toWorld (p);
    const glm::vec3 eye = this->position ();
    return PrimRay (eye, w - eye);
  }

  void updateProjection ()
  {
    OpenGL::glViewport (0, 0, this->resolution.x, this->resolution.y);
    this->projection =
      glm::perspective (this->fieldOfView, float(this->resolution.x) / float(this->resolution.y),
                        this->nearClipping, this->farClipping);
  }

  void updateView ()
  {
    const glm::vec3 realUp = glm::normalize (glm::cross (this->toEyePoint, this->right));

    this->view = glm::lookAt (this->position (), this->gazePoint, realUp);
    this->viewRotation = glm::lookAt (glm::normalize (this->toEyePoint), glm::vec3 (0.0f), realUp);
    this->renderer.setEyePoint (this->position ());
  }

  Dimension primaryDimension () const
  {
    const glm::vec3 t = glm::abs (this->toEyePoint);
    if (t.x > t.y && t.x > t.z)
    {
      return Dimension::X;
    }
    else if (t.y > t.z)
    {
      return Dimension::Y;
    }
    return Dimension::Z;
  }

  void runFromConfig (const Config& config)
  {
    this->renderer.fromConfig (config);

    this->nearClipping = config.get<float> ("editor/camera/near-clipping");
    this->farClipping = config.get<float> ("editor/camera/far-clipping");
    this->fieldOfView = glm::radians (config.get<float> ("editor/camera/field-of-view"));

    this->updateProjection ();
  }
};

DELEGATE1_BIG3_SELF (Camera, const Config&)

GETTER_CONST (Renderer&, Camera, renderer)
GETTER_CONST (const glm::uvec2&, Camera, resolution)
GETTER_CONST (const glm::vec3&, Camera, gazePoint)
GETTER_CONST (const glm::vec3&, Camera, toEyePoint)
GETTER_CONST (const glm::vec3&, Camera, up)
GETTER_CONST (const glm::vec3&, Camera, right)
GETTER_CONST (const glm::mat4x4&, Camera, view)
GETTER_CONST (const glm::mat4x4&, Camera, viewRotation)
DELEGATE_CONST (glm::vec3, Camera, position)
DELEGATE_CONST (glm::mat4x4, Camera, world)
DELEGATE1 (void, Camera, updateResolution, const glm::uvec2&)
DELEGATE3 (void, Camera, setModelViewProjection, const glm::mat4x4&, const glm::mat3x3&, bool)
DELEGATE3 (void, Camera, set, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE1 (void, Camera, setGaze, const glm::vec3&)
DELEGATE1 (void, Camera, stepAlongGaze, float)
DELEGATE1 (void, Camera, verticalRotation, float)
DELEGATE1 (void, Camera, horizontalRotation, float)
DELEGATE3_CONST (glm::vec2, Camera, fromWorld, const glm::vec3&, const glm::mat4x4&, bool)
DELEGATE2_CONST (glm::vec3, Camera, toWorld, const glm::ivec2&, float)
DELEGATE2_CONST (float, Camera, toWorld, float, float)
DELEGATE1_CONST (PrimRay, Camera, ray, const glm::ivec2&)
DELEGATE_CONST (Dimension, Camera, primaryDimension)
DELEGATE1 (void, Camera, runFromConfig, const Config&)
