/* 
 *
 * Copyright (C) 2005 Sebastian Trueg <trueg@k3b.org>
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

#ifndef _K3B_PROJECTPLUGIN_DIALOG_H_
#define _K3B_PROJECTPLUGIN_DIALOG_H_

#include <k3binteractiondialog.h>

namespace K3b {
    class ProjectPlugin;
}
namespace K3b {
    class ProjectPluginGUIBase;
}
namespace K3b {
    class Doc;
}


namespace K3b {
class ProjectPluginDialog : public InteractionDialog
{
  Q_OBJECT

 public:
  ProjectPluginDialog( ProjectPlugin*, Doc*, QWidget* );
  ~ProjectPluginDialog();
  
 protected Q_SLOTS:
  void slotStartClicked();
  void saveUserDefaults( KConfigGroup config );
  void loadUserDefaults( const KConfigGroup& config );
  void loadK3bDefaults();

 private:
  ProjectPlugin* m_plugin;
  ProjectPluginGUIBase* m_pluginGui;
};
}

#endif
