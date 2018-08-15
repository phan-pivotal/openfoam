/*=========================================================================

   Program: ParaView
   Module:    pqTreeViewEventPlayer.cxx

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#include "pqTreeViewEventPlayer.h"
#include <QTreeWidget>
#include <QMenu>
#include <QDebug>

//-----------------------------------------------------------------------------
pqTreeViewEventPlayer::pqTreeViewEventPlayer(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqTreeViewEventPlayer::~pqTreeViewEventPlayer()
{
}

//-----------------------------------------------------------------------------0000000
bool pqTreeViewEventPlayer::playEvent(
  QObject* object, const QString& command,
  const QString& arguments, int eventType, bool& error)
{
  QTreeView* treeView= qobject_cast<QTreeView*>(object);
  QMenu* contextMenu= qobject_cast<QMenu*>(object);
  // if this a QMenu (potentially a context menu of the view),
  // we should not move onto parent
  if(!treeView && !contextMenu)
    {
    // mouse events go to the viewport widget
    treeView = qobject_cast<QTreeView*>(object->parent());
    }
  if(!treeView)
    {
    return false;
    }

  QRegExp regExp0("^([\\d\\.]+),(\\d+),(\\d+)$");
  if (command == "setTreeItemCheckState" && regExp0.indexIn(arguments) != -1)
    {
    // legacy command recorded from tree widgets.
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(object);
    if (!treeWidget)
      {
      return false;
      }
    QString str_index = regExp0.cap(1);
    int column = regExp0.cap(2).toInt();
    int check_state = regExp0.cap(3).toInt();

    QStringList indices = str_index.split(".",QString::SkipEmptyParts);
    QTreeWidgetItem* cur_item = NULL;
    foreach (QString cur_index, indices)
      {
      int index = cur_index.toInt();
      if (!cur_item)
        {
        cur_item = treeWidget->topLevelItem(index);
        }
      else
        {
        cur_item = cur_item->child(index);
        }
      if (!cur_item)
        {
        error=true;
        qCritical() << "ERROR: Tree widget must have changed. "
          << "Indices recorded in the test are no longer valid. Cannot playback.";
        return true;
        }
      }
    cur_item->setCheckState(column, static_cast<Qt::CheckState>(check_state));
    return true;
    }

  if (command == "expand" || command == "collapse")
    {
    QString str_index = arguments;
    QModelIndex index = pqTreeViewEventPlayer::GetIndex(str_index, treeView, error);
    if (error)
      {
      return true;
      }
    treeView->setExpanded(index, (command=="expand"));
    return true;
    }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}
