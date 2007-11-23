/* 
 *
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

#include <config-k3b.h>

#include "k3bexternalencodercommand.h"

#include <k3bcore.h>

#include <kconfig.h>
#include <KConfigGroup>
#include <kstandarddirs.h>
//Added by qt3to4:
#include <Q3ValueList>


Q3ValueList<K3bExternalEncoderCommand> K3bExternalEncoderCommand::readCommands()
{
  KConfig* c = k3bcore->config();
  KConfigGroup grp(c,"K3bExternalEncoderPlugin" );

  Q3ValueList<K3bExternalEncoderCommand> cl;

  QStringList cmds = grp.readEntry( "commands",QStringList() );
  for( QStringList::iterator it = cmds.begin(); it != cmds.end(); ++it ) {
    QStringList cmdString = grp.readEntry( "command_" + *it,QStringList() );
    K3bExternalEncoderCommand cmd;
    cmd.name = cmdString[0];
    cmd.extension = cmdString[1];
    cmd.command = cmdString[2];
    for( unsigned int i = 3; i < cmdString.count(); ++i ) {
      if( cmdString[i] == "swap" )
	cmd.swapByteOrder = true;
      else if( cmdString[i] == "wave" )
	cmd.writeWaveHeader = true;
    }
    cl.append(cmd);
  }

  // some defaults
  if( cmds.isEmpty() ) {
    // check if the lame encoding plugin has been compiled
#ifndef HAVE_LAME
    K3bExternalEncoderCommand lameCmd;
    lameCmd.name = "Mp3 (Lame)";
    lameCmd.extension = "mp3";
    lameCmd.command = "lame -h --tt %t --ta %a --tl %m --ty %y --tc %c - %f"; 

    cl.append( lameCmd );
#endif

    if( !KStandardDirs::findExe( "flac" ).isEmpty() ) {
      K3bExternalEncoderCommand flacCmd;
      flacCmd.name = "Flac";
      flacCmd.extension = "flac";
      flacCmd.command = "flac "
	"-V "
	"-o %f "
	"--force-raw-format "
	"--endian=big "
	"--channels=2 "
	"--sample-rate=44100 "
	"--sign=signed "
	"--bps=16 "
	"-T ARTIST=%a "
	"-T TITLE=%t "
	"-T TRACKNUMBER=%n "
	"-T DATE=%y "
	"-T ALBUM=%m "
	"-";
      
      cl.append( flacCmd );
    }

    if( !KStandardDirs::findExe( "mppenc" ).isEmpty() ) {
      K3bExternalEncoderCommand mppCmd;
      mppCmd.name = "Musepack";
      mppCmd.extension = "mpc";
      mppCmd.command = "mppenc "
	"--standard "
	"--overwrite "
	"--silent "
	"--artist %a "
	"--title %t "
	"--track %n "
	"--album %m "
	"--comment %c "
	"--year %y "
	"- "
	"%f";
      mppCmd.swapByteOrder = true;
      mppCmd.writeWaveHeader = true;
      
      cl.append( mppCmd );
    }
  }

  return cl;
}

