/*
 *
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
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

#include "k3bmsinfofetcher.h"

#include <k3bexternalbinmanager.h>
#include <k3bdevicemanager.h>
#include <k3bdevicehandler.h>
#include <k3bdevice.h>
#include <k3bcore.h>
#include <k3bglobals.h>
#include <k3biso9660.h>

#include <klocale.h>
#include <kprocess.h>
#include <kdebug.h>

#include <qstringlist.h>


K3bMsInfoFetcher::K3bMsInfoFetcher( K3bJobHandler* jh, QObject* parent )
  : K3bJob( jh, parent ),
    m_process(0),
    m_device(0),
    m_dvd(false)
{
}


K3bMsInfoFetcher::~K3bMsInfoFetcher()
{
  delete m_process;
}


void K3bMsInfoFetcher::start()
{
  jobStarted();

  emit infoMessage( i18n("Searching previous session"), K3bJob::INFO );

  if( !k3bcore->externalBinManager()->foundBin( "cdrecord" ) ) {
    kDebug() << "(K3bMsInfoFetcher) could not find cdrecord executable";
    emit infoMessage( i18n("Could not find %1 executable.",QString("cdrecord")), K3bJob::ERROR );
    jobFinished(false);
    return;
  }

  if( m_device == 0 ) {
    kDebug() << "(K3bMsInfoFetcher) internal error: No device set!";
    jobFinished(false);
    return;
  }

  //
  // first we try to determine if it is a dvd. If so we need to
  // read the info on our own
  //

  connect( K3bDevice::sendCommand( K3bDevice::DeviceHandler::NG_DISKINFO, m_device ),
	   SIGNAL(finished(K3bDevice::DeviceHandler*)),
	   this,
	   SLOT(slotMediaDetectionFinished(K3bDevice::DeviceHandler*)) );
}


void K3bMsInfoFetcher::getMsInfo()
{
  delete m_process;
  m_process = new KProcess(this);

  const K3bExternalBin* bin = 0;
  if( m_dvd ) {
    // already handled
  }
  else {
    bin = k3bcore->externalBinManager()->binObject( "cdrecord" );

    if( !bin ) {
      emit infoMessage( i18n("Could not find %1 executable.", m_dvd ? QString("dvdrecord") : QString("cdrecord" )), ERROR );
      jobFinished(false);
      return;
    }

    *m_process << bin->path;

    // add the device (e.g. /dev/sg1)
    *m_process << QString("dev=") + K3b::externalBinDeviceParameter(m_device, bin);

    *m_process << "-msinfo";

    // additional user parameters from config
    *m_process << bin->userParameters();

    kDebug() << "***** " << bin->name() << " parameters:\n";
    QStringList args = m_process->program();
    args.removeFirst();
    QString s = args.join(" ");
    kDebug() << s << flush;
    emit debuggingOutput( "msinfo command:", s );


    //   connect( m_process, SIGNAL(readReadyStandardError()),
    // 	   this, SLOT(slotCollectOutput()) );
    connect( m_process, SIGNAL(readReadyStandardOutput()),
	     this, SLOT(slotCollectOutput()) );
    connect( m_process, SIGNAL(finished()),
	     this, SLOT(slotProcessExited()) );

    m_msInfo = QString();
    m_collectedOutput = QString();
    m_canceled = false;

    m_process->setOutputChannelMode(KProcess::SeparateChannels);

    m_process->start();
  }
}


void K3bMsInfoFetcher::slotMediaDetectionFinished( K3bDevice::DeviceHandler* h )
{
  if( h->success() ) {
    m_dvd = h->diskInfo().isDvdMedia();
  }
  else {
    // for now we just default to cd and go on with the detecting
    m_dvd = false;
  }

  if( m_dvd ) {
    if( h->diskInfo().mediaType() & (K3bDevice::MEDIA_DVD_PLUS_RW|K3bDevice::MEDIA_DVD_RW_OVWR) ) {
      // get info from iso filesystem
      K3bIso9660 iso( m_device, h->toc().last().firstSector().lba() );
      if( iso.open() ) {
	unsigned long long nextSession = iso.primaryDescriptor().volumeSpaceSize;
	// pad to closest 32K boundary
        nextSession += 15;
        nextSession /= 16;
        nextSession *= 16;
	m_msInfo.sprintf( "16,%llu", nextSession );

	jobFinished( true );
      }
      else {
	emit infoMessage( i18n("Could not open Iso9660 filesystem in %1.",
			  m_device->vendor() + " " + m_device->description() ), ERROR );
	jobFinished( false );
      }
    }
    else {
      unsigned int lastSessionStart, nextWritableAdress;
      if( m_device->getNextWritableAdress( lastSessionStart, nextWritableAdress ) ) {
	m_msInfo.sprintf( "%u,%u", lastSessionStart+16, nextWritableAdress );
	jobFinished( true );
      }
      else {
	emit infoMessage( i18n("Could not determine next writable address."), ERROR );
	jobFinished( false );
      }
    }
  }
  else // call cdrecord
    getMsInfo();
}


void K3bMsInfoFetcher::slotProcessExited()
{
  if( m_canceled )
    return;

  if (m_process->error() == QProcess::FailedToStart) {
    emit infoMessage( i18n("Could not start %1", m_process->program().at(0)), K3bJob::ERROR );
    jobFinished(false);
    return;
  }

  kDebug() << "(K3bMsInfoFetcher) msinfo fetched";

  // now parse the output
  QString firstLine = m_collectedOutput.left( m_collectedOutput.indexOf('\n') );
  QStringList list = firstLine.split( ',' );
  if( list.count() == 2 ) {
    bool ok1, ok2;
    m_lastSessionStart = list.first().toInt( &ok1 );
    m_nextSessionStart = list[1].toInt( &ok2 );
    if( ok1 && ok2 )
      m_msInfo = firstLine.trimmed();
    else
      m_msInfo = QString();
  }
  else {
    m_msInfo = QString();
  }

  kDebug() << "(K3bMsInfoFetcher) msinfo parsed: " << m_msInfo;

  if( m_msInfo.isEmpty() ) {
    emit infoMessage( i18n("Could not retrieve multisession information from disk."), K3bJob::ERROR );
    emit infoMessage( i18n("The disk is either empty or not appendable."), K3bJob::ERROR );
    jobFinished(false);
  }
  else {
    jobFinished(true);
  }
}


void K3bMsInfoFetcher::slotCollectOutput()
{
  QByteArray a = m_process->readAllStandardOutput();

  emit debuggingOutput( "msinfo", QString::fromLocal8Bit( a ) );

  m_collectedOutput += QString::fromLocal8Bit( a );
}


void K3bMsInfoFetcher::cancel()
{
  // FIXME: this does not work if the devicehandler is running

  if( m_process )
    if( m_process->state() != QProcess::NotRunning) {
      m_canceled = true;
      m_process->kill();
      emit canceled();
      jobFinished(false);
    }
}


#include "k3bmsinfofetcher.moc"
