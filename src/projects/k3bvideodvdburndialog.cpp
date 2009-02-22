/*
 *
 * Copyright (C) 2003-2007 Sebastian Trueg <trueg@k3b.org>
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

#include "k3bvideodvdburndialog.h"
#include "k3bvideodvddoc.h"

#include <k3bdevice.h>
#include <k3bwriterselectionwidget.h>
#include <k3btempdirselectionwidget.h>
#include <k3bcore.h>
#include <k3bwritingmodewidget.h>
#include <k3bglobals.h>
#include <k3bdataimagesettingswidget.h>
#include <k3bisooptions.h>
#include <k3bstdguiitems.h>
#include <k3bglobalsettings.h>

#include <kconfig.h>
#include <klocale.h>
#include <kio/global.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qcheckbox.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <q3whatsthis.h>


K3b::VideoDvdBurnDialog::VideoDvdBurnDialog( K3b::VideoDvdDoc* doc, QWidget *parent )
    : K3b::ProjectBurnDialog( doc, parent ),
      m_doc( doc )
{
    prepareGui();

    setTitle( i18n("Video DVD Project"), i18n("Size: %1", KIO::convertSize(doc->size()) ) );

    // for now we just put the verify checkbox on the main page...
    m_checkVerify = K3b::StdGuiItems::verifyCheckBox( m_optionGroup );
    m_optionGroupLayout->addWidget( m_checkVerify );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_optionGroupLayout->addItem( spacer );

    // create image settings tab
    m_imageSettingsWidget = new K3b::DataImageSettingsWidget( this );
    m_imageSettingsWidget->showFileSystemOptions( false );

    addPage( m_imageSettingsWidget, i18n("Filesystem") );

    m_tempDirSelectionWidget->setSelectionMode( K3b::TempDirSelectionWidget::FILE );

    QString path = m_doc->tempDir();
    if( path.isEmpty() ) {
        path = K3b::defaultTempPath();
        if( m_doc->isoOptions().volumeID().isEmpty() )
            path.append( "image.iso" );
        else
            path.append( m_doc->isoOptions().volumeID() + ".iso" );
    }
    m_tempDirSelectionWidget->setTempPath( path );
}


K3b::VideoDvdBurnDialog::~VideoDvdBurnDialog()
{
}


void K3b::VideoDvdBurnDialog::saveSettings()
{
    K3b::ProjectBurnDialog::saveSettings();

    // save iso image settings
    K3b::IsoOptions o = m_doc->isoOptions();
    m_imageSettingsWidget->save( o );
    m_doc->setIsoOptions( o );

    // save image file path
    m_doc->setTempDir( m_tempDirSelectionWidget->tempPath() );

    m_doc->setVerifyData( m_checkVerify->isChecked() );
}


void K3b::VideoDvdBurnDialog::readSettings()
{
    K3b::ProjectBurnDialog::readSettings();

    if( !doc()->tempDir().isEmpty() )
        m_tempDirSelectionWidget->setTempPath( doc()->tempDir() );
    else
        m_tempDirSelectionWidget->setTempPath( K3b::defaultTempPath() + doc()->name() + ".iso" );

    m_checkVerify->setChecked( m_doc->verifyData() );

    m_imageSettingsWidget->load( m_doc->isoOptions() );

    // in case overburn is enabled we allow some made up max size
    // before we force a DL medium
    if( doc()->size() > 4700372992LL &&
        ( !k3bcore->globalSettings()->overburn() ||
          doc()->size() > 4900000000LL ) )
        m_writerSelectionWidget->setWantedMediumType( K3b::Device::MEDIA_WRITABLE_DVD_DL );
    else
        m_writerSelectionWidget->setWantedMediumType( K3b::Device::MEDIA_WRITABLE_DVD );

    toggleAll();
}


void K3b::VideoDvdBurnDialog::toggleAll()
{
    K3b::ProjectBurnDialog::toggleAll();

    if( m_checkSimulate->isChecked() || m_checkOnlyCreateImage->isChecked() ) {
        m_checkVerify->setChecked(false);
        m_checkVerify->setEnabled(false);
    }
    else
        m_checkVerify->setEnabled(true);
}


void K3b::VideoDvdBurnDialog::loadK3bDefaults()
{
    K3b::ProjectBurnDialog::loadK3bDefaults();

    m_imageSettingsWidget->load( K3b::IsoOptions::defaults() );
    m_checkVerify->setChecked( false );

    toggleAll();
}


void K3b::VideoDvdBurnDialog::loadUserDefaults( const KConfigGroup& c )
{
    K3b::ProjectBurnDialog::loadUserDefaults( c );

    K3b::IsoOptions o = K3b::IsoOptions::load( c );
    m_imageSettingsWidget->load( o );

    m_checkVerify->setChecked( c.readEntry( "verify data", false ) );

    toggleAll();
}


void K3b::VideoDvdBurnDialog::saveUserDefaults( KConfigGroup c )
{
    K3b::ProjectBurnDialog::saveUserDefaults(c);

    K3b::IsoOptions o;
    m_imageSettingsWidget->save( o );
    o.save( c );

    c.writeEntry( "verify data", m_checkVerify->isChecked() );
}


void K3b::VideoDvdBurnDialog::slotStartClicked()
{
    if( m_checkOnlyCreateImage->isChecked() ||
        m_checkCacheImage->isChecked() ) {
        QFileInfo fi( m_tempDirSelectionWidget->tempPath() );
        if( fi.isDir() )
            m_tempDirSelectionWidget->setTempPath( fi.filePath() + "/image.iso" );

        if( QFile::exists( m_tempDirSelectionWidget->tempPath() ) ) {
            if( KMessageBox::warningContinueCancel( this,
                                                    i18n("Do you want to overwrite %1?",m_tempDirSelectionWidget->tempPath()),
                                                    i18n("File Exists"), KGuiItem(i18n("Overwrite")) )
                == KMessageBox::Continue ) {
                // delete the file here to avoid problems with free space in K3b::ProjectBurnDialog::slotStartClicked
                QFile::remove( m_tempDirSelectionWidget->tempPath() );
            }
            else
                return;
        }
    }

    K3b::ProjectBurnDialog::slotStartClicked();
}

#include "k3bvideodvdburndialog.moc"
