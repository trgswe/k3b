/* 
 *
 * Copyright (C) 2003-2007 Sebastian Trueg <trueg@k3b.org>
 * Copyright (C) 2009      Arthur Renato Mello <arthur@mandriva.com>
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



#ifndef _K3B_MOVIX_VIEW_H_
#define _K3B_MOVIX_VIEW_H_

#include <k3bstandardview.h>

namespace K3b {
    class MovixDoc;
}
class KAction;
class KMenu;
class Q3ListViewItem;
class QPoint;
class QLineEdit;

namespace K3b {
    class MovixProjectModel;
}

namespace K3b {
class MovixView : public StandardView
{
    Q_OBJECT

public:
    MovixView( MovixDoc* doc, QWidget* parent = 0 );
    virtual ~MovixView();

private Q_SLOTS:
    void slotContextMenuRequested(Q3ListViewItem*, const QPoint& , int );
    void slotRemoveSubTitleItems();
    void showPropertiesDialog();
    void slotAddSubTitleFile();
    void slotDocChanged();

protected:
    virtual ProjectBurnDialog* newBurnDialog( QWidget* parent = 0 );

private:
    MovixDoc* m_doc;
    K3b::MovixProjectModel *m_model;

    KAction* m_actionProperties;
    KAction* m_actionRemove;
    KAction* m_actionRemoveSubTitle;
    KAction* m_actionAddSubTitle;
    KMenu* m_popupMenu;

    QLineEdit* m_volumeIDEdit;
};
}

#endif
