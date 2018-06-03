/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_SHORTCUT
#define DILAY_VIEW_SHORTCUT

#include <functional>
#include "macro.hpp"

enum class ViewInputEvent;
enum class ViewInputModifier;
class QShortcut;
class QString;
class QWidget;

class ViewShortcut
{
public:
  typedef std::function<void()> Call;

  DECLARE_BIG6 (ViewShortcut, const ViewInputEvent&, const QString&, Call&&)

  ViewShortcut (const ViewInputEvent&, const ViewInputModifier&, const QString&, Call&&);

  ViewInputEvent    event () const;
  ViewInputModifier modifier () const;
  const QString&    label () const;
  const Call&       call () const;
  QShortcut&        toQShortcut (QWidget&) const;

private:
  IMPLEMENTATION
};

#endif
