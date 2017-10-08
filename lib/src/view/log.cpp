/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QGuiApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include "../util.hpp"
#include "view/log.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"

QString ViewLog::logPath () { return QDir::temp ().filePath ("dilay.log"); }

QString ViewLog::crashLogPath () { return QDir::temp ().filePath ("dilay-crash.log"); }

void ViewLog::show (ViewMainWindow& window)
{
  QDialog      dialog (&window);
  QVBoxLayout* layout = new QVBoxLayout;
  QTextEdit*   textEdit = new QTextEdit (&dialog);

  textEdit->setReadOnly (true);
  textEdit->setLineWrapMode (QTextEdit::NoWrap);

  if (QFile (ViewLog::logPath ()).exists ())
  {
    const std::string content = Util::readFile (ViewLog::logPath ().toStdString ());
    textEdit->setPlainText ("-- LOG ----------------------------------");
    textEdit->append (QString::fromStdString (content));
    textEdit->append ("");
  }

  if (QFile (ViewLog::crashLogPath ()).exists ())
  {
    const std::string content = Util::readFile (ViewLog::crashLogPath ().toStdString ());
    textEdit->append ("-- CRASH LOG ----------------------------");
    textEdit->append (QString::fromStdString (content));
    textEdit->append ("");
  }

  QDialogButtonBox* buttons = new QDialogButtonBox (QDialogButtonBox::Close);
  QPushButton*      ctc =
    buttons->addButton (QObject::tr ("Copy to clipboard"), QDialogButtonBox::ActionRole);

  QObject::connect (buttons, &QDialogButtonBox::rejected, [&dialog]() { dialog.reject (); });

  ViewUtil::connect (*ctc, [textEdit]() {
    if (QGuiApplication::clipboard ())
    {
      QGuiApplication::clipboard ()->setText (textEdit->toPlainText ());
    }
    else
    {
      DILAY_WARN ("Could not access clipboard");
    }
  });

  layout->addWidget (textEdit);
  layout->addWidget (buttons);

  dialog.setFixedWidth (100 * QFontMetrics (textEdit->currentFont ()).width ("x"));
  dialog.setFixedHeight (25 * QFontMetrics (textEdit->currentFont ()).height ());
  dialog.setLayout (layout);
  dialog.setWindowTitle (QObject::tr ("Log"));
  dialog.exec ();
}
