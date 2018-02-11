/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_OPENGL_BUFFER_ID
#define DILAY_OPENGL_BUFFER_ID

#include "macro.hpp"

class OpenGLBufferId
{
public:
  DECLARE_BIG6 (OpenGLBufferId)

  unsigned int id () const;
  bool         isValid () const;

  void allocate ();
  void reset ();

private:
  unsigned int _id;
};

#endif
