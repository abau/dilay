/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cstdlib>
#include "render-mode.hpp"
#include "shader.hpp"
#include "util.hpp"

RenderMode::RenderMode ()
{
  this->smoothShading (true);
  this->renderWireframe (false);
  this->cameraRotationOnly (false);
  this->noDepthTest (false);
}

RenderMode::RenderMode (const RenderMode& other)
  : flags (other.flags)
{
}

bool RenderMode::smoothShading () const
{
  return this->flags.get<0> ();
}

bool RenderMode::flatShading () const
{
  return this->flags.get<1> ();
}

bool RenderMode::constantShading () const
{
  return this->flags.get<2> ();
}

bool RenderMode::renderWireframe () const
{
  return this->flags.get<3> ();
}

bool RenderMode::cameraRotationOnly () const
{
  return this->flags.get<4> ();
}

bool RenderMode::noDepthTest () const
{
  return this->flags.get<5> ();
}

const char* RenderMode::vertexShader () const
{
  if (this->smoothShading ())
  {
    return Shader::smoothVertexShader ();
  }
  else if (this->flatShading ())
  {
    return Shader::flatVertexShader ();
  }
  else if (this->constantShading ())
  {
    return Shader::constantVertexShader ();
  }
  else
  {
    DILAY_IMPOSSIBLE
  }
}

const char* RenderMode::fragmentShader () const
{
  if (this->smoothShading ())
  {
    return this->renderWireframe () ? Shader::smoothWireframeFragmentShader ()
                                    : Shader::smoothFragmentShader ();
  }
  else if (this->flatShading ())
  {
    return this->renderWireframe () ? Shader::flatWireframeFragmentShader ()
                                    : Shader::flatFragmentShader ();
  }
  else if (this->constantShading ())
  {
    return this->renderWireframe () ? Shader::constantWireframeFragmentShader ()
                                    : Shader::constantFragmentShader ();
  }
  else
  {
    DILAY_IMPOSSIBLE
  }
}

void RenderMode::smoothShading (bool v)
{
  this->flags.set<0> (v);
  if (v)
  {
    this->flags.set<1> (false);
    this->flags.set<2> (false);
  }
}

void RenderMode::flatShading (bool v)
{
  this->flags.set<1> (v);
  if (v)
  {
    this->flags.set<0> (false);
    this->flags.set<2> (false);
  }
}

void RenderMode::constantShading (bool v)
{
  this->flags.set<2> (v);
  if (v)
  {
    this->flags.set<0> (false);
    this->flags.set<1> (false);
  }
}

void RenderMode::renderWireframe (bool v)
{
  this->flags.set<3> (v);
}

void RenderMode::cameraRotationOnly (bool v)
{
  this->flags.set<4> (v);
}

void RenderMode::noDepthTest (bool v)
{
  this->flags.set<5> (v);
}
