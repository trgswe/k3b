/*
 *
 * Copyright (C) 2009      Gustavo Pichorim Boiko <gustavo.boiko@kdemail.net>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2009 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef K3BSTANDARDVIEW_H
#define K3BSTANDARDVIEW_H

#include <k3bview.h>

/**
 * This view is a standard dir/file view that can be used by K3b projects that 
 * need to show a list of items.
 *
 * It provides a two treeview display: one showing the directory structure of the 
 * project, and the other one showing the files of the currently selected directory.
 *
 * If the project does not need the dir panel (like an audiocd which has no 
 * subdirectories), it can be hidden by calling @ref setShowDirPanel()
 *
 * The project types need to provide a data model for this view to work by
 * calling @ref setModel()
 *
 *@author Gustavo Pichorim Boiko
 */

#include <k3bview.h>
#include <QModelIndex>

class QAbstractItemModel;
class QTreeView;
class QSplitter;
namespace K3b {
    class DirProxyModel;
}

namespace K3b {
class StandardView : public View
{
    Q_OBJECT

public:
    StandardView(Doc* doc, QWidget* parent=0);
    virtual ~StandardView();

protected:
    /**
     * Sets the data model which will be used to manage items in the list
     */
    void setModel(QAbstractItemModel *model);

    /**
     * Sets whether the dir panel (the one shown on the right)  is visible
     * or not.
     */
    void setShowDirPanel(bool show);

    /**
     * Context menu for a list of indexes.
     * This method should be reimplemented in derived classes to get
     * custom context menus for the selected items.
     *
     * The default implementation does nothing (at least for now)
     */
    virtual void contextMenuForSelection(const QModelIndexList &selectedIndexes, const QPoint &pos);

    /**
     * Returns a list of the currently selected indexes.
     * This method is meant to be used together with @ref contextMenuForSelection()
     * meaning that when @ref contextMenuForSelection() is called, currentSelection()
     * will provide the same list as the one used in the previous slot.
     *
     * The main purpose of its existence is for slots connected to actions in the context menu
     * to know which items they should operate in
     */
    QModelIndexList currentSelection() const;

    QModelIndex currentRoot() const;

protected slots:
    void slotCurrentDirChanged();
    void slotCustomContextMenu(const QPoint &pos);

    /**
     * Go to the parent dir (meaning that the parent of the current dir is going
     * to be selected in the dir panel, and its children will be shown in the 
     * file panel
     */
    void slotParentDir();

    /**
     * Remove the currently selected indexes.
     * This slot is meant to be used together with the context menu code
     * being triggered by an action in the derived classes
     */
    void slotRemoveSelectedIndexes();

    /**
     * Edit the currently selected item
     */
    void slotRenameItem();

signals:
    void currentRootChanged( const QModelIndex& newRoot );

private:
    QTreeView* m_dirView;
    QTreeView* m_fileView;
    QSplitter* m_splitter;
    DirProxyModel* m_dirProxy;
    QModelIndexList m_currentSelection;
};
}


#endif
