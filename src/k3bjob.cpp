/***************************************************************************
                          k3bjob.cpp  -  description
                             -------------------
    begin                : Thu May 3 2001
    copyright            : (C) 2001 by Sebastian Trueg
    email                : trueg@informatik.uni-freiburg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "k3bjob.h"

#include <klocale.h>
#include <kprocess.h>

#include <qstringlist.h>


K3bJob::K3bJob( QObject* parent )
  : QObject( parent )
{
}

K3bJob::~K3bJob()
{
}


K3bBurnJob::K3bBurnJob( QObject* parent )
  : K3bJob( parent )
{
}


void K3bBurnJob::parseCdrdaoOutput( KProcess*, char* data, int len )
{
  QString buffer = QString::fromLatin1( data, len );
  QStringList lines = QStringList::split( "\n", buffer );

  if( !m_notFinishedLine.isEmpty() ) {
    qDebug("(K3bBurnJob) joining line: " + m_notFinishedLine + lines.front());

    lines.first().prepend( m_notFinishedLine );
    m_notFinishedLine = "";
  }

  QStringList::iterator it;

  // check if line ends with a newline
  // if not save the last line because it is not finished
  bool notFinishedLine = ( buffer.right(1) != "\n" && buffer.right(1) != "\r" );
  if( notFinishedLine ) {
    qDebug("(K3bBurnJob) found unfinished line: " + lines.last());
    m_notFinishedLine = lines.last();
    it = lines.end();
    --it;
    lines.remove( it );
  }
  
  for( it = lines.begin(); it != lines.end(); ++it ) {
    QString& str = *it;
    if( str[0] == '\r' )
      str = str.mid( 1 );
    parseCdrdaoLine( str );
  }
}


void K3bBurnJob::parseCdrdaoLine( const QString& str )
{
  emit debuggingOutput( "cdrdao", str );

  // find some messages from cdrdao
  // -----------------------------------------------------------------------------------------
  if( (str).startsWith( "Warning" ) || (str).startsWith( "ERROR" ) ) {
    if( str.contains( "No driver found" ) ) {
      emit infoMessage( i18n("No cdrdao driver found."), K3bJob::ERROR );
      emit infoMessage( i18n("Please select one manually in the device settings."), K3bJob::ERROR );
      emit infoMessage( i18n("For most current drives this would be 'generic-mmc'."), K3bJob::ERROR );
    }
    else if( str.contains( "Cannot setup device" ) ) {
      // no nothing...
    }
    else
      parseCdrdaoError( str );
  }
  else if( (str).startsWith( "Executing power" ) ) {
    emit newSubTask( i18n("Executing Power calibration") );
  }
  else if( (str).startsWith( "Power calibration successful" ) ) {
    emit infoMessage( i18n("Power calibration successful"), K3bJob::PROCESS );
    emit newSubTask( i18n("Preparing burn process...") );
  }
  else if( (str).startsWith( "Flushing cache" ) ) {
    emit newSubTask( i18n("Flushing cache") );
  }
  else if( (str).startsWith( "Writing CD-TEXT lead" ) ) {
    emit newSubTask( i18n("Writing CD-Text leadin...") );
  }
  else if( (str).startsWith( "Turning BURN-Proof on" ) ) {
    emit infoMessage( i18n("Turning BURN-Proof on"), K3bJob::PROCESS );
  }
  else if( str.contains( "at speed" ) ) {
    // parse speed
    // compare it to writer()->currentSpeed()
    // if different -> emit infoMessage that requested speed could not be set
    qDebug("(K3bBurnJob) ------ SPEED -------");
  }
  else if( (str).contains( "Writing track" ) ) {
    // a new track has been started
    // let the derived classes do whatever they want...
    startNewCdrdaoTrack();
  }
  // -----------------------------------------------------------------------------------------


  // parse the progress
  // -----------------------------------------------------------------------------------------
  // here "contains" has to be used since cdrdao sometimes "forgets" to do a newline!!
  else if( (str).contains( "Wrote " ) ) {
    // percentage
    int made, size, fifo;
    bool ok;
			
    // --- parse already written mb ------
    int pos1 = 6;
    int pos2 = (str).find("of");
			
    if( pos2 == -1 )
      return; // there is one line at the end of the writing process that has no 'of'
			
    made = (str).mid( 6, pos2-pos1-1 ).toInt( &ok );
    if( !ok )
      qDebug( "(K3bBurnJob) Parsing did not work for: " + (str).mid( 6, pos2-pos1-1 ) );
			
    // ---- parse size ---------------------------
    pos1 = pos2 + 2;
    pos2 = (str).find("MB");
    size = (str).mid( pos1, pos2-pos1-1 ).toInt(&ok);
    if( !ok )
      qDebug( "(K3bBurnJob) Parsing did not work for: " + (str).mid( pos1, pos2-pos1-1 ) );
				
    // ----- parsing fifo ---------------------------
    pos1 = (str).findRev(' ');
    pos2 =(str).findRev('%');
    fifo = (str).mid( pos1, pos2-pos1 ).toInt(&ok);
    if( !ok )
      qDebug( "(K3bBurnJob) Parsing did not work for: " + (str).mid( pos1, pos2-pos1 ) );
			
    emit bufferStatus( fifo );
	
    // let the derived classes do whatever they want...
    createCdrdaoProgress( made, size );
  }

  else {
    parseCdrdaoSpecialLine( str );
  }
}


void K3bBurnJob::parseCdrdaoSpecialLine( const QString& str )
{
  qDebug("(cdrdao) " + str );
}


void K3bBurnJob::parseCdrdaoError( const QString& line )
{
  emit infoMessage( line, K3bJob::ERROR );
}


void K3bBurnJob::createCdrdaoProgress( int made, int size )
{
  emit percent( 100*made/size );
}

void K3bBurnJob::startNewCdrdaoTrack()
{}


#include "k3bjob.moc"
