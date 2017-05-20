/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "opengl-buffer-id.hpp"
#include "opengl.hpp"

OpenGLBufferId::OpenGLBufferId ()
  : _id (0)
{
}

OpenGLBufferId::OpenGLBufferId (const OpenGLBufferId&)
  : OpenGLBufferId ()
{
}

OpenGLBufferId::OpenGLBufferId (OpenGLBufferId&& other)
  : _id (other._id)
{
}

const OpenGLBufferId& OpenGLBufferId::operator= (const OpenGLBufferId&) { return *this; }

const OpenGLBufferId& OpenGLBufferId::operator= (OpenGLBufferId&& other)
{
  this->_id = other._id;
  return *this;
}

OpenGLBufferId::~OpenGLBufferId () { this->reset (); }

unsigned int OpenGLBufferId::id () const { return this->_id; }

bool OpenGLBufferId::isValid () const { return this->_id > 0; }

void OpenGLBufferId::allocate ()
{
  assert (this->isValid () == false);

  OpenGL::glGenBuffers (1, &this->_id);

  assert (this->isValid ());
}

void OpenGLBufferId::reset ()
{
  if (this->isValid ())
  {
    OpenGL::safeDeleteBuffer (this->_id);
  }
}
