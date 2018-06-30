/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_CONTEXT_MENU
#define DILAY_VIEW_CONTEXT_MENU

#include <QMenu>
#include <glm/fwd.hpp>

class DynamicMesh;
class SketchMesh;
class ViewMainWindow;

class ViewContextMenu : public QMenu
{
public:
  ViewContextMenu (ViewMainWindow&, DynamicMesh&);
  ViewContextMenu (ViewMainWindow&, SketchMesh&);
  ViewContextMenu (ViewMainWindow&, const glm::ivec2&);
};

#endif
