# Dilay - a 3D sculpting application

**Project website:** http://abau.org/dilay

## Installing

In order to run Dilay, the following requirements have to be met:

- Graphics card that supports OpenGL 2.1 (for wireframe rendering
  the `GL_EXT_geometry_shader4` OpenGL extension must be supported)
- three-button mouse with a mouse wheel

### Windows

Pre-built installers for Windows are provided on the [project website](http://abau.org/dilay/download.html).

### Linux

Pre-built packages for Linux are not provided.  Instead, Dilay needs to be
built from source, which requires the following tools:

- C++14 compliant compiler
- [Qt](https://www.qt.io) (version 5.4 or later) 
- [GLM](http://glm.g-truc.net)

Firstly, get Dilay's sources:

    $ git clone https://github.com/abau/dilay.git
    $ cd dilay

Building is done via:

    $ qmake -r PREFIX=DIR
    $ make release

If GLM is not installed in one of the systems standard directories, add the
argument

    INCLUDEPATH += GLM_DIR

to `qmake` with `GLM_DIR` denoting the path where GLM is installed.
Add the `-jN` argument to `make` for building using `N` parallel processes.
Install by running:

    # make install

If no `PREFIX` was given to `qmake`, Dilay is installed to `/usr/local/`.
