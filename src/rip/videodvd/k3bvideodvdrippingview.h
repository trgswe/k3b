/* 
 *
 * Copyright (C) 2006 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2007 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _K3B_VIDEODVD_RIPPING_VIEW_H_
#define _K3B_VIDEODVD_RIPPING_VIEW_H_

#include <k3bmediacontentsview.h>
#include <k3bmedium.h>
#include <k3bvideodvd.h>

#include <QLabel>

namespace K3b {
    class VideoDVDRippingTitleListView;
}
class KToolBar;
class QLabel;
class KActionCollection;
class KMenu;
class K3ListView;
class Q3ListViewItem;

namespace K3b {
class VideoDVDRippingView : public MediaContentsView
{
  Q_OBJECT

 public:
  VideoDVDRippingView( QWidget* parent = 0 );
  ~VideoDVDRippingView();

  KActionCollection* actionCollection() const { return m_actionCollection; }

 private Q_SLOTS:
  void slotStartRipping();

  void slotContextMenu( K3ListView*, Q3ListViewItem*, const QPoint& );

  void slotCheckAll();
  void slotUncheckAll();
  void slotCheck();
  void slotUncheck();

 private:
  void reloadMedium();
  void enableInteraction( bool enable );
  void initActions();

  KActionCollection* m_actionCollection;
  KMenu* m_popupMenu;

  KToolBar* m_toolBox;
  QLabel* m_labelLength;
  VideoDVDRippingTitleListView* m_titleView;  

  VideoDVD::VideoDVD m_dvd;
};
}

#endif
