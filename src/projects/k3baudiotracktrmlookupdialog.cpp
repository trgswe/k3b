/*
 *
 * Copyright (C) 2005-2008 Sebastian Trueg <trueg@k3b.org>
 * Copyright (C) 2011 Michal Malek <michalm@jabster.pl>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2008 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "k3baudiotracktrmlookupdialog.h"
#include "k3bmusicbrainzjob.h"

#include <config-k3b.h>

#include "k3bbusywidget.h"
#include "k3baudiotrack.h"
#include "k3baudiofile.h"

#include <QtCore/QDebug>
#include <KDELibs4Support/KDE/KGlobal>
#include <KIconThemes/KIconLoader>
#include <KDELibs4Support/KDE/KInputDialog>
#include <KDELibs4Support/KDE/KLocale>
#include <KDELibs4Support/KDE/KMessageBox>
#include <KDELibs4Support/KDE/KNotification>

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>


K3b::AudioTrackTRMLookupDialog::AudioTrackTRMLookupDialog( QWidget* parent )
    : KDialog( parent ),
      m_loop( 0 )
{
    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    setWindowTitle(i18n("MusicBrainz Query"));
    setButtons(KDialog::Cancel);
    setDefaultButton(KDialog::Cancel);
    setModal(true);
    QGridLayout* grid = new QGridLayout( widget );

    m_infoLabel = new QLabel( widget );
    QLabel* pixLabel = new QLabel( widget );
    pixLabel->setPixmap( KIconLoader::global()->loadIcon( "musicbrainz", KIconLoader::NoGroup, 64 ) );
    pixLabel->setScaledContents( false );

    m_busyWidget = new K3b::BusyWidget( widget );

    grid->addWidget( pixLabel, 0, 0, 2, 1 );
    grid->addWidget( m_infoLabel, 0, 1 );
    grid->addWidget( m_busyWidget, 1, 1 );

    m_mbJob = new K3b::MusicBrainzJob( this );
    connect( m_mbJob, SIGNAL(infoMessage(QString,int)),
             this, SLOT(slotMbJobInfoMessage(QString,int)) );
    connect( m_mbJob, SIGNAL(finished(bool)), this, SLOT(slotMbJobFinished(bool)) );
    connect( m_mbJob, SIGNAL(trackFinished(K3b::AudioTrack*,bool)),
             this, SLOT(slotTrackFinished(K3b::AudioTrack*,bool)) );
}


K3b::AudioTrackTRMLookupDialog::~AudioTrackTRMLookupDialog()
{
}


int K3b::AudioTrackTRMLookupDialog::lookup( const QList<K3b::AudioTrack*>& tracks )
{
    m_mbJob->setTracks( tracks );
    m_mbJob->start();

    m_busyWidget->showBusy(true);
    setModal( true );
    show();

    QEventLoop loop;
    m_loop = &loop;
    loop.exec();
    m_loop = 0;

    return 0;
}


void K3b::AudioTrackTRMLookupDialog::slotMbJobInfoMessage( const QString& message, int )
{
    m_infoLabel->setText( message );
}


void K3b::AudioTrackTRMLookupDialog::slotMbJobFinished( bool )
{
    m_busyWidget->showBusy(false);
    hide();
    if( m_loop )
        m_loop->exit();
}


void K3b::AudioTrackTRMLookupDialog::slotCancel()
{
    enableButton( KDialog::Cancel, false );
    m_mbJob->cancel();
}


void K3b::AudioTrackTRMLookupDialog::slotTrackFinished( K3b::AudioTrack* track, bool success )
{
    if( !success )
        KNotification::event( "TrackDataNotFound",
                              i18n("Audio Project"),
                              i18n("Track %1 was not found in the MusicBrainz database.",
                                   track->trackNumber()) );
}


