/*
 *
 * $Id$
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2004 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#include "k3bburnprogressdialog.h"

#include "k3bjob.h"
#include <k3bdevice.h>
#include "k3bstdguiitems.h"
#include <k3bthememanager.h>

#include <kglobal.h>
#include <kprogress.h>
#include <klocale.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qframe.h>


K3bBurnProgressDialog::K3bBurnProgressDialog( QWidget *parent, const char *name, bool showSubProgress,
					      bool modal, WFlags wf )
  : K3bJobProgressDialog(parent,name, showSubProgress, modal, wf)
{
  m_labelWritingSpeed = new QLabel( m_frameExtraInfo, "m_labelWritingSpeed" );
  //  m_labelWritingSpeed->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

  m_frameExtraInfoLayout->addWidget( m_labelWritingSpeed, 2, 0 );
  m_frameExtraInfoLayout->addWidget( new QLabel( i18n("Estimated writing speed:"), m_frameExtraInfo ), 1, 0 );

  QFrame* headerFrame = K3bStdGuiItems::purpleFrame( m_frameExtraInfo );
  QHBoxLayout* headerLayout = new QHBoxLayout( headerFrame );
  headerLayout->setMargin( 2 );
  m_labelWriter = new QLabel( headerFrame );
  headerLayout->addWidget( m_labelWriter );
  QFont textLabel14_font( m_labelWriter->font() );
  textLabel14_font.setBold( TRUE );
  m_labelWriter->setFont( textLabel14_font );
  m_labelWriter->setMargin( 3 );

  m_frameExtraInfoLayout->addMultiCellWidget( headerFrame, 0, 0, 0, 3 );
  m_frameExtraInfoLayout->addWidget( new QLabel( i18n("FIFO Buffer:"), m_frameExtraInfo ), 1, 2 );
  m_frameExtraInfoLayout->addWidget( new QLabel( i18n("Device Buffer:"), m_frameExtraInfo ), 2, 2 );

  m_progressWritingBuffer = new KProgress( m_frameExtraInfo, "m_progressWritingBuffer" );
  m_frameExtraInfoLayout->addWidget( m_progressWritingBuffer, 1, 3 );

  m_progressDeviceBuffer = new KProgress( m_frameExtraInfo );
  m_frameExtraInfoLayout->addWidget( m_progressDeviceBuffer, 2, 3 );

  QFrame* line1 = new QFrame( m_frameExtraInfo, "line1" );
  line1->setFrameShape( QFrame::VLine );
  line1->setFrameShadow( QFrame::Sunken );

  m_frameExtraInfoLayout->addMultiCellWidget( line1, 1, 2, 1, 1 );

  if( K3bTheme* theme = k3bthememanager->currentTheme() ) {
    m_labelWriter->setPaletteBackgroundColor( theme->backgroundColor() );
    m_labelWriter->setPaletteForegroundColor( theme->foregroundColor() );
  }
}

K3bBurnProgressDialog::~K3bBurnProgressDialog()
{
}


void K3bBurnProgressDialog::setJob( K3bJob* job )
{
  if( K3bBurnJob* burnJob = dynamic_cast<K3bBurnJob*>(job) )
    setBurnJob(burnJob);
  else
    K3bJobProgressDialog::setJob(job);
}


void K3bBurnProgressDialog::setBurnJob( K3bBurnJob* burnJob )
{
  K3bJobProgressDialog::setJob(burnJob);

  if( burnJob ) {
    connect( burnJob, SIGNAL(bufferStatus(int)), this, SLOT(slotBufferStatus(int)) );
    connect( burnJob, SIGNAL(deviceBuffer(int)), this, SLOT(slotDeviceBuffer(int)) );
    connect( burnJob, SIGNAL(writeSpeed(int, int)), this, SLOT(slotWriteSpeed(int, int)) );
    connect( burnJob, SIGNAL(burning(bool)), m_progressWritingBuffer, SLOT(setEnabled(bool)) );
    connect( burnJob, SIGNAL(burning(bool)), m_progressDeviceBuffer, SLOT(setEnabled(bool)) );
    connect( burnJob, SIGNAL(burning(bool)), m_labelWritingSpeed, SLOT(setEnabled(bool)) );

    if( burnJob->writer() )
      m_labelWriter->setText( i18n("Writer: %1 %2").arg(burnJob->writer()->vendor()).
			      arg(burnJob->writer()->description()) );

    m_labelWritingSpeed->setText( i18n("no info") );
    m_progressWritingBuffer->setFormat( i18n("no info") );
    m_progressDeviceBuffer->setFormat( i18n("no info") );
  }
}


void K3bBurnProgressDialog::slotFinished( bool success )
{
  K3bJobProgressDialog::slotFinished( success );
  if( success ) {
    m_labelWritingSpeed->setEnabled( false );
    m_progressWritingBuffer->setEnabled( false );
    m_progressDeviceBuffer->setEnabled( false );
  }
}


void K3bBurnProgressDialog::slotBufferStatus( int b )
{
  m_progressWritingBuffer->setFormat( "%p%" );
  m_progressWritingBuffer->setValue( b );
}


void K3bBurnProgressDialog::slotDeviceBuffer( int b )
{
  m_progressDeviceBuffer->setFormat( "%p%" );
  m_progressDeviceBuffer->setValue( b );
}


void K3bBurnProgressDialog::slotWriteSpeed( int s, int multiplicator )
{
  m_labelWritingSpeed->setText( QString("%1 KB/s (%2x)").arg(s).arg(KGlobal::locale()->formatNumber((double)s/(double)multiplicator,2)) );
}

#include "k3bburnprogressdialog.moc"
